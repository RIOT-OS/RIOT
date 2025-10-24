/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup net_unicoap_options
 * @brief   Options implementation
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "ztimer.h" /* needed for generating observe value */
#include "byteorder.h"
#include "compiler_hints.h"

#include "net/unicoap/message.h"
#include "net/unicoap/options.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

/**
 * # Terminology
 *
 * ## Quantities
 * - `size`: Number of bytes a given value consists of.
 * - `capacity`: Maximum number of bytes available in a given buffer.
 *    A buffer usually has a capacity and the value stored inside has a size.
 * - `count`: A discrete number of elements in a container.
 * - `width`: Constant number of bits a certain type or field occupies.
 *
 * ## Option Format
 * - `delta`: Positive option number difference to the preceding option, zero if first option.
 * - `length`: Size of option value attached.
 * - `field`: A specific bit or byte region in the option header
 * - `HEAD`: An encoded option's first 1 to 5 bytes, contains `delta nibble` and `length nibble`,
 *    may contain `delta extended` and `length extended`.
 * - `VALUE`: Value attached to option, succeeding `HEAD`.
 * - `delta nibble`: Either `delta` value, if lower than 13, or sentinel value 13 (`0xe`),
 *    or 14 (`0xd`). 15 (`0xf`)  is disallowed.
 * - `delta extended`: For `delta nibble` values greater or equal 13, `delta nibble - 13`.
 *    For `delta` values greater or equal `14 + 0xff`, `delta - 14 - 0xff`
 * - `length nibble`: Either `length` value, if lower than 13, or sentinel value 13 (`0xe`),
 *    or 14 (`0xd`). 15 (`0xf`)  is disallowed.
 * - `length extended`: For `length nibble` values greater or equal 13, `length nibble - 13`.
 *    For `length` values greater or equal `14 + 0xff`, `length - 14 - 0xff`
 *
 * ```
 *   0   1   2   3   4   5   6   7
 * +===============================+
 * |            H E A D            |
 * |               '               |
 * |     delta     '     length    |
 * |     nibble    '     nibble    |
 * |    (4 bits)   '    (4 bits)   |
 * + - - - - - - - + - - - - - - - +
 * /             delta             /
 * \            extended           \
 * /          (0-2 bytes)          /
 * + - - - - - - - - - - - - - - . +
 * /             length            /
 * \            extended           \
 * /          (0-2 bytes)          /
 * +===============================+
 * \                               \
 * /           V A L U E           /
 * \         (length bytes)        \
 * /                               /
 * +===============================+
 * ```
 */

#define DECODE_DELTA_NIBBLE(head)    (head >> 4)
#define ENCODE_DELTA_NIBBLE(nibble)  (nibble << 4)

#define DECODE_LENGTH_NIBBLE(head)    (head & 0xf)
#define ENCODE_LENGTH_NIBBLE(nibble)  (nibble)

/* MARK: - Unsigned Integer Parsing */

#define UINT4_MAX                    (12)
#define UINT12_MAX                   (13 + 0xff)
#define UINT20_MAX                   (14 + 0xff + 0xffff)

#define DECODE_UINT12(extended)      (13 + *extended)
#define ENCODE_UINT12(value, cursor) *cursor = (value - 13)

#define DECODE_UINT20(extended)      14 + 0xff + byteorder_bebuftohs(extended)
#define ENCODE_UINT20(value, cursor) byteorder_htobebufs(cursor, value - (14 + 0xff))

static int _uint_extended_size_from_nibble(uint8_t nibble)
{
    if (nibble < 13) {
        return 0;
    }
    else if (nibble == 13) {
        return sizeof(uint8_t);
    }
    else if (nibble == 14) {
        return sizeof(uint16_t);
    }
    else {
        return -EINVAL;
    }
}

static inline ssize_t _uint_extended_size(uint32_t uint)
{
    if (uint <= UINT4_MAX) {
        return 0;
    }
    else if (uint <= UINT12_MAX) {
        return sizeof(uint8_t);
    }
    else if (uint <= UINT20_MAX) {
        return sizeof(uint16_t);
    }
    else {
        return -EINVAL;
    }
}

