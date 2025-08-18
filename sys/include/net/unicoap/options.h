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
#  define _UNICOAP_OPTIONS_ALLOC(_buf, _name, capacity, _static)    \
    _static uint8_t _buf[capacity];                                 \
    _static unicoap_options_t _name = {                             \
        .entries = { { .data = _buf } },                            \
        .storage_capacity = capacity,                               \
    };
#endif

/**
 * @brief Allocates options with buffer capacity
 *
 * @param name Name of the variable storing the options structure
 * @param capacity Storage buffer capacity in bytes
 *
 * Allocates a new @ref unicoap_options_t container and a storage buffer with
 * the given capacity, and initializes it. No need to call
 * @ref unicoap_options_t::unicoap_options_init afterwards.
 *
 * See @ref UNICOAP_OPTIONS_ALLOC_STATIC for static allocation
 */
#define UNICOAP_OPTIONS_ALLOC(name, capacity) \
    _UNICOAP_OPTIONS_ALLOC(_CONCAT3(name, _storage, __LINE__), name, capacity,)

/**
 * @brief Statically allocates options with buffer capacity
 *
 * @param name Name of the variable storing the options structure
 * @param capacity Static storage buffer capacity in bytes
 *
 * Statically allocates a new @ref unicoap_options_t container and a storage
 * buffer with the given capacity, and initializes it. No need to call
 * @ref unicoap_options_t::unicoap_options_init afterwards.
 *
 * See @ref UNICOAP_OPTIONS_ALLOC for non-static allocation
 */
#define UNICOAP_OPTIONS_ALLOC_STATIC(name, capacity) \
    _UNICOAP_OPTIONS_ALLOC(_CONCAT3(name, _storage, __LINE__), name, capacity, static)

/**
 * @brief Allocates options with default capacity
 *
 * @param name Name of the variable storing the options structure
 *
 * Allocates a new @ref unicoap_options_t container and a storage buffer with
 * @ref CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY, and initializes it.
 * No need to call @ref unicoap_options_t::unicoap_options_init afterwards.
 *
 * See @ref UNICOAP_OPTIONS_ALLOC_STATIC_DEFAULT for static allocation
 */
#define UNICOAP_OPTIONS_ALLOC_DEFAULT(name) \
    UNICOAP_OPTIONS_ALLOC(name, CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY)

/**
 * @brief Statically allocates options with default capacity
 *
 * @param name Name of the variable storing the options structure
 *
 * Statically allocates a new @ref unicoap_options_t container and a storage buffer
 * with @ref CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY, and initializes it.
 * No need to call @ref unicoap_options_t::unicoap_options_init afterwards.
 *
 * See @ref UNICOAP_OPTIONS_ALLOC_DEFAULT for non-static allocation
 */
#define UNICOAP_OPTIONS_ALLOC_STATIC_DEFAULT(name) \
    UNICOAP_OPTIONS_ALLOC_STATIC(name, CONFIG_UNICOAP_OPTIONS_BUFFER_DEFAULT_CAPACITY)
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

/* MARK: - Predefined Options - */

/**
 * @defgroup net_unicoap_options_predefined Predefined Options
 * @ingroup net_unicoap_options
 * @brief Read, set, and add options predefined by IANA
 * @{
 */

/* MARK: - Uri-Host */
/**
 * @name Uri-Host
 * @{
 */

/**
 * @brief Retrieves the `Uri-Host` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] host Pointer to value variable as buffer of UTF-8 code units
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_uri_host(const unicoap_options_t* options,
                                                   const char** host)
{
    return unicoap_options_get(options, UNICOAP_OPTION_URI_HOST, (const uint8_t**)host);
}

/**
 * @brief Sets the `Uri-Host` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] host `Uri-Host` value as buffer of UTF-8 code units
 * @param length Number of UTF-8 code units in @p host, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 *
 * @pre @p host must be at least of size 1 bytes
 * @pre @p host size must not exceed 255 bytes
 */
static inline ssize_t unicoap_options_set_uri_host(unicoap_options_t* options, char* host,
                                                   size_t length)
{
    assert(length > 0 && length <= 255);
    return unicoap_options_set(options, UNICOAP_OPTION_URI_HOST, (uint8_t*)host, length);
}

/**
 * @brief Sets the `Uri-Host` option from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] host `Uri-Host` value as buffer of UTF-8 code units, must be null-terminated
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 *
 * @pre @p host must be at least of size 1 bytes
 * @pre @p host size must not exceed 255 bytes
 *
 * Uses `strlen` to calculate the number of code units.
 * @see @ref unicoap_options_set_uri_host
 */
static inline ssize_t unicoap_options_set_uri_host_string(unicoap_options_t* options, char* host)
{
    return unicoap_options_set_uri_host(options, host, strlen(host));
}

/**
 * @brief Removes the `Uri-Host` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_uri_host(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_URI_HOST);
}
/** @} */ /* Uri-Host */

/* MARK: - If-None-Match */
/**
 * @name If-None-Match
 * @{
 */

