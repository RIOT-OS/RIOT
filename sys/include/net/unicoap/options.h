/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>

#include "byteorder.h"

#include "net/unicoap/constants.h"
#include "net/unicoap/util_macros.h"

/**
 * @defgroup net_unicoap_options Options
 * @ingroup  net_unicoap_message
 * @brief    Access options from a CoAP message
 * @{
 *
 * To allocate an options object, use @ref UNICOAP_OPTIONS_ALLOC. Provide a name for the variable
 * and an estimate for the total number of bytes needed to represent all options. If you are unsure,
 * consider using @ref UNICOAP_OPTIONS_ALLOC_DEFAULT.
 *
 * After having allocated an options object, you can access individual options using the many
 * accessor methods. Refer to @ref net_unicoap_options_predefined for typed methods adjusted to
 * each predefined option or @ref net_unicoap_options_generic for options where predefined methods
 * are not provided.
 *
 * **Example**:
 * ```c
 * UNICOAP_OPTIONS_ALLOC(my_options, 20);
 *
 * int res = 0;
 * if ((res = unicoap_options_add_uri_queries_string(&my_options, "a=1&b=2")) < 0) {
 *     printf("error: could not add Uri-Query options: %i (%s)\n", res, strerror(-res));
 * }
 *
 * if (unicoap_options_set_accept(&my_options, UNICOAP_FORMAT_TEXT) < 0) {
 *     printf("error: could set Accept option: %i (%s)\n", res, strerror(-res));
 * }
 * ```
 *
 * Should you need to dump all options, use @ref unicoap_options_t::unicoap_options_dump_all.
 */

/**
 * @file
 * @brief  Options header
 * @author Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - Essentials */
/**
 * @name Essentials
 * @{
 */
/**
 * @brief Helper struct for options parser
 */
typedef struct {
    /**
     * @brief Pointer into options storage where this option starts.
     * @note This is not the start of the given option's value.
     */
    uint8_t* data;

    /**
     * @brief Number of bytes this option is comprised of
     */
    uint16_t size;

    /**
     * @brief Option number
     */
    unicoap_option_number_t number;
} unicoap_option_entry_t;

/**
 * @brief CoAP options container
 *
 * This structure provides a *view* into a storage buffer
 * containing [RFC7252](https://datatracker.ietf.org/doc/html/rfc7252)-encoded options.
 *
 * @see @ref net_unicoap_options
 */
typedef struct {
    /* TODO: OSCORE: read-only due to options stored in two different buffers */

    /**
     * @brief Current size of encoded options
     */
    size_t storage_size;

    /**
     * @brief Available capacity in options storage buffer
     *
     * If capacity is insufficient, option accessor functions will return `-ENOBUFS`.
     */
    size_t storage_capacity;

    /**
     * @brief Helper array used to encode and decode options into options storage.
     * @internal
     *
     * @see @ref CONFIG_UNICOAP_OPTIONS_MAX
     */
    unicoap_option_entry_t entries[CONFIG_UNICOAP_OPTIONS_MAX];

    /**
     * @brief Number of options present
     *
     * This is also the number of entries in the @ref unicoap_options_t::entries array.
     */
    size_t option_count;
} unicoap_options_t;

/**
 * @brief Initializes the given options structure.
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options struct to initialize
 * @param[in] storage A buffer unicoap will use to store option values in
 * @param capacity The number of usable bytes in @p storage
 */
static inline void unicoap_options_init(unicoap_options_t* options, uint8_t* storage,
                                        size_t capacity)
{
    options->entries->data = storage;
    options->option_count = 0;
    options->storage_size = 0;
    options->storage_capacity = capacity;
}

/**
 * @brief Determines whether the given options container has one or more options with the given
 *        number.
 *
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 * @param number The option number
 *
 * @returns `true` if @p options contains an options with the given number
 */
bool unicoap_options_contains(const unicoap_options_t* options, unicoap_option_number_t number);

/**
 * @brief Removes all options
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options
 */
static inline void unicoap_options_clear(unicoap_options_t* options)
{
    options->option_count = 0;
    options->storage_size = 0;
}
/** @} */

/* MARK: - Allocating option buffers */
/**
 * @name Allocating option buffers
 * @{
 */