ssize_t _uint_read_in_range(uint8_t nibble, const uint8_t** cursor, const uint8_t* end)
{
    const uint8_t* extended = *cursor;
    if (nibble < 13) {
        return nibble;
    }
    else if (nibble == 13) {
        *cursor += sizeof(uint8_t);
        if (*cursor > end) {
            OPTIONS_DEBUG("extended uint: reading out of bounds\n");
            return -EBADOPT;
        }
        return DECODE_UINT12(extended);
    }
    else if (nibble == 14) {
        *cursor += sizeof(uint16_t);
        if (*cursor > end) {
            OPTIONS_DEBUG("extended uint: reading out of bounds\n");
            return -EBADOPT;
        }
        return DECODE_UINT20(extended);
    }
    else {
        OPTIONS_DEBUG("invalid uint nibble\n");
        return -EBADOPT;
    }
}

static ssize_t _uint_read(uint8_t nibble, const uint8_t** cursor)
{
    if (nibble < 13) {
        return nibble;
    }
    else if (nibble == 13) {
        ssize_t value = DECODE_UINT12(*cursor);
        *cursor += sizeof(uint8_t);
        return value;
    }
    else if (nibble == 14) {
        ssize_t value = DECODE_UINT20(*cursor);
        *cursor += sizeof(uint16_t);
        return value;
    }
    else {
        OPTIONS_DEBUG("invalid uint nibble\n");
        return -EBADOPT;
    }
}

static int _uint_write(uint32_t value, uint8_t** cursor)
{
    if (value <= UINT4_MAX) {
        return value;
    }
    else if (value <= UINT12_MAX) {
        ENCODE_UINT12(value, *cursor);
        *cursor += sizeof(uint8_t);
        return 13;
    }
    else if (value <= UINT20_MAX) {
        ENCODE_UINT20(value, *cursor);
        *cursor += sizeof(uint16_t);
        return 14;
    }
    else {
        OPTIONS_DEBUG("invalid uint value\n");
        return -EINVAL;
    }
}

/* MARK: - Private API - */

static inline ssize_t _option_size_diff(uint16_t new_delta, uint8_t current_delta_nibble)
{
    return _uint_extended_size(new_delta) - _uint_extended_size_from_nibble(current_delta_nibble);
}

static inline ssize_t _option_size(uint16_t delta, size_t value_size)
{
    ssize_t delta_field_size = _uint_extended_size(delta);
    if (delta_field_size < 0) {
        return delta_field_size;
    }
    ssize_t length_field_size = _uint_extended_size((uint32_t)value_size);
    if (length_field_size < 0) {
        return length_field_size;
    }

    /* add extended delta field size, extended length field size + actual size of value */
    return sizeof(uint8_t) + delta_field_size + length_field_size + value_size;
}

/**
 * @brief Reads option
 *
 * @param[in,out]   cursor Buffer cursor variable
 * @param[in]       end Pointer to after last buffer element
 * @param[out]      delta Option delta of this option
 * @param[out]      value  Option value
 *
 * @return Size of option value, value starting at `value`
 * @return `-EPAYLD` if @p cursor points at the payload marker
 * @return `-EBADOPT` if reading this option fails
 */
static inline ssize_t _read_option_in_range(const uint8_t** cursor, const uint8_t* end,
                                                   uint16_t* delta, const uint8_t** value)
{
    uint8_t head = **cursor;
    *cursor += 1;

    if (head == UNICOAP_PAYLOAD_MARKER) {
        return -EPAYLD;
    }

    ssize_t d = _uint_read_in_range(DECODE_DELTA_NIBBLE(head), cursor, end);
    if (d < 0) {
        OPTIONS_DEBUG("bad op delta\n");
        return d;
    }

    ssize_t value_size = _uint_read_in_range(DECODE_LENGTH_NIBBLE(head), cursor, end);
    if (value_size < 0) {
        OPTIONS_DEBUG("bad op len\n");
        return value_size;
    }

    if (value) {
        *value = *cursor;
    }

    if (delta) {
        *delta = d;
    }
    *cursor += value_size;
    if (*cursor > end) {
        OPTIONS_DEBUG("bad op val len\n");
        return -EBADOPT;
    }

    return value_size;
}