/**
 * @brief Determines whether the `If-None-Match` option is present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline bool unicoap_options_get_if_none_match(const unicoap_options_t* options)
{
    return unicoap_options_contains(options, UNICOAP_OPTION_IF_NONE_MATCH);
}

/**
 * @brief Sets the `If-None-Match` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param value `If-None-Match` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_if_none_match(unicoap_options_t* options, bool value)
{
    if (value) {
        return unicoap_options_remove(options, UNICOAP_OPTION_IF_NONE_MATCH);
    }
    else {
        return unicoap_options_set(options, UNICOAP_OPTION_IF_NONE_MATCH, NULL, 0);
    }
}
/** @} */ /* If-None-Match */

/* MARK: - Uri-Port */
/**
 * @name Uri-Port
 * @{
 */

/**
 * @brief Retrieves the `Uri-Port` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] port Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_uri_port(const unicoap_options_t* options, uint16_t* port)
{
    return unicoap_options_get_uint16(options, UNICOAP_OPTION_URI_PORT, port);
}

/**
 * @brief Sets the `Uri-Port` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param port `Uri-Port` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_uri_port(unicoap_options_t* options, uint16_t port)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_URI_PORT, (uint32_t)port);
}

/**
 * @brief Removes the `Uri-Port` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_uri_port(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_URI_PORT);
}
/** @} */ /* Uri-Port */

/* MARK: - Content-Format */
/**
 * @name Content-Format
 * @{
 */

/**
 * @brief Retrieves the `Content-Format` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] format Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_content_format(const unicoap_options_t* options,
                                                         unicoap_content_format_t* format)
{
    return unicoap_options_get_uint16(options, UNICOAP_OPTION_CONTENT_FORMAT, (uint16_t*)format);
}

/**
 * @brief Sets the `Content-Format` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param format `Content-Format` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_content_format(unicoap_options_t* options,
                                                         unicoap_content_format_t format)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_CONTENT_FORMAT, (uint32_t)format);
}

/**
 * @brief Removes the `Content-Format` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_content_format(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_CONTENT_FORMAT);
}
/** @} */ /* Content-Format */

/* MARK: - Max-Age */
/**
 * @name Max-Age
 * @{
 */

/**
 * @brief Retrieves the `Max-Age` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] age Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_max_age(const unicoap_options_t* options, uint32_t* age)
{
    return unicoap_options_get_uint32(options, UNICOAP_OPTION_MAX_AGE, age);
}

/**
 * @brief Sets the `Max-Age` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param age `Max-Age` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_max_age(unicoap_options_t* options, uint32_t age)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_MAX_AGE, age);
}

/**
 * @brief Removes the `Max-Age` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_max_age(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_MAX_AGE);
}
/** @} */ /* Max-Age */

/* MARK: - Accept */
/**
 * @name Accept
 * @{
 */

/**
 * @brief Retrieves the `Accept` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] format Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_accept(const unicoap_options_t* options,
                                                 unicoap_content_format_t* format)
{
    return unicoap_options_get_uint16(options, UNICOAP_OPTION_ACCEPT, (uint16_t*)format);
}

/**
 * @brief Sets the `Accept` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param format `Accept` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_accept(unicoap_options_t* options,
                                                 unicoap_content_format_t format)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_ACCEPT, (uint32_t)format);
}

/**
 * @brief Removes the `Accept` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_accept(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_ACCEPT);
}
/** @} */ /* Accept */

/* MARK: - Proxy-Scheme */
/**
 * @name Proxy-Scheme
 * @{
 */

/**
 * @brief Retrieves the `Proxy-Scheme` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] scheme Pointer to value variable as buffer of UTF-8 code units
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_proxy_scheme(const unicoap_options_t* options,
                                                       const char** scheme)
{
    return unicoap_options_get(options, UNICOAP_OPTION_PROXY_SCHEME, (const uint8_t**)scheme);
}

/**
 * @brief Sets the `Proxy-Scheme` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] scheme `Proxy-Scheme` value as buffer of UTF-8 code units
 * @param length Number of UTF-8 code units in @p scheme, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 *
 */
static inline ssize_t unicoap_options_set_proxy_scheme(unicoap_options_t* options, char* scheme,
                                                       size_t length)
{
    return unicoap_options_set(options, UNICOAP_OPTION_PROXY_SCHEME, (uint8_t*)scheme, length);
}

/**
 * @brief Sets the `Proxy-Scheme` option from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] scheme `Proxy-Scheme` value as buffer of UTF-8 code units, must be null-terminated
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 *
 *
 * Uses `strlen` to calculate the number of code units.
 * @see @ref unicoap_options_set_proxy_scheme
 */
static inline ssize_t unicoap_options_set_proxy_scheme_string(unicoap_options_t* options,
                                                              char* scheme)
{
    return unicoap_options_set_proxy_scheme(options, scheme, strlen(scheme));
}

/**
 * @brief Removes the `Proxy-Scheme` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_proxy_scheme(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_PROXY_SCHEME);
}
/** @} */ /* Proxy-Scheme */

/* MARK: - Proxy-Uri */
/**
 * @name Proxy-Uri
 * @{
 */