#ifndef DOXYGEN
#  define _UNICOAP_OPTIONS_ALLOC(_buf, _name, capacity) \
      uint8_t _buf[capacity];                           \
      unicoap_options_t _name;                          \
      unicoap_options_init(&_name, _buf, capacity);
#endif

/**
 * @brief Allocates options with buffer capacity
 *
 * @param name Name of the variable storing the options structure
 * @param capacity Storage buffer capacity in bytes
 *
 * Allocates a new @ref unicoap_options_t container and a storage buffer with
 * the given capacity, then calls @ref unicoap_options_t::unicoap_options_init.
 */
#define UNICOAP_OPTIONS_ALLOC(name, capacity) \
    _UNICOAP_OPTIONS_ALLOC(_CONCAT3(name, _storage, __LINE__), name, capacity)

/**
 * @brief Allocates options with default capacity
 *
 * @param name Name of the variable storing the options structure
 *
 * Allocates a new @ref unicoap_options_t container and a storage buffer with
 * @ref CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY,
 * then calls @ref unicoap_options_t::unicoap_options_init.
 */
#define UNICOAP_OPTIONS_ALLOC_DEFAULT(name) \
    UNICOAP_OPTIONS_ALLOC(name, CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY)
/** @} */

/* MARK: - Option characteristics */
/**
 * @name Option characteristics
 * @{
 */
/**
 * @brief Determines whether the given option is considered critical
 *
 * Criticality is independent from the corresponding option's value.
 * As per [CoAP RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252),
 * critical options must never be silently ignored.
 *
 * @param option_number The option number
 * @returns A boolean value indicating whether options with the given number are critical
 */
static inline bool unicoap_option_is_critical(unicoap_option_number_t option_number)
{
    return option_number & 1;
}

/**
 * @brief Determines whether the given option is safe to forward
 *
 * Whether an option is safe to forward is independent from the corresponding option's value.
 *
 * @param option_number The option number
 * @returns A boolean value indicating whether options with the given number are safe to forward
 */
static inline bool unicoap_option_is_safe_to_forward(unicoap_option_number_t option_number)
{
    return (option_number & 2) == 0;
}

/**
 * @brief Determines whether the given option is **not** intended to be part of the cache key
 *
 * Whether an option is considered
 * [NoCacheKey](https://datatracker.ietf.org/doc/html/rfc7252#section-5.4.2)
 * is independent from the option's value.
 *
 * @param option_number The option number
 * @returns A boolean value indicating whether options with the given number are **not** a cache key
 */
static inline bool unicoap_option_is_no_cache_key(unicoap_option_number_t option_number)
{
    return ((option_number & 0x1e) == 0x1c);
}

/**
 * @brief Determines whether the given option is intended to be part of the cache key
 *
 * Whether an option is considered a
 * [NoCacheKey](https://datatracker.ietf.org/doc/html/rfc7252#section-5.4.2)
 * is independent from the option's value.
 *
 * @param option_number The option number
 * @returns A boolean value indicating whether options with the given number are a cache key
 */
static inline bool unicoap_option_is_cache_key(unicoap_option_number_t option_number)
{
    return ((option_number & 0x1e) != 0x1c);
}
/** @} */

/* MARK: - Tools */
/**
 * @name Tools
 * @{
 */
/**
 * @brief Returns label of option corresponding to the given number
 * @param number Option number
 * @returns Null-terminated string label, such as `Content-Format`
 */
const char* unicoap_string_from_option_number(unicoap_option_number_t number);

/**
 * @brief Copies storage and adjusts @p options struct.
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options struct whose storage to swap
 * @param[in,out] destination New storage buffer
 * @param capacity @p destination capacity
 *
 * @returns negative number on error
 * @retval `0` on success
 * @retval `-ENOBUFS` @p destination is not sufficiently big
 */
ssize_t unicoap_options_swap_storage(unicoap_options_t* options, uint8_t* destination,
                                     size_t capacity);

/**
 * @brief Retrieves storage buffer
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 * @returns Pointer to first byte of storage buffer, can be `NULL`
 */
static inline uint8_t* unicoap_options_data(const unicoap_options_t* options)
{
    return options->entries->data;
}

/**
 * @brief Retrieves total size of options in buffer
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 * @returns Size of options in buffer in bytes
 */
static inline size_t unicoap_options_size(const unicoap_options_t* options)
{
    return options->storage_size;
}
/** @} */