/**
 * @brief Reads option
 *
 * @param[in,out]   cursor Buffer cursor variable
 * @param[out]   value  Option value
 *
 * @return Size of option value, value starting at `value`
 * @return `-EPAYLD` if @p cursor points at the payload marker
 * @return `-EBADOPT` if reading this option fails
 */
static inline ssize_t _read_option(const uint8_t** cursor, const uint8_t** value)
{
    uint8_t head = **cursor;
    *cursor += 1;

    ssize_t delta_size = _uint_extended_size(DECODE_DELTA_NIBBLE(head));
    if (delta_size < 0) {
        OPTIONS_DEBUG("bad op delta\n");
        return -EBADOPT;
    }
    *cursor += delta_size;

    ssize_t value_size = _uint_read(DECODE_LENGTH_NIBBLE(head), cursor);
    if (value_size < 0) {
        OPTIONS_DEBUG("bad op value size\n");
        return value_size;
    }

    *value = *cursor;
    *cursor += value_size;
    return value_size;
}

static void _write_option(uint8_t** cursor, uint16_t delta, const uint8_t* value, size_t value_size)
{
    uint8_t* head = *cursor;
    *cursor += 1;
    *head = ENCODE_DELTA_NIBBLE(_uint_write(delta, cursor));
    *head |= ENCODE_LENGTH_NIBBLE(_uint_write((uint32_t)value_size, cursor));
    if (value) {
        memcpy(*cursor, value, value_size);
        *cursor += value_size;
    }
}

static inline void _write_head_partial(uint8_t** cursor, uint16_t delta, uint8_t length_nibble)
{
    uint8_t* head = *cursor;
    *cursor += 1;
    *head = ENCODE_DELTA_NIBBLE(_uint_write(delta, cursor)) | ENCODE_LENGTH_NIBBLE(length_nibble);
}

/**
 * @brief Moves options in storage buffer freely in both directions
 *
 * @param[in,out] options Option struct whose storage buffer to mutate
 * @param[in,out] src Start of memory region to move
 * @param[in,out] dest Start of target memory region where to move @p src
 *
 * Moves memory region starting at @p src and ending the last of the currently used buffer
 * to @p dest.
 *
 * @pre @p dest lies within storage buffer capacity
 * @pre @p src lies within storage buffer capacity
 *
 * @warning You must perform runtime bounds checks at the call site.
 */
static inline
void _move_options_in_storage_buffer(unicoap_options_t* options, uint8_t* dest, uint8_t* src)
{
    size_t remainder = options->storage_size - ((uintptr_t)src - (uintptr_t)options->entries->data);
    assert(dest < options->entries->data + options->storage_capacity);
    assert(src < (options->entries->data + options->storage_capacity));
    assert((src + remainder) < (options->entries->data + options->storage_capacity));
    memmove(dest, src, remainder);
}

/**
 * @brief Adds diff to pointers in lookup array ('entries')
 *
 * @param[in,out] options Options with lookup array ('entries')
 * @param i Index in lookup array where to begin applying @p data_diff (equals option index)
 * @param data_diff Positive or negative difference in storage buffer at the `i`-th option.
 *
 * Positive diff shifts towards higher addresses, negative diff towards lower ones.
 *
 * Use this function to update entries in the lookup array after you have moved options in the
 * storage buffer using @ref _move_options_in_storage_buffer.
 */
static inline void _update_option_entries_with_storage_diff(unicoap_options_t* options,
                                                            size_t i, ssize_t data_diff)
{
    for (; i < options->option_count; i += 1) {
        options->entries[i].data += data_diff;
    }
}

/**
 * @brief Shifts entries in lookup array ('entries') by given diff
 *
 * @param[in,out] options Options with lookup array ('entries')
 * @param i Index in lookup array where to begin shifting (equals index of option)
 * @param diff Positive or negative difference.
 *
 * Positive diff shifts towards higher addresses, negative diff towards lower ones.
 *
 * @warning You must perform bounds checks at the call site.
 */