/**
 * @brief Retrieves the `Proxy-Uri` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] uri Pointer to value variable as buffer of UTF-8 code units
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_proxy_uri(const unicoap_options_t* options,
                                                    const char** uri)
{
    return unicoap_options_get(options, UNICOAP_OPTION_PROXY_URI, (const uint8_t**)uri);
}

/**
 * @brief Sets the `Proxy-Uri` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] uri `Proxy-Uri` value as buffer of UTF-8 code units
 * @param length Number of UTF-8 code units in @p uri, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 *
 * @pre @p uri must be at least of size 1 bytes
 * @pre @p uri size must not exceed 1034 bytes
 */
static inline ssize_t unicoap_options_set_proxy_uri(unicoap_options_t* options, char* uri,
                                                    size_t length)
{
    assert(length > 0 && length <= 1034);
    return unicoap_options_set(options, UNICOAP_OPTION_PROXY_URI, (uint8_t*)uri, length);
}

/**
 * @brief Sets the `Proxy-Uri` option from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] uri `Proxy-Uri` value as buffer of UTF-8 code units, must be null-terminated
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 *
 * @pre @p uri must be at least of size 1 bytes
 * @pre @p uri size must not exceed 1034 bytes
 *
 * Uses `strlen` to calculate the number of code units.
 * @see @ref unicoap_options_set_proxy_uri
 */
static inline ssize_t unicoap_options_set_proxy_uri_string(unicoap_options_t* options, char* uri)
{
    return unicoap_options_set_proxy_uri(options, uri, strlen(uri));
}

/**
 * @brief Removes the `Proxy-Uri` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_proxy_uri(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_PROXY_URI);
}
/** @} */ /* Proxy-Uri */

/* MARK: - No-Response */
/**
 * @name No-Response
 * @{
 */

/**
 * @brief Retrieves the `No-Response` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] value Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_no_response(const unicoap_options_t* options,
                                                      uint8_t* value)
{
    return unicoap_options_get_uint8(options, UNICOAP_OPTION_NO_RESPONSE, value);
}

/**
 * @brief Sets the `No-Response` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param value `No-Response` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_no_response(unicoap_options_t* options, uint8_t value)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_NO_RESPONSE, (uint32_t)value);
}

/**
 * @brief Removes the `No-Response` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_no_response(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_NO_RESPONSE);
}
/** @} */ /* No-Response */

/* MARK: - If-Match */
/**
 * @name If-Match
 * @{
 */

/**
 * @brief Retrieves the first `If-Match` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] value Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_first_if_match(const unicoap_options_t* options,
                                                         const uint8_t** value)
{
    return unicoap_options_get(options, UNICOAP_OPTION_IF_MATCH, value);
}

/**
 * @brief Gets the next `If-Match` option provided by the specified iterator
 * @memberof unicoap_options_t
 *
 * @param[in,out] iterator Option iterator
 * @param[out] value Pointer to a variable that will store next value
 *
 * @see @ref unicoap_options_iterator_t::unicoap_options_get_next_by_number
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t unicoap_options_get_next_if_match(unicoap_options_iterator_t* iterator,
                                                        const uint8_t** value)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_IF_MATCH, value);
}

/**
 * @brief Adds `If-Match` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] value `If-Match` value
 * @param size Number of bytes in @p value in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p value size must not exceed 8 bytes
 */
static inline ssize_t unicoap_options_add_if_match(unicoap_options_t* options, uint8_t* value,
                                                   size_t size)
{
    assert(size <= 8);
    return unicoap_options_add(options, UNICOAP_OPTION_IF_MATCH, value, size);
}

/**
 * @brief Removes all `If-Match` options, if any
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_all_if_match(unicoap_options_t* options)
{
    return unicoap_options_remove_all(options, UNICOAP_OPTION_IF_MATCH);
}
/** @} */ /* If-Match */

/* MARK: - ETag */
/**
 * @name ETag
 * @{
 */

/**
 * @brief Retrieves the first `ETag` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] etag Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_first_etag(const unicoap_options_t* options,
                                                     const uint8_t** etag)
{
    return unicoap_options_get(options, UNICOAP_OPTION_ETAG, etag);
}

/**
 * @brief Gets the next `ETag` option provided by the specified iterator
 * @memberof unicoap_options_t
 *
 * @param[in,out] iterator Option iterator
 * @param[out] etag Pointer to a variable that will store next etag
 *
 * @see @ref unicoap_options_iterator_t::unicoap_options_get_next_by_number
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t unicoap_options_get_next_etag(unicoap_options_iterator_t* iterator,
                                                    const uint8_t** etag)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_ETAG, etag);
}

/**
 * @brief Adds `ETag` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] etag `ETag` value
 * @param size Number of bytes in @p etag in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p etag must be at least of size 1 bytes
 * @pre @p etag size must not exceed 8 bytes
 */
static inline ssize_t unicoap_options_add_etag(unicoap_options_t* options, uint8_t* etag,
                                               size_t size)
{
    assert(size > 0 && size <= 8);
    return unicoap_options_add(options, UNICOAP_OPTION_ETAG, etag, size);
}