/* MARK: - Auxiliary constants */
/**
 * @name Auxiliary constants
 * @{
 */
/** @brief Bad option */
#if !defined(EBADOPT) || defined(DOXYGEN)
#  define EBADOPT 151
#endif

/** @brief Cannot read option, encountered payload marker */
#if !defined(EPAYLD) || defined(DOXYGEN)
#  define EPAYLD 152
#endif

/** @brief Largest number representable with 24 bits (3 bytes) */
#define UNICOAP_UINT24_MAX (0xffffff)

/** @brief Size in bytes of number representable with 24 bits (3 bytes) */
#define UNICOAP_UINT24_SIZE (3)

/**
 * @brief Unsigned integer large enough to accommodate the maximum integer
 * representable by CoAP option delta fields and extended length fields.
 *
 * @see [CoAP RFC 7252, 3.1](https://datatracker.ietf.org/doc/html/rfc7252#section-3.1)
 */
#define UNICOAP_UINT_MAX   (14 + 255 + 0xffff)
/** @} */

/* MARK: - Generic Option Methods - */
/**
 * @defgroup net_unicoap_options_generic Generic Option Methods
 * @ingroup net_unicoap_options
 * @brief Retrieve and manipulate generic repeatable and non-repeatable options in CoAP messages
 * @{
 */
/* MARK: - Repeatable options */
/**
 * @name Repeatable options
 * @{
 * Options that can occur more than once in a CoAP message
 */
/**
 * @brief Adds a repeatable option with the given value.
 * @memberof unicoap_options_t
 *
 * Use this function to insert multiple options with the same number but potentially different
 * values.
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in] value Option value to set
 * @param value_size Number of bytes the @p value is made up of
 *
 * @returns Negative integer on error
 * @retval Zero on success
 * @retval `-ENOBUFS` if options buffer lacks sufficient capacity to add option
 */
int unicoap_options_add(unicoap_options_t* options, unicoap_option_number_t number,
                        const uint8_t* value, size_t value_size);

/**
 * @brief Splits the given value into separate values and adds them as option values
 * @memberof unicoap_options_t
 *
 * Use this function to insert multiple options with the same number but potentially different
 * values.
 * The @p buffer will be sliced everywhere the @p separator occurs. This is the same as calling
 * @ref unicoap_options_add for each chunk inbeetween separator bytes.
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in] buffer Option values separated by @p separator
 * @param size Number of bytes the value is made up of
 * @param separator Separator byte used to indicate boundaries between option values
 *
 * @returns Negative integer on error or zero on success
 * @retval `-ENOBUFS` if options buffer lacks sufficient capacity to add options
 *
 * @note A leading @p separator in @p buffer will be ignored by this function.
 */
int unicoap_options_add_values_joined(unicoap_options_t* options, unicoap_option_number_t number,
                                      const uint8_t* buffer, size_t size, uint8_t separator);

/**
 * @brief Copies the values of all options with the given number joined by the given separator
 * @memberof unicoap_options_t
 *
 * Use this API with repeatable options
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] buffer Buffer to copy string value into, must be large enough to carry characters.
 * @param capacity Number of usable bytes in @p dest
 * @param separator Separator to insert between option values.
 *
 * @returns Size of generated byte sequence, including separators or negative integer on error
 * @retval `-ENOBUFS` if @p buffer lacks sufficient capacity to store string
 *
 * @pre @p capacity is greater than zero.
 *
 * @warning This function does _not_ create a null-terminated C string.
 */
ssize_t unicoap_options_copy_values_joined(const unicoap_options_t* options,
                                           unicoap_option_number_t number, uint8_t* buffer,
                                           size_t capacity, uint8_t separator);

/**
 * @brief Removes all options with the given number, if any
 * @memberof unicoap_options_t
 *
 * Use this API with repeatable options
 *
 * @param[in,out] options Options
 * @param number Option number
 *
 * @note If no option with the specified number exists, this function will return zero.
 *
 * @returns Negative integer on error or zero on success
 */
int unicoap_options_remove_all(unicoap_options_t* options, unicoap_option_number_t number);
/** @} */

/* MARK: - Non-repeatable options */
/**
 * @name Non-repeatable options
 * @{
 * Options that occur only once
 */