static inline void _shift_option_entries(unicoap_options_t* options, size_t i, ssize_t diff)
{
    assert(i < CONFIG_UNICOAP_OPTIONS_MAX);
    unicoap_option_entry_t* e = &options->entries[i];
    assert((uint8_t*)(e + diff) < (uint8_t*)(options->entries) + sizeof(options->entries));
    memmove(e + diff, e, (options->option_count - i) * sizeof(unicoap_option_entry_t));
    options->option_count += diff;
}

/**
 * @brief Shifts options in storage buffer and updates entries in lookup array accordingly
 *
 * @param[in,out] options Options struct whose storage buffer to mutate, with lookup
 * array ('entries')
 *
 * @param i Index in lookup array where to begin shifting (equals index of option)
 * @param data_diff Positive or negative difference.
 *
 * Positive diff shifts towards higher addresses, negative diff towards lower ones.
 *
 * This function checks if the storage buffer has enough remaining capacity and is roughly equal
 * to calling @ref _move_options_in_storage_buffer first, followed by
 * @ref _update_option_entries_with_storage_diff.
 *
 * @remark If @p i is equal to the end index
 * (one index after the last valid index, i.e., the count), this function does not shift as there is
 * nothing to shift at all.
 */
static int _shift_options(unicoap_options_t* options, size_t i, ssize_t data_diff)
{
    size_t new_size = options->storage_size + data_diff;
    if (new_size > options->storage_capacity) {
        OPTIONS_DEBUG("buf too small, " _UNICOAP_NEED_HAVE "\n", new_size,
                      options->storage_capacity);
        return -ENOBUFS;
    }

    if (i < options->option_count) {
        uint8_t* data = options->entries[i].data;
        _move_options_in_storage_buffer(options, data + data_diff, data);
        _update_option_entries_with_storage_diff(options, i, data_diff);
    } else {
        /* Nothing to shift, see @remark above */
    }

    options->storage_size = new_size;
    return 0;
}

static int _find_option_index(const unicoap_options_t* options, uint16_t number)
{
    size_t count = options->option_count;
    for (size_t i = 0; i < count; i += 1) {
        uint16_t n = options->entries[i].number;
        if (n > number || !options->entries[i].data) {
            return -1;
        }
        if (n == number) {
            return (int)i;
        }
    }
    return -1;
}

ssize_t unicoap_pdu_parse_options_and_payload(uint8_t* cursor, const uint8_t* end,
                                              unicoap_message_t* message)
{
    uint8_t* start = cursor;
    assert(message->options);
    /* e points to the current entry in the lookup array */
    unicoap_option_entry_t* e = message->options->entries;
    unicoap_option_number_t option_number = 0;
    message->options->option_count = 0;

    /* Set the pointer of the first entry in the lookup array to the start of the storage buffer. */
    message->options->entries->data = start;
    message->options->storage_size = 0;

    /* In case someone wants to parse a message and that message does not contains an FF
     * payload separator, they can actually set/add options and mutate the storage buffer.
     * If there is a payload separator however, below at (<-) we adjust the capacity to exactly
     * the size of options blob. I.e., you could still mutate the options buffer by setting
     * options (or removing, and then setting/adding), but you won't be able to write beyond
     * the last byte before the FF separator.
     *
     * Parsing and getters in unicoap do not mutate the storage buffer.
     * After having parsed a message, callers can still decide not to treat their own buffer
     * as const and add/set/remove options (mutate). This of course requires global reasoning in
     * the application. If the original buffer passed to this function was considered const
     * by the applications, so should the options. If it wasn't considered const, options can be
     * mutated.
     */
    message->options->storage_capacity = (uintptr_t)end - (uintptr_t)start;

    while (cursor < end) {
        uint8_t* option_start = cursor;
        uint16_t delta = 0;

        /* Casting from non-const to const is safe. */
        ssize_t option_size = _read_option_in_range((const uint8_t **)&cursor, end, &delta, NULL);

        if (option_size == -EPAYLD) {
            /* we hit the payload marker */
            message->payload_size = (size_t)(end - cursor);
            message->payload = message->payload_size > 0 ? cursor : NULL;
            message->options->storage_capacity = (uintptr_t)cursor - (uintptr_t)start; /* (<-) */
            OPTIONS_DEBUG("payload size = %" PRIuSIZE " opts capacity = %" PRIuSIZE "\n",
                          message->payload_size, message->options->storage_capacity);
            return 0;
        }
        else if (option_size >= 0) {
            option_number += delta;
            if (message->options->option_count >= CONFIG_UNICOAP_OPTIONS_MAX) {
                OPTIONS_DEBUG("unicoap: max nr of options exceeded\n");
                return -ENOBUFS;
            }

            e->number = option_number;
            e->data = option_start;
            e->size = (uintptr_t)cursor - (uintptr_t)option_start;
            message->options->storage_size += e->size;

            /*
            OPTIONS_DEBUG("option nr=%u label=%s\n", (unsigned int)option_number,
             unicoap_string_from_option_number(option_number));
             */
            e += 1;
            message->options->option_count += 1;
        }
        else {
            /* return error */
            return option_size;
        }
    }
    return 0;
}