/**
 * @brief Removes all `ETag` options, if any
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_etags(unicoap_options_t* options)
{
    return unicoap_options_remove_all(options, UNICOAP_OPTION_ETAG);
}
/** @} */ /* ETag */

/* MARK: - Location-Path */
/**
 * @name Location-Path
 * @{
 */

/**
 * @brief Retrieves the first `Location-Path` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] component Pointer to value variable as buffer of UTF-8 code units
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t
unicoap_options_get_first_location_path_component(const unicoap_options_t* options,
                                                  const char** component)
{
    return unicoap_options_get(options, UNICOAP_OPTION_LOCATION_PATH, (const uint8_t**)component);
}

/**
 * @brief Gets the next `Location-Path` option provided by the specified iterator
 * @memberof unicoap_options_t
 *
 * @param[in,out] iterator Option iterator
 * @param[out] component Pointer to a variable that will store next component
 *
 * @see @ref unicoap_options_iterator_t::unicoap_options_get_next_by_number
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t
unicoap_options_get_next_location_path_component(unicoap_options_iterator_t* iterator,
                                                 const char** component)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_LOCATION_PATH,
                                              (const uint8_t**)component);
}

/**
 * @brief Copies absolute location path into the given buffer
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[in,out] path Buffer of UTF-8 code units
 * @param capacity Capacity of @p path
 *
 * @returns Length of string value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-ENOBUFS` @p path lacks sufficient capacity to copy values
 *
 * @pre @p capacity must be greater than zero
 *
 * This function creates a string from all `Location-Path` options
 * by joining them with the `/` separator.
 * The string will bear the `/` prefix even if there is no `Location-Path` option.
 * The string will not be null-terminated. The resulting string uses UTF-8 encoding.
 */
static inline ssize_t unicoap_options_copy_location_path(const unicoap_options_t* options,
                                                         char* path, size_t capacity)
{
    assert(capacity > 0);
    *path = '/';
    path += 1;
    capacity -= 1;
    int res = unicoap_options_copy_values_joined(options, UNICOAP_OPTION_LOCATION_PATH,
                                                 (uint8_t*)path, capacity, '/');
    return res < 0 ? res : res + 1;
}

/**
 * @brief Adds `Location-Path` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] component `Location-Path` value as buffer of UTF-8 code units
 * @param length Number of UTF-8 code units in @p component, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 */
static inline ssize_t unicoap_options_add_location_path_component(unicoap_options_t* options,
                                                                  char* component, size_t length)
{
    return unicoap_options_add(options, UNICOAP_OPTION_LOCATION_PATH, (uint8_t*)component, length);
}

/**
 * @brief Adds `Location-Path` option from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] component `Location-Path` value as buffer of UTF-8 code units, must be null-terminated
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 *
 * Uses `strlen` to calculate the number of code units.
 * @see @ref unicoap_options_add_location_path_component
 */
static inline ssize_t unicoap_options_add_location_path_component_string(unicoap_options_t* options,
                                                                         char* component)
{
    return unicoap_options_add_location_path_component(options, component, strlen(component));
}

/**
 * @brief Adds multiple `Location-Path` options from string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] path as buffer of UTF-8 code units with values separated by `/`
 * @param length Number of UTF-8 code units in @p path, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 */
static inline ssize_t unicoap_options_add_location_path(unicoap_options_t* options, char* path,
                                                        size_t length)
{
    return unicoap_options_add_values_joined(options, UNICOAP_OPTION_LOCATION_PATH, (uint8_t*)path,
                                             length, '/');
}

/**
 * @brief Adds multiple `Location-Path` options from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] path as null-terminated string of UTF-8 code units with values separated by `/`
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 * Uses `strlen` to calculate the number of code units.
 *
 * @see @ref unicoap_options_add_location_path
 */
static inline ssize_t unicoap_options_add_location_path_string(unicoap_options_t* options,
                                                               char* path)
{
    return unicoap_options_add_location_path(options, path, strlen(path));
}

/**
 * @brief Removes all `Location-Path` options, if any
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_location_path(unicoap_options_t* options)
{
    return unicoap_options_remove_all(options, UNICOAP_OPTION_LOCATION_PATH);
}
/** @} */ /* Location-Path */

/* MARK: - Uri-Path */
/**
 * @name Uri-Path
 * @{
 */

/**
 * @brief Retrieves the first `Uri-Path` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] component Pointer to value variable as buffer of UTF-8 code units
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_first_uri_path_component(const unicoap_options_t* options,
                                                                   const char** component)
{
    return unicoap_options_get(options, UNICOAP_OPTION_URI_PATH, (const uint8_t**)component);
}

/**
 * @brief Gets the next `Uri-Path` option provided by the specified iterator
 * @memberof unicoap_options_t
 *
 * @param[in,out] iterator Option iterator
 * @param[out] component Pointer to a variable that will store next component
 *
 * @see @ref unicoap_options_iterator_t::unicoap_options_get_next_by_number
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t
unicoap_options_get_next_uri_path_component(unicoap_options_iterator_t* iterator,
                                            const char** component)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_URI_PATH,
                                              (const uint8_t**)component);
}

/**
 * @brief Copies absolute URI path into the given buffer
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[in,out] path Buffer of UTF-8 code units
 * @param capacity Capacity of @p path
 *
 * @returns Length of string value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-ENOBUFS` @p path lacks sufficient capacity to copy values
 *
 * @pre @p capacity must be greater than zero
 *
 * This function creates a string from all `Uri-Path` options
 * by joining them with the `/` separator.
 * The string will bear the `/` prefix even if there is no `Uri-Path` option.
 * The string will not be null-terminated. The resulting string uses UTF-8 encoding.
 */