/**
 * @brief Retrieves the value of the option with given value, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] value Option value. Provide a pointer to a an `uint8_t` pointer, which may be
 *                      `NULL`.
 *
 * @return Size of option value (zero or more bytes) or negative errno if the get operation failed
 * @retval `-ENOENT` Options not found
 * @retval `-EBADOPT` Options buffer is corrupted
 */
ssize_t unicoap_options_get(const unicoap_options_t* options, unicoap_option_number_t number,
                            const uint8_t** value);

/**
 * @brief Copies the value of the option with given value, if present, into a buffer
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] dest Destination buffer where option value will be copied into
 * @param capacity Number of usable bytes in the @p dest buffer
 *
 * @return Size of option value (zero or more bytes) or negative errno if the get operation failed
 * @retval `-ENOENT` Options not found
 * @retval `-EBADOPT` Options buffer is corrupted
 * @retval `-ENOBUFS` if @p dest lacks sufficient capacity to store value
 */
ssize_t unicoap_options_copy_value(const unicoap_options_t* options, unicoap_option_number_t number,
                                   uint8_t* dest, size_t capacity);

/**
 * @brief Sets the option with the given number
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options
 * @param number Option number
 * @param[in] value Option value
 * @param value_size Number of bytes the @p value is made up of
 *
 * @returns Zero on success or egative integer on error instead
 * @retval `-ENOBUFS` if options storage buffer lacks sufficient capacity to set option
 */
int unicoap_options_set(unicoap_options_t* options, unicoap_option_number_t number,
                        const uint8_t* value, size_t value_size);

/**
 * @brief Removes option with the given number, if present
 * @memberof unicoap_options_t
 *
 * @param options Options
 * @param number Option number
 *
 * @note If no option with the specified number exists, this function will return zero.
 *
 * @returns Zero on success or negative integer on error instead
 */
static inline int unicoap_options_remove(unicoap_options_t* options, unicoap_option_number_t number)
{
    /* reuse unicoap_options_remove_all, helps decrease binary size */
    return unicoap_options_remove_all(options, number);
}
/** @} */

/* MARK: - Enumerating options */
/**
 * @name Enumerating options
 * @{
 */
/**
 * @brief The iterator you use to retrieve option values in-order.
 *
 * ## Example
 * ```c
 * unicoap_option_iterator iterator = {0};
 * unicoap_options_iterator_init(&iterator, message->options);
 *
 * uint8_t* value = NULL;
 * ssize_t size = -1;
 * unicoap_option_number_t number = 0;
 *
 * while ((size = unicoap_options_get_next(&iterator, &number, &value)) >= 0)  {
 *     printf("%s (%i) = (%" PRIiSIZE " bytes): ", unicoap_string_from_option_number(number), number, size);
 *     my_dump_hex(value, size);
 * }
 * ```
 */
typedef struct {
    /**
     * @brief Options
     */
    unicoap_options_t* options;

    /**
     * @brief Current option's index
     */
    size_t index;
} unicoap_options_iterator_t;

/**
 * @brief Initializes the given iterator structure
 * @memberof unicoap_options_iterator_t
 *
 * @param[in,out] iterator Option iterator struct to initialize
 * @param[in] options Options to iterate over
 */
static inline void unicoap_options_iterator_init(unicoap_options_iterator_t* iterator,
                                                 unicoap_options_t* options)
{
    assert(iterator);
    assert(options);
    assert(options->entries->data);
    iterator->options = options;
    iterator->index = 0;
}

/**
 * @brief Gets the next option provided by the given iterator
 * @memberof unicoap_options_iterator_t
 *
 * This is a zero-copy API.
 *
 * @param[in,out] iterator Option iterator
 * @param[out] number The number of the next option
 * @param[out] value A pointer to next option's value
 *
 * @returns Positive size of option value on success or negative integer on error instead
 * @retval `-1` if the iterator is finished
 * @retval `-EBADOPT` Options buffer is corrupted
 */
ssize_t unicoap_options_get_next(unicoap_options_iterator_t* iterator,
                                 unicoap_option_number_t* number, const uint8_t** value);

/**
 * @brief Gets the next option with the given number, potentially skipping any options in between.
 * @memberof unicoap_options_iterator_t
 *
 * This is a zero-copy API. Use this function to iterate over specific options that may occur more
 * than once.
 *
 * @param[in,out] iterator Option iterator
 * @param number The option number to look out for
 * @param[out] value A pointer to next option's value
 *
 * @returns Positive size of option value or negative integer on error
 * @retval `-1` if the iterator is finished
 * @retval `-EBADOPT` Options buffer is corrupted
 */