/* MARK: - Public API - */

bool unicoap_options_contains(const unicoap_options_t* options, unicoap_option_number_t number)
{
    return _find_option_index(options, number) >= 0;
}

ssize_t unicoap_options_get(const unicoap_options_t* options, unicoap_option_number_t number,
                           const uint8_t** value)
{
    int i = _find_option_index(options, number);
    if (i < 0) {
        return -ENOENT;
    }

    const uint8_t* cursor = options->entries[i].data;
    return _read_option(&cursor, value);
}

ssize_t unicoap_options_copy_value(const unicoap_options_t* options, unicoap_option_number_t number,
                                   uint8_t* dest, size_t capacity)
{
    assert(dest && capacity > 0);

    const uint8_t* src = NULL;
    ssize_t size = unicoap_options_get(options, number, &src);
    if (size < 0) {
        return size;
    }
    if ((size_t)size > capacity) {
        return -ENOBUFS;
    }
    memcpy(dest, src, size);
    return size;
}

ssize_t unicoap_options_copy_values_joined(const unicoap_options_t* options,
                                           unicoap_option_number_t number,
                                           uint8_t* buffer, size_t capacity, uint8_t separator)
{
    assert(buffer && capacity > 0);

    /* We are not mutating options here and the iterator does not escape. */
    unicoap_options_iterator_t iterator;
    unicoap_options_iterator_init(&iterator, (unicoap_options_t*)options);
    size_t size = 0;

    ssize_t res = 0;
    const uint8_t* component;
    while ((res = unicoap_options_get_next_by_number(&iterator, number, &component)) > 0) {
        if (capacity < (size_t)(size + res + 1)) {
            return -ENOBUFS;
        }

        /* Only insert separator between values, not at the beginning */
        if (size > 0) {
            *buffer = separator;
            buffer += 1;
            size += 1;
        }
        memcpy(buffer, component, res);
        buffer += res;
        size += res;
    }

    return size;
}