static inline ssize_t unicoap_options_copy_uri_path(const unicoap_options_t* options, char* path,
                                                    size_t capacity)
{
    assert(capacity > 0);
    *path = '/';
    path += 1;
    capacity -= 1;
    int res = unicoap_options_copy_values_joined(options, UNICOAP_OPTION_URI_PATH, (uint8_t*)path,
                                                 capacity, '/');
    return res < 0 ? res : res + 1;
}

/**
 * @brief Adds `Uri-Path` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] component `Uri-Path` value as buffer of UTF-8 code units
 * @param length Number of UTF-8 code units in @p component, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p component size must not exceed 255 bytes
 */
static inline ssize_t unicoap_options_add_uri_path_component(unicoap_options_t* options,
                                                             char* component, size_t length)
{
    assert(length <= 255);
    return unicoap_options_add(options, UNICOAP_OPTION_URI_PATH, (uint8_t*)component, length);
}

/**
 * @brief Adds `Uri-Path` option from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] component `Uri-Path` value as buffer of UTF-8 code units, must be null-terminated
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p component size must not exceed 255 bytes
 *
 * Uses `strlen` to calculate the number of code units.
 * @see @ref unicoap_options_add_uri_path_component
 */
static inline ssize_t unicoap_options_add_uri_path_component_string(unicoap_options_t* options,
                                                                    char* component)
{
    return unicoap_options_add_uri_path_component(options, component, strlen(component));
}

/**
 * @brief Adds multiple `Uri-Path` options from string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] path as buffer of UTF-8 code units with values separated by `/`
 * @param length Number of UTF-8 code units in @p path, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 * @pre components' size must not exceed 255 bytes
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 */
static inline ssize_t unicoap_options_add_uri_path(unicoap_options_t* options, char* path,
                                                   size_t length)
{
    return unicoap_options_add_values_joined(options, UNICOAP_OPTION_URI_PATH, (uint8_t*)path,
                                             length, '/');
}

/**
 * @brief Adds multiple `Uri-Path` options from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] path as null-terminated string of UTF-8 code units with values separated by `/`
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 * @pre components size must not exceed 255 bytes
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 * Uses `strlen` to calculate the number of code units.
 *
 * @see @ref unicoap_options_add_uri_path
 */
static inline ssize_t unicoap_options_add_uri_path_string(unicoap_options_t* options, char* path)
{
    return unicoap_options_add_uri_path(options, path, strlen(path));
}

/**
 * @brief Removes all `Uri-Path` options, if any
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_uri_path(unicoap_options_t* options)
{
    return unicoap_options_remove_all(options, UNICOAP_OPTION_URI_PATH);
}
/** @} */ /* Uri-Path */

/* MARK: - Uri-Query */
/**
 * @name Uri-Query
 * @{
 */

/**
 * @brief Retrieves the first `Uri-Query` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] query Pointer to value variable as buffer of UTF-8 code units
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_first_uri_query(const unicoap_options_t* options,
                                                          const char** query)
{
    return unicoap_options_get(options, UNICOAP_OPTION_URI_QUERY, (const uint8_t**)query);
}

/**
 * @brief Gets the next `Uri-Query` option provided by the specified iterator
 * @memberof unicoap_options_t
 *
 * @param[in,out] iterator Option iterator
 * @param[out] query Pointer to a variable that will store next query
 *
 * @see @ref unicoap_options_iterator_t::unicoap_options_get_next_by_number
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t unicoap_options_get_next_uri_query(unicoap_options_iterator_t* iterator,
                                                         const char** query)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_URI_QUERY,
                                              (const uint8_t**)query);
}

/**
 * @brief Gets the next `Uri-Query` option matching the given name,
 * potentially skipping any options in between.
 *
 * @memberof unicoap_options_t
 *
 * This method splits `Uri-Query` options at the `=` character.
 *
 * @param[in,out] iterator Option iterator
 * @param[in] name Name of query parameter to find
 * @param[out] value Pointer to a UTF-8 string variable that will store the entire option,
 * including the `name=` part
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t
unicoap_options_get_next_uri_query_by_name(unicoap_options_iterator_t* iterator, const char* name,
                                           const char** value)
{
    return unicoap_options_get_next_query_by_name(iterator, UNICOAP_OPTION_URI_QUERY, name, value);
}

/**
 * @brief Retrieves the first `Uri-Query` option matching the given name, if present
 * @memberof unicoap_options_t
 *
 * This method splits `Uri-Query` options at the `=` character.
 *
 * @param[in] options Options to read from
 * @param[in] name Name of query parameter to find
 * @param[out] value Pointer to a UTF-8 string variable that will store the entire option,
 * including the `name=` part
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished and no matching `Uri-Query` option was found
 *
 * @note If you already have an @ref unicoap_options_iterator_t instance allocated somewhere, use
 * it and call @ref unicoap_options_get_next_uri_query_by_name instead.
 */