ssize_t unicoap_options_get_next_by_number(unicoap_options_iterator_t* iterator,
                                           unicoap_option_number_t number, const uint8_t** value);

/**
 * @brief Gets the next query option matching the given name, potentially skipping any options
 *        in between.
 *
 * @memberof unicoap_options_iterator_t
 *
 * Use this function to iterate over specific options that may occur more than once.
 *
 * This method splits query values at the `=` character. It is used both for `Uri-Query`
 * and `Location-Query`.
 *
 * @param[in,out] iterator Option iterator
 * @param[out] number Option number
 * @param[in] name Name of UTF-8 query parameter to find
 * @param[out] value Pointer to a UTF-8 string variable
 *
 * @returns Positive size of option value or Negative integer on error
 * @retval `-EBADOPT` Option is corrupted
 * @retval `-1` if the iterator is finished
 *
 * @warning @p value will not be null-terminated.
 * @note This function does not perform Unicode normalization when comparing strings.
 * Instead, `memcmp` is used. This is compliant with
 * [RFC 7252, Section 3.2](https://datatracker.ietf.org/doc/html/rfc7252#section-3.2)
 */
ssize_t unicoap_options_get_next_query_by_name(unicoap_options_iterator_t* iterator,
                                               unicoap_option_number_t number, const char* name,
                                               const char** value);

/**
 * @brief Iterates and dumps all options using `printf`
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 */
void unicoap_options_dump_all(const unicoap_options_t* options);
/** @} */

/* MARK: - Strings */
/**
 * @name Strings
 * @{
 */
/**
 * @brief Sets a non-repeatable option to the given string value.
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in] string String option value to set
 * @param count Number of characters the string is made up of, excluding null-terminator, or zero,
 *              which triggers a `strlen` call
 *
 * Specify the number of characters or zero to let `unicoap` determine the string length.
 *
 * @returns Zero on success or negative integer on error otherwise
 * @retval `-ENOBUFS` if options buffer lacks sufficient capacity to set option
 */
static inline int unicoap_options_set_string(unicoap_options_t* options,
                                             unicoap_option_number_t number, const char* string,
                                             size_t count)
{
    return unicoap_options_set(options, number, (uint8_t*)string,
                               ((count > 0) && string) ? count : strlen(string));
}

/**
 * @brief Adds a repeatable option with the given string value.
 * @memberof unicoap_options_t
 *
 * Use this function to insert multiple options with the same number but potentially different
 * values.
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in] string String option value to set
 * @param count Number of characters the string is made up of, excluding null-terminator, or zero,
 *              which triggers a `strlen` call
 *
 * Specify the number of characters or zero to let `unicoap` determine the string length.
 *
 * @returns Zero on success or negative integer on error
 * @retval `-ENOBUFS` if options buffer lacks sufficient capacity to add option
 */
static inline int unicoap_options_add_string(unicoap_options_t* options,
                                             unicoap_option_number_t number, const char* string,
                                             size_t count)
{
    return unicoap_options_add(options, number, (uint8_t*)string,
                               ((count > 0) && string) ? count : strlen(string));
}
/** @} */

/* MARK: - Unsigned integers */
/**
 * @name Unsigned integers
 * @{
 */
#ifndef DOXYGEN
/**
 * @brief Retrieves an unsigned option value with a configurable maximum width
 * @memberof unicoap_options_t
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] uint Pointer to 32-bit integer that will store unsigned integer in host byte order
 * @param max_size Maximum number of bytes used to represent the integer (1 to 4)
 *
 * @returns Number of bytes occupied by the unsigned integer in the option value or negative
 *          integer on error
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option is corrupted or integer wider than expected
 *
 * @pre @p max_size does not exceed `sizeof(uint32_t)`
 */
ssize_t _unicoap_options_get_variable_uint(const unicoap_options_t* options,
                                           unicoap_option_number_t number, uint32_t* uint,
                                           size_t max_size);
#endif

/**
 * @brief Retrieves an unsigned option value that takes up at most 4 bytes.
 * @memberof unicoap_options_t
 *
 * Unsigned option values in CoAP are variable in length, i.e., uints are not zero-padded.
 * A zero may be represented by a zero-length option value.
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] uint Provide a pointer to an allocated 32-bit unsigned integer, will have been
 *                     filled after function has returned
 *
 * @returns Number of bytes occupied by the unsigned integer in the option value or negative
 *          integer on error
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option is corrupted
 */