int unicoap_options_add(unicoap_options_t* options, unicoap_option_number_t number,
                        const uint8_t* value, size_t value_size)
{
    unicoap_option_entry_t* opts = options->entries;
    assert(value_size <= UNICOAP_UINT_MAX);
    assert(opts->data);

    size_t count = options->option_count;
    if (count >= CONFIG_UNICOAP_OPTIONS_MAX) {
        OPTIONS_DEBUG("limit of %" PRIuSIZE " exceeded\n", (size_t)CONFIG_UNICOAP_OPTIONS_MAX);
        return -ENOBUFS;
    }

    size_t i = 0;
    while ((i < count) && (opts[i].number <= number)) {
        i += 1;
    }

    unicoap_option_entry_t* e = &opts[i];
    uint16_t delta = (i > 0) ? (number - opts[i - 1].number) : number;
    const ssize_t option_size = _option_size(delta, value_size);

    if (i == count) {
        size_t storage_size = options->storage_size + option_size;
        /* Option to be inserted is trailing option, can just add after last option */
        if (storage_size > options->storage_capacity) {
            OPTIONS_DEBUG("buf too small to insert opt " _UNICOAP_NEED_HAVE "\n",
                          storage_size, options->storage_capacity);
            return -ENOBUFS;
        }

        uint8_t* dest = (e->data = opts->data + options->storage_size);
        _write_option(&dest, delta, value, value_size);

        options->storage_size = storage_size;
        options->option_count += 1;
    }
    else {
        /* The successor's option delta will change */
        uint8_t* cursor = e->data;
        uint8_t length_nibble = DECODE_LENGTH_NIBBLE(*cursor);
        uint16_t new_delta = e->number - number;

        ssize_t diff = _option_size_diff(new_delta, DECODE_DELTA_NIBBLE(*cursor));
        ssize_t total_diff = option_size + diff;
        if (_shift_options(options, i, total_diff) < 0) {
            OPTIONS_DEBUG("storage too small for new option\n");
            return -ENOBUFS;
        }

        /* The size of the option (not just the value) grows if its new delta is wider than before.
         * If it is smaller, the entire option shrinks. */
        e->size += diff;

        /* If the options grows due to the delta field becoming wider, we extend the option in the
         * leading direction. Otherwise, we would need to move once to adjust for the delta width
         * and once again to accommodate the new option. We already created exactly enough space to
         * fit the new delta width AND the new option by calling
         * _shift_options(options, i, total_diff) above.
         *
         * Before:
         * ... value ] [ Nibbles (1B) | extended delta | extended length | value ]
         *            ^\_______ shift remainder ______>
         *            |
         *    New opt goes here
         *
         * After making space for new option + changed delta of successor option:
         * ... value ]                    [ Nibbles (1B) | extended delta | extended length | value]
         *            ^                   ^
         *            |                   |
         *    New opt goes here        e->data
         *
         * If successor opt delta width grows, e->data must move in leading direction
         * ... value ] ________________ [#######[ Nibbles old (1B) | extended old delta | ... ]
         *            ^       NEW       ^ (new) ^ (old)
         *            |                  \      |
         *    New opt goes here           \__ e->data
         *
         * If successor opt delta width shrinks, e->data must move in trailing direction
         * ... value ] ________________ [Nibbles old (1B) | extended old delta | ... ]
         *            ^    NEW   ^ (old) ^ (new)
         *            |          |      /
         *    New opt goes here  e->data
         */
        e->data -= diff;
        _shift_option_entries(options, i, 1);

        e->data = cursor;
        _write_option(&cursor, delta, value, value_size);
        _write_head_partial(&cursor, new_delta, length_nibble);
    }

    e->number = number;
    e->size = option_size;
    return 0;
}

int unicoap_options_add_values_joined(unicoap_options_t* options, unicoap_option_number_t number,
                               const uint8_t* buffer, size_t size, uint8_t separator)
{
    int res = 0;
    const uint8_t* end = buffer + size;
    if (*buffer == separator) {
        buffer += 1;
    }
    const uint8_t* start = buffer;
    while (buffer <= end) {
        /* Add option when encountering separator and when there is nothing following.
         * This would be bar in /foo/bar. */
        if ((*buffer == separator) || ((buffer != start) && (buffer == end))) {
            if ((res = unicoap_options_add(options, number, start,
                                           (uintptr_t)buffer - (uintptr_t)start)) < 0) {
                return res;
            }
            buffer += 1;
            start = buffer;
        }
        else {
            buffer += 1;
        }
    }
    return res;
}

int unicoap_options_set(unicoap_options_t* options, unicoap_option_number_t number,
                        const uint8_t* value, size_t value_size)
{
    unicoap_option_entry_t* opts = options->entries;
    assert(value_size <= UNICOAP_UINT_MAX);
    assert(opts->data);

    int i = _find_option_index(options, number);
    if (i < 0) {
        return unicoap_options_add(options, number, value, value_size);
    }
    else {
        unicoap_option_entry_t* e = &opts[i];
        uint16_t delta = (i > 0) ? (number - opts[i - 1].number) : number;

        size_t option_size = _option_size(delta, value_size);
        if (_shift_options(options, i + 1, (int)option_size - (int)e->size) < 0) {
            OPTIONS_DEBUG("storage too small for new option value\n");
            return -ENOBUFS;
        }

        e->size = option_size;
        uint8_t* dest = e->data;
        _write_option(&dest, delta, value, value_size);
    }
    return 0;
}