static inline ssize_t unicoap_options_get_first_uri_query_by_name(unicoap_options_t* options,
                                                                  const char* name,
                                                                  const char** value)
{
    unicoap_options_iterator_t iterator;
    unicoap_options_iterator_init(&iterator, options);
    return unicoap_options_get_next_uri_query_by_name(&iterator, name, value);
}

/**
 * @brief Copies URI query string into the given buffer
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[in,out] queries Buffer of UTF-8 code units
 * @param capacity Capacity of @p queries
 *
 * @returns Length of string value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-ENOBUFS` @p queries lacks sufficient capacity to copy values
 *
 * This function creates a string from all `Uri-Query` options
 * by joining them with the `&` separator.
 * The string will not be null-terminated. The resulting string uses UTF-8 encoding.
 */
static inline ssize_t unicoap_options_copy_uri_queries(const unicoap_options_t* options,
                                                       char* queries, size_t capacity)
{
    return unicoap_options_copy_values_joined(options, UNICOAP_OPTION_URI_QUERY, (uint8_t*)queries,
                                              capacity, '&');
}

/**
 * @brief Adds `Uri-Query` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] query `Uri-Query` value as buffer of UTF-8 code units
 * @param length Number of UTF-8 code units in @p query, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p query size must not exceed 255 bytes
 */
static inline ssize_t unicoap_options_add_uri_query(unicoap_options_t* options, char* query,
                                                    size_t length)
{
    assert(length <= 255);
    return unicoap_options_add(options, UNICOAP_OPTION_URI_QUERY, (uint8_t*)query, length);
}

/**
 * @brief Adds `Uri-Query` option from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] query `Uri-Query` value as buffer of UTF-8 code units, must be null-terminated
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p query size must not exceed 255 bytes
 *
 * Uses `strlen` to calculate the number of code units.
 * @see @ref unicoap_options_add_uri_query
 */
static inline ssize_t unicoap_options_add_uri_query_string(unicoap_options_t* options, char* query)
{
    return unicoap_options_add_uri_query(options, query, strlen(query));
}

/**
 * @brief Adds multiple `Uri-Query` options from string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] queries as buffer of UTF-8 code units with values separated by `&`
 * @param length Number of UTF-8 code units in @p queries, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 * @pre queries' size must not exceed 255 bytes
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 */
static inline ssize_t unicoap_options_add_uri_queries(unicoap_options_t* options, char* queries,
                                                      size_t length)
{
    return unicoap_options_add_values_joined(options, UNICOAP_OPTION_URI_QUERY, (uint8_t*)queries,
                                             length, '&');
}

/**
 * @brief Adds multiple `Uri-Query` options from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] queries as null-terminated string of UTF-8 code units with values separated by `&`
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 * @pre queries size must not exceed 255 bytes
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 * Uses `strlen` to calculate the number of code units.
 *
 * @see @ref unicoap_options_add_uri_queries
 */
static inline ssize_t unicoap_options_add_uri_queries_string(unicoap_options_t* options,
                                                             char* queries)
{
    return unicoap_options_add_uri_queries(options, queries, strlen(queries));
}

/**
 * @brief Removes all `Uri-Query` options, if any
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_uri_queries(unicoap_options_t* options)
{
    return unicoap_options_remove_all(options, UNICOAP_OPTION_URI_QUERY);
}
/** @} */ /* Uri-Query */

/* MARK: - Location-Query */
/**
 * @name Location-Query
 * @{
 */

/**
 * @brief Retrieves the first `Location-Query` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] query Pointer to value variable as buffer of UTF-8 code units
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_first_location_query(const unicoap_options_t* options,
                                                               const char** query)
{
    return unicoap_options_get(options, UNICOAP_OPTION_LOCATION_QUERY, (const uint8_t**)query);
}

/**
 * @brief Gets the next `Location-Query` option provided by the specified iterator
 * @memberof unicoap_options_t
 *
 * @param[in,out] iterator Option iterator
 * @param[out] query Pointer to a variable that will store next query
 *
 * @see @ref unicoap_options_iterator_t::unicoap_options_get_next_by_number
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t unicoap_options_get_next_location_query(unicoap_options_iterator_t* iterator,
                                                              const char** query)
{
    return unicoap_options_get_next_by_number(iterator, UNICOAP_OPTION_LOCATION_QUERY,
                                              (const uint8_t**)query);
}

/**
 * @brief Gets the next `Location-Query` option matching the given name,
 * potentially skipping any options in between.
 *
 * @memberof unicoap_options_t
 *
 * This method splits `Location-Query` options at the `=` character.
 *
 * @param[in,out] iterator Option iterator
 * @param[in] name Name of query parameter to find
 * @param[out] value Pointer to a UTF-8 string variable that will store the entire option,
 * including the `name=` part
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished
 */