static inline ssize_t unicoap_options_get_uint32(const unicoap_options_t* options,
                                                 unicoap_option_number_t number, uint32_t* uint)
{
    return _unicoap_options_get_variable_uint(options, number, uint, sizeof(uint32_t));
}

/**
 * @brief Retrieves an unsigned option value that takes up at most 3 bytes.
 * @memberof unicoap_options_t
 *
 * Unsigned option values in CoAP are variable in length, i.e., uints are not zero-padded.
 * A zero may be represented by a zero-length option value.
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] uint Provide a pointer to an allocated 32-bit unsigned integer, will have been
 *                     filled after function has returned
 *
 * @returns Number of bytes occupied by the unsigned integer in the option value or negative
 *          integer on error
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option is corrupted
 */
static inline ssize_t unicoap_options_get_uint24(const unicoap_options_t* options,
                                                 unicoap_option_number_t number, uint32_t* uint)
{
    return _unicoap_options_get_variable_uint(options, number, uint, UNICOAP_UINT24_SIZE);
}

/**
 * @brief Retrieves an unsigned option value that takes up at most 2 bytes.
 * @memberof unicoap_options_t
 *
 * Unsigned option values in CoAP are variable in length, i.e., uints are not zero-padded.
 * A zero may be represented by a zero-length option value.
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] uint Provide a pointer to an allocated 16-bit unsigned integer, will have been
 *                     filled after function has returned
 *
 * @returns Number of bytes occupied by the unsigned integer in the option value or negative
 *          integer on error
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option is corrupted
 */
static inline ssize_t unicoap_options_get_uint16(const unicoap_options_t* options,
                                                 unicoap_option_number_t number, uint16_t* uint)
{
    uint32_t _uint = 0;
    int res = _unicoap_options_get_variable_uint(options, number, &_uint, sizeof(uint16_t));
    *uint = (uint16_t)_uint;
    return res;
}

/**
 * @brief Retrieves an unsigned option value that takes up at most one bytes.
 * @memberof unicoap_options_t
 *
 * Unsigned option values in CoAP are variable in length, i.e., uints are not zero-padded.
 * A zero may be represented by a zero-length option value.
 *
 * @param[in] options Options
 * @param number Option number
 * @param[in,out] uint Provide a pointer to an allocated 8-bit unsigned integer, will have been
 *                     filled after function has returned
 *
 * @returns Number of bytes occupied by the unsigned integer in the option value or negative
 *          integer on error
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option is corrupted
 * @retval `-ENOBUFS` Option value was bigger than 1 byte
 */
static inline ssize_t unicoap_options_get_uint8(const unicoap_options_t* options,
                                                unicoap_option_number_t number, uint8_t* uint)
{
    return unicoap_options_copy_value(options, number, uint, sizeof(uint8_t));
}

/**
 * @brief Sets the option with the given number to the unsigned integer value passed.
 * @memberof unicoap_options_t
 *
 * Unsigned option values in CoAP are variable in length, i.e., uints are not zero-padded.
 * A zero may be represented by a zero-length option value.
 *
 * @param[in] options Options
 * @param number Option number
 * @param value Unsigned integer option value
 *
 * @returns Zero on success or negative integer on error
 * @retval `-ENOBUFS` if options buffer lacks sufficient capacity to set option
 */
int unicoap_options_set_uint(unicoap_options_t* options, unicoap_option_number_t number,
                             uint32_t value);

/**
 * @brief Adds a repeatable option with the given unsigned integer value.
 * @memberof unicoap_options_t
 *
 * Use this function to insert multiple options with the same number but potentially different
 * values.
 *
 * Unsigned option values in CoAP are variable in length, i.e., uints are not zero-padded.
 * A zero may be represented by a zero-length option value.
 *
 * @param[in] options Options
 * @param number Option number
 * @param value Unsigned integer option value
 *
 * @returns Zero on success or negative integer on error
 * @retval `-ENOBUFS` if options buffer lacks sufficient capacity to add option
 */
int unicoap_options_add_uint(unicoap_options_t* options, unicoap_option_number_t number,
                             uint32_t value);
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