int unicoap_options_remove_all(unicoap_options_t* options, unicoap_option_number_t number)
{
    OPTIONS_DEBUG("attempting to remove %s (nr=%u)\n", unicoap_string_from_option_number(number),
                  number);
    int i = _find_option_index(options, number);
    if (unlikely(i < 0)) {
        return 0;
    }

    unicoap_option_entry_t* removed_entry = &options->entries[i];

    int count = (int)options->option_count;
    /* index of next option */
    int next_i = i + 1;
    while (next_i < count && options->entries[next_i].number == number) {
        next_i += 1;
    }

    ssize_t index_offset = next_i - i;

    if (next_i == count) {
        /* just drop last entry, no succeeding options */
        options->storage_size -= removed_entry->size;
        options->option_count -= index_offset;
    }
    else {
        /* the successor's option delta will change */
        unicoap_option_entry_t* next = removed_entry + index_offset;
        unicoap_option_entry_t* prev = (i > 0) ? (&options->entries[i - 1]) : NULL;
        uint8_t length_nibble = DECODE_LENGTH_NIBBLE(*next->data);
        uint16_t new_delta = next->number - (prev ? prev->number : 0);

        /* This diff corresponds to the number of bytes the next option
         * grows (positive) or shrinks by (negative). */
        ssize_t diff = _option_size_diff(new_delta, DECODE_DELTA_NIBBLE(*next->data));
        /* The extended delta field's size might change due to a new
         * delta value. */
        ssize_t total_diff = diff;
        for (int k = i; k < next_i; k += 1) {
            total_diff -= options->entries[k].size;
        }

        size_t new_size = options->storage_size + total_diff;
        if (new_size > options->storage_capacity) {
            OPTIONS_DEBUG("storage too small to remove option (delta of next option changes)\n");
            return -ENOBUFS;
        }

        /* shift, starting at extended length field
         * [ Nibbles (1B) | extended delta | extended length | value ] [ Nib...
         *   \______ changes anyway ______/^\_______ shift remainder _______
         *                                 |
         *                               cursor
         *
         * Fig. 1: Cursor for shifting the remaining options blob
         */
        uint8_t* cursor =
            next->data + 1 + _uint_extended_size_from_nibble(DECODE_DELTA_NIBBLE(*next->data));

        /* next option is shifted to the start of the first removed option
         * (there might be multiple (repeatable) options with this number */
        next->data = removed_entry->data;
        next->size += diff; /* amount for changes in ext. delta field size */

        /* this is the number of bytes following the cursor in Fig. 1 */
        size_t remainder_size =
            options->storage_size - ((uintptr_t)cursor - (uintptr_t)options->entries->data);
        /* need to move next and all options after that at the same time,
         * otherwise we may overwrite data of another */
        memmove(cursor + total_diff, cursor, remainder_size);
        options->storage_size = new_size;

        /* rewrite Nibbles (1 byte) and extended delta field */
        cursor = removed_entry->data;
        _write_head_partial(&cursor, new_delta, length_nibble);

        /* remove entry from options lookup array */
        _shift_option_entries(options, i + index_offset, -index_offset);
        _update_option_entries_with_storage_diff(options, i + 1, total_diff);
    }
    return 0;
}

/* MARK: - Iterator */

ssize_t unicoap_options_get_next(unicoap_options_iterator_t* iterator,
                                 unicoap_option_number_t* number, const uint8_t** value)
{
    assert(iterator->options);
    assert(iterator->options->entries->data);

    if (iterator->index >= iterator->options->option_count) {
        return -1;
    }

    unicoap_option_entry_t* e = &iterator->options->entries[iterator->index];
    const uint8_t* cursor = e->data;
    ssize_t value_size = _read_option(&cursor, value);
    if (value_size < 0) {
        return value_size;
    }

    if (number) {
        *number = iterator->options->entries[iterator->index].number;
    }
    iterator->index += 1;
    return value_size;
}

ssize_t unicoap_options_get_next_by_number(unicoap_options_iterator_t* iterator,
                                           unicoap_option_number_t number, const uint8_t** value)
{
    unicoap_options_t* options = iterator->options;
    while ((iterator->index < options->option_count) &&
           (options->entries[iterator->index].number != number)) {
        iterator->index += 1;
    }

    return unicoap_options_get_next(iterator, NULL, value);
}