static inline ssize_t
unicoap_options_get_next_location_query_by_name(unicoap_options_iterator_t* iterator,
                                                const char* name, const char** value)
{
    return unicoap_options_get_next_query_by_name(iterator, UNICOAP_OPTION_LOCATION_QUERY, name,
                                                  value);
}

/**
 * @brief Retrieves the first `Location-Query` option matching the given name, if present
 * @memberof unicoap_options_t
 *
 * This method splits `Location-Query` options at the `=` character.
 *
 * @param[in] options Options to read from
 * @param[in] name Name of query parameter to find
 * @param[out] value Pointer to a UTF-8 string variable that will store the entire option,
 * including the `name=` part
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-1` if the iterator is finished and no matching `Location-Query` option was found
 *
 * @note If you already have an @ref unicoap_options_iterator_t instance allocated somewhere, use
 * it and call @ref unicoap_options_get_next_location_query_by_name instead.
 */
static inline ssize_t unicoap_options_get_first_location_query_by_name(unicoap_options_t* options,
                                                                       const char* name,
                                                                       const char** value)
{
    unicoap_options_iterator_t iterator;
    unicoap_options_iterator_init(&iterator, options);
    return unicoap_options_get_next_location_query_by_name(&iterator, name, value);
}

/**
 * @brief Copies location query string into the given buffer
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[in,out] queries Buffer of UTF-8 code units
 * @param capacity Capacity of @p queries
 *
 * @returns Length of string value in bytes on success, negative error number otherwise
 * @retval `-EBADOPT` Option corrupted
 * @retval `-ENOBUFS` @p queries lacks sufficient capacity to copy values
 *
 * This function creates a string from all `Location-Query` options
 * by joining them with the `&` separator.
 * The string will not be null-terminated. The resulting string uses UTF-8 encoding.
 */
static inline ssize_t unicoap_options_copy_location_queries(const unicoap_options_t* options,
                                                            char* queries, size_t capacity)
{
    return unicoap_options_copy_values_joined(options, UNICOAP_OPTION_LOCATION_QUERY,
                                              (uint8_t*)queries, capacity, '&');
}

/**
 * @brief Adds `Location-Query` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] query `Location-Query` value as buffer of UTF-8 code units
 * @param length Number of UTF-8 code units in @p query, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p query size must not exceed 255 bytes
 */
static inline ssize_t unicoap_options_add_location_query(unicoap_options_t* options, char* query,
                                                         size_t length)
{
    assert(length <= 255);
    return unicoap_options_add(options, UNICOAP_OPTION_LOCATION_QUERY, (uint8_t*)query, length);
}

/**
 * @brief Adds `Location-Query` option from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] query `Location-Query` value as buffer of UTF-8 code units, must be null-terminated
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 *
 * @pre @p query size must not exceed 255 bytes
 *
 * Uses `strlen` to calculate the number of code units.
 * @see @ref unicoap_options_add_location_query
 */
static inline ssize_t unicoap_options_add_location_query_string(unicoap_options_t* options,
                                                                char* query)
{
    return unicoap_options_add_location_query(options, query, strlen(query));
}

/**
 * @brief Adds multiple `Location-Query` options from string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] queries as buffer of UTF-8 code units with values separated by `&`
 * @param length Number of UTF-8 code units in @p queries, excluding null-terminator
 * (if present) in bytes
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 * @pre queries' size must not exceed 255 bytes
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 */
static inline ssize_t unicoap_options_add_location_queries(unicoap_options_t* options,
                                                           char* queries, size_t length)
{
    return unicoap_options_add_values_joined(options, UNICOAP_OPTION_LOCATION_QUERY,
                                             (uint8_t*)queries, length, '&');
}

/**
 * @brief Adds multiple `Location-Query` options from null-terminated string
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param[in] queries as null-terminated string of UTF-8 code units with values separated by `&`
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to add options
 * @pre queries size must not exceed 255 bytes
 *
 * If the string with the values you want to add _starts_ with the separator,
 * you can still use this function.
 * Uses `strlen` to calculate the number of code units.
 *
 * @see @ref unicoap_options_add_location_queries
 */
static inline ssize_t unicoap_options_add_location_queries_string(unicoap_options_t* options,
                                                                  char* queries)
{
    return unicoap_options_add_location_queries(options, queries, strlen(queries));
}

/**
 * @brief Removes all `Location-Query` options, if any
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_location_queries(unicoap_options_t* options)
{
    return unicoap_options_remove_all(options, UNICOAP_OPTION_LOCATION_QUERY);
}
/** @} */ /* Location-Query */

/* MARK: - Observe */
/**
 * @name Observe
 * @{
 */

/** @brief Observe value for registering for notifications */
#define UNICOAP_OBSERVE_OPTION_REGISTER   (0)

/** @brief Observe value for cancelling a registration for notifications */
#define UNICOAP_OBSERVE_OPTION_DEREGISTER (1)

/**
 * @brief Retrieves the `Observe` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] observe Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_observe(const unicoap_options_t* options,
                                                  uint32_t* observe)
{
    return unicoap_options_get_uint24(options, UNICOAP_OPTION_OBSERVE, observe);
}

/**
 * @brief Sets the `Observe` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param observe `Observe` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_observe(unicoap_options_t* options, uint32_t observe)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_OBSERVE, observe);
}

/**
 * @brief Removes the `Observe` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_observe(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_OBSERVE);
}

/**
 * @brief Sets the `Observe` option to a randomly generated value
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
int unicoap_options_set_observe_generated(unicoap_options_t* options);

/** @} */ /* Observe */

/* MARK: - Block-wise Transfers */
/**
 * @name Block-wise Transfers
 * @{
 */

/**
 * @brief Sentinel value for @ref unicoap_block_option_t indicating an uninitialized value
 * @note This constant occupies 4 bytes and does thus not represent a valid `Block1` or `Block2`
 * value.
 */
#define UNICOAP_BLOCK_OPTION_NONE (0x0f000000)

/**
 * @brief `Block1` and `Block2` option value
 */
typedef uint32_t unicoap_block_option_t;

/**
 * @brief Retrieves a `Block1` or `Block2` option
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param number Option number (@ref UNICOAP_OPTION_BLOCK1 or @ref UNICOAP_OPTION_BLOCK2)
 * @param[out] block Pointer to variable that will store `Block1` or `Block2` value
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_block(const unicoap_options_t* options,
                                                unicoap_option_number_t number,
                                                unicoap_block_option_t* block)
{
    return unicoap_options_get_uint24(options, number, block);
}

/**
 * @brief Sets the `Block1` or `Block2` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param number Option number (@ref UNICOAP_OPTION_BLOCK1 or @ref UNICOAP_OPTION_BLOCK2)
 * @param block `Block1` or `Block2` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline int unicoap_options_set_block(unicoap_options_t* options,
                                            unicoap_option_number_t number,
                                            unicoap_block_option_t block)
{
    assert(block <= UNICOAP_UINT24_MAX);
    return unicoap_options_set_uint(options, number, block);
}

/**
 * @brief Retrieves the `Block1` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] block Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_block1(const unicoap_options_t* options,
                                                 unicoap_block_option_t* block)
{
    return unicoap_options_get_block(options, UNICOAP_OPTION_BLOCK1, block);
}

/**
 * @brief Sets the `Block1` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param block `Block1` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_block1(unicoap_options_t* options,
                                                 unicoap_block_option_t block)
{
    return unicoap_options_set_block(options, UNICOAP_OPTION_BLOCK1, block);
}

/**
 * @brief Removes the `Block1` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_block1(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_BLOCK1);
}

/**
 * @brief Retrieves the `Block2` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] block Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_block2(const unicoap_options_t* options,
                                                 unicoap_block_option_t* block)
{
    return unicoap_options_get_block(options, UNICOAP_OPTION_BLOCK2, block);
}

/**
 * @brief Sets the `Block2` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param block `Block2` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_block2(unicoap_options_t* options,
                                                 unicoap_block_option_t block)
{
    return unicoap_options_set_block(options, UNICOAP_OPTION_BLOCK2, block);
}

/**
 * @brief Removes the `Block2` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_block2(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_BLOCK2);
}

/**
 * @brief Retrieves the `Size1` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] size Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_size1(const unicoap_options_t* options, uint32_t* size)
{
    return unicoap_options_get_uint32(options, UNICOAP_OPTION_SIZE1, size);
}

/**
 * @brief Sets the `Size1` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param size `Size1` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_size1(unicoap_options_t* options, uint32_t size)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_SIZE1, size);
}

/**
 * @brief Removes the `Size1` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_size1(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_SIZE1);
}

/**
 * @brief Retrieves the `Size2` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in] options Options to read from
 * @param[out] size Pointer to value variable
 *
 * @returns Size of option value in bytes on success, negative error number otherwise
 * @retval `-ENOENT` Option not found
 * @retval `-EBADOPT` Option corrupted
 */
static inline ssize_t unicoap_options_get_size2(const unicoap_options_t* options, uint32_t* size)
{
    return unicoap_options_get_uint32(options, UNICOAP_OPTION_SIZE2, size);
}

/**
 * @brief Sets the `Size2` option
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 * @param size `Size2` value
 *
 * @returns Zero on success, negative error number otherwise
 * @retval `-ENOBUFS` Options buffer lacks sufficient capacity to set option
 */
static inline ssize_t unicoap_options_set_size2(unicoap_options_t* options, uint32_t size)
{
    return unicoap_options_set_uint(options, UNICOAP_OPTION_SIZE2, size);
}

/**
 * @brief Removes the `Size2` option, if present
 * @memberof unicoap_options_t
 *
 * @param[in,out] options Options to write to
 *
 * @returns Zero on success, negative error number otherwise
 */
static inline int unicoap_options_remove_size2(unicoap_options_t* options)
{
    return unicoap_options_remove(options, UNICOAP_OPTION_SIZE2);
}
/** @} */ /* Block-wise Transfers */

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