ssize_t unicoap_options_get_next_query_by_name(unicoap_options_iterator_t* iterator,
                                               unicoap_option_number_t number, const char* name,
                                               const char** value)
{
    const uint8_t* _name = NULL;
    const uint8_t* component = NULL;
    ssize_t res = -1;
    while ((res =unicoap_options_get_next_by_number(iterator, number,
                                                    (const uint8_t**)&component)) >= 0) {
        assert(component);
        _name = component;

        while (res > 0 && *component != '=') {
            component += 1;
            res -= 1;
        }

        if (strncmp(name, (char*)_name, (uintptr_t)component - (uintptr_t)_name) != 0) {
            continue;
        }

        if (res > 0) {
            assert(*component == '=');
            component += 1;
            res -= 1;
            *value = (const char*)component;
        }
        else {
            *value = NULL;
        }

        return res;
    }

    return -ENOENT;
}

static inline uint32_t _ntoh_variable(const uint8_t* variable_network_uint, size_t size)
{
    switch (size) {
        case sizeof(uint8_t):
            return (uint32_t)*variable_network_uint;
        case sizeof(uint16_t):
            return (uint32_t)byteorder_bebuftohs(variable_network_uint);
        case UNICOAP_UINT24_SIZE:
            return ((uint32_t)byteorder_bebuftohs(variable_network_uint) << 8) |
                ((uint32_t)variable_network_uint[2]);
        case sizeof(uint32_t):
            return byteorder_bebuftohl(variable_network_uint);
        default:
            UNREACHABLE();
            return 0;
    }
}

ssize_t _unicoap_options_get_variable_uint(const unicoap_options_t* options,
                                          unicoap_option_number_t number, uint32_t* uint,
                                          size_t max_size)
{
    assert(max_size <= sizeof(uint32_t));
    const uint8_t* src = NULL;
    ssize_t size = unicoap_options_get(options, number, &src);
    if (size < 0) {
        return size;
    }
    if ((size_t)size > max_size) {
        return -EBADOPT;
    }

    *uint = _ntoh_variable(src, size);
    return size;
}

static size_t _encode_variable_uint(uint32_t* value)
{
    uint8_t* dest = (uint8_t*)value;
    size_t size = 0;

    /* count number of used bytes */
    uint32_t tmp = *value;
    while (tmp) {
        size += 1;
        tmp >>= 8;
    }

    /* convert to network byte order */
    tmp = htonl(*value);

    /* copy bytewise, starting with first actually used byte */
    *value = 0;
    uint8_t* cursor = (uint8_t*)&tmp;
    cursor += (sizeof(uint32_t) - size);
    for (unsigned n = 0; n < size; n++) {
        *dest = *cursor;
        dest += 1;
        cursor += 1;
    }

    return size;
}

int unicoap_options_set_uint(unicoap_options_t* options, unicoap_option_number_t number,
                             uint32_t value)
{
    size_t size = _encode_variable_uint(&value);
    return unicoap_options_set(options, number, (uint8_t*)&value, size);
}

int unicoap_options_add_uint(unicoap_options_t* options, unicoap_option_number_t number,
                             uint32_t value)
{
    size_t size = _encode_variable_uint(&value);
    return unicoap_options_add(options, number, (uint8_t*)&value, size);
}

int unicoap_options_set_observe_generated(unicoap_options_t* options)
{
    /* generate notification value */
    return unicoap_options_set_observe(
        options, (ztimer_now(ZTIMER_MSEC) >> UNICOAP_OBSERVE_TICK_EXPONENT) & 0xFFFFFF);
}

ssize_t unicoap_options_swap_storage(unicoap_options_t* options, uint8_t* destination,
                                     size_t capacity)
{
    if (options->storage_size == 0) {
        return 0;
    }
    assert(options->entries->data);

    if (options->storage_size > capacity) {
        OPTIONS_DEBUG("no buffer space to copy options\n");
        return -ENOBUFS;
    }

    options->storage_capacity = capacity;
    memcpy(destination, options->entries->data, options->storage_size);

    size_t offset = 0;
    for (size_t i = 0; i < options->option_count; i += 1) {
        options->entries[i].data = destination + offset;
        offset += options->entries[i].size;
    }

    return 0;
}
