/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @defgroup    sys_ubjson Universal Binary JSON
 * @ingroup     sys
 * @brief       A library to read and write UBJSON serialized data.
 * @{
 *
 * @file
 * @brief       Headers for the UBJSON module
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef UBJSON_H__
#define UBJSON_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(MODULE_MSP430_COMMON)
#   include "msp430_types.h"
#elif !defined(__linux__)
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ***************************************************************************
 * READ FUNCTIONS / DEFINITIONS
 *************************************************************************** */

/**
 * @brief Status code of ubjson_read(), ubjson_read_array() and ubjson_read_object() callback
 *
 * When ubjson_read(), ubjson_read_array() and ubjson_read_object() iteratively invokes the gives callback function.
 * The callback function then has to invoke another function such as ubjson_get_i32(), depending on the parameter `type`.
 */
typedef enum {
    /**
     * @brief There is no such value.
     *
     * Only used in the callback of ubjson_read() for parameter `type2`.
     */
    UBJSON_ABSENT,

    /**
     * @brief The next datum is a null value.
     *
     * As you might have already guessed: you cannot read a null value.
     */
    UBJSON_TYPE_NULL,

    /**
     * @brief The next datum is a no-op value.
     *
     * As you might have already guessed: you cannot read a no-op value.
     */
    UBJSON_TYPE_NOOP,

    /**
     * @brief The next datum is a boolean.
     *
     * The `content` is the boolean value.
     * Use ubjson_get_bool() or use `content` verbatim.
     */
    UBJSON_TYPE_BOOL,

    /**
     * @brief The next datum is an integer that fits into an int32_t.
     *
     * Use ubjson_get_i32() to read the value.
     * `content` is one of ::ubjson_int32_type_t.
     */
    UBJSON_TYPE_INT32,

    /**
     * @brief The next datum is an integer that fits into an int64_t.
     *
     * Use ubjson_get_i64() to read the value.
     */
    UBJSON_TYPE_INT64,

    /**
     * @brief The next datum is a 32 bit floating point value.
     *
     * Use ubjson_get_float() to read the value.
     */
    UBJSON_TYPE_FLOAT,

    /**
     * @brief The next datum is a 64 bit floating point value.
     *
     * Use ubjson_get_double() to read the value.
     */
    UBJSON_TYPE_DOUBLE,

    /* NOTE: High-precision numbers are not implemented, yet. Implement if needed. */
    /* UBJSON_TYPE_HP_NUMBER, */

    /**
     * @brief The next datum is a string (blob).
     *
     * Use ubjson_get_string() to read the value.
     * `content` is the length of the blob.
     */
    UBJSON_TYPE_STRING,

    /**
     * @brief The next datum is an array.
     *
     * Use ubjson_read_array() to read its contents.
     */
    UBJSON_ENTER_ARRAY,

    /**
     * @brief The next datum is an object.
     *
     * Use ubjson_read_object() to read its contents.
     */
    UBJSON_ENTER_OBJECT,

    /**
     * @brief The next datum is an array index.
     *
     * This value is emitted for every index in a call to ubjson_read_array().
     *
     * `content1` is the array index.
     * `type2` and `content2` describe the value of the index.
     *
     * Arrays can be nested.
     */
    UBJSON_INDEX,

    /**
     * @brief The next datum is an object key.
     *
     * This value is emitted for every index in a call to ubjson_read_object().
     *
     * `content1` is the length of the key, invoke ubjson_get_string().
     * `type2` and `content2` describe the value.
     *
     * Objects can be nested.
     */
    UBJSON_KEY,
} ubjson_type_t;

/**
 * @brief Length of the UBJSON_TYPE_INT32 datum.
 */
typedef enum {
    UBJSON_INT32_INT8,  /**< The stream contains an int8_t. */
    UBJSON_INT32_UINT8, /**< The stream contains an uint8_t. */
    UBJSON_INT32_INT16, /**< The stream contains an int16_t. */
    UBJSON_INT32_INT32, /**< The stream contains an int32_t. */
} ubjson_int32_type_t;

/**
 * @brief Return value of ::ubjson_read_callback_t and ubjson_read()
 *
 * The callback invoked by ubjson_read(), ubjson_read_array() or ubjson_read_object() can return an error value.
 * The error value is then returned by the read function.
 *
 * The values UBJSON_INVALID_DATA, UBJSON_PREMATURELY_ENDED, and UBJSON_SIZE_ERROR are returned on encoding errors, too.
 */
typedef enum {
    UBJSON_OKAY,              /**< success / do continue */
    UBJSON_ABORTED,           /**< aborted / do abort */
    UBJSON_INVALID_DATA,      /**< invalid marker or type*/
    UBJSON_PREMATURELY_ENDED, /**< the stream abruptly ended */
    UBJSON_SIZE_ERROR,        /**< the length of a field exceeded SSIZE_MAX */
} ubjson_read_callback_result_t;

struct ubjson_cookie;

/**
 * @brief         A cookie passed between the read and write functions.
 * @details       You probably want to wrap the cookie in some other data structure,
 *                which you retrieve with container_of() in the callback.
 */
typedef struct ubjson_cookie ubjson_cookie_t;

/**
 * @brief         Method called by ubjson_read() to get more data.
 * @param[in]     cookie    The cookie that was passed to ubjson_read().
 * @param[out]    buf       The buffer that should be written to.
 * @param[in]     max_len   The length of the buffer. Always `>= 1`.
 * @return        @arg `< 0` on error. UBJSON_PREMATURELY_ENDED will be return by ubjson_read().
 *                @arg `> 0` the amount of read data, which must not exceed max_len.
 */
typedef ssize_t (*ubjson_read_t)(ubjson_cookie_t *__restrict cookie, void *buf, size_t max_len);

/**
 * @brief         Method called by ubjson_read() to denote the next element in the structure.
 * @details       Depending on the value of type1 a different function, such as ubjson_get_i32(),
 *                must be invoked by the callback function.
 *
 *                With ubjson_read_array() or ubjson_read_object() the value of type1 is
 *                UBJSON_INDEX or UBJSON_KEY, resp.
 * @param[in]     cookie     The cookie that was passed to ubjson_read().
 * @param[in]     type1      The type of the next datum.
 * @param[in]     content1   The sub-type of the next datum.
 * @param[in]     type2      The type of the value that belongs to the next key/index, or UBJSON_ABSENT.
 * @param[in]     content2   The sub-type of the value that belongs to the next key/index.
 * @returns       Either UBJSON_OKAY or UBJSON_ABORTED.
 */
typedef ubjson_read_callback_result_t (*ubjson_read_callback_t)(ubjson_cookie_t *__restrict cookie,
                                                                ubjson_type_t type1, ssize_t content1,
                                                                ubjson_type_t type2, ssize_t content2);

/**
 * @brief         Method called by ubjson_write_null() and friends.
 * @details       The function in invoked multiple times per written value.
 *                You should use some kind of buffer if you send the data over a stream.
 *
 *                The function must write the whole buffer before returning.
 * @param[in]     cookie     The cookie that was passed to ubjson_write_init().
 * @param[in]     buf        Data to write, never NULL.
 * @param[in]     len        Data to write, always >= 0.
 * @returns       @arg `< 0` to indicate an error.
 *                @arg `> 0` to indicate success.
 */
typedef ssize_t (*ubjson_write_t)(ubjson_cookie_t *__restrict cookie, const void *buf, size_t len);

/**
 * @brief        See @ref ubjson_cookie_t.
 */
struct ubjson_cookie {
    /**
     * @brief     Read/write function
     * @internal
     */
    union {
        ubjson_read_t read;   /**< read function */
        ubjson_write_t write; /**< write function */
    } rw;

    /**
     * @brief     Callback function
     * @internal
     */
    union {
        ubjson_read_callback_t read; /**< Callback when a datum was read. */
    } callback; /**< @internal */

    /**
     * @brief     One byte push-back buffer.
     * @internal
     */
    char marker;
};

/**
 * @brief         Used to read with a setup cookie.
 * @details       You need to use this function instead of ubjson_read() only if
 *                your UBJSON data contains further UBJSON serialized data in a string.
 *
 *                UBJSON data in a typed array may or may not work.
 * @param[in]     cookie     The cookie that is passed to the callback function.
 * @returns       The same as ubjson_read().
 */
ubjson_read_callback_result_t ubjson_read_next(ubjson_cookie_t *__restrict cookie);

/**
 * @brief         The entry function to read UBJSON serialized data.
 * @details       This function invokes the callback function.
 *                The value of type1 in the callback indicates which function
 *                to call inside the callback function, e.g. ubjson_get_i32().
 *
 *                Nested calls to ubjson_read_array(), ubjson_read_object() or ubjson_read_next()
 *                invoke the same callback function, possibly multiple times.
 *
 *                You probably want to wrap the cookie in some other data structure,
 *                which you retrieve with container_of() in the callback.
 * @param[in]     cookie     The cookie that is passed to the callback function.
 * @param[in]     read       The function that is called to receive more data.
 * @param[in]     callback   The callback function.
 * @returns       See \ref ubjson_read_callback_result_t
 */
static inline ubjson_read_callback_result_t ubjson_read(ubjson_cookie_t *__restrict cookie,
                                                        ubjson_read_t read,
                                                        ubjson_read_callback_t callback)
{
    cookie->rw.read = read;
    cookie->callback.read = callback;
    cookie->marker = 0;
    return ubjson_read_next(cookie);
}

/**
 * @brief         Use in a callback if type1 is UBJSON_KEY or UBJSON_INDEX.
 * @details       Call like ``ubjson_peek_value(cookie, &type2, &content2)``.
 * @param[in]     cookie     The cookie that was passed to the callback.
 * @param[in,out] type       Pointer to a variable that was initialized with the value of type2, returns the new type1.
 * @param[in,out] content    Pointer to a variable that was initialized with the value of content2, returns the new content1.
 * @returns       The same as ubjson_read().
 */
ubjson_read_callback_result_t ubjson_peek_value(ubjson_cookie_t *__restrict cookie,
                                                ubjson_type_t *type, ssize_t *content);

/**
 * @brief         Call if type1 of the callback was UBJSON_TYPE_INT32.
 * @details       The value of content1 is one of ubjson_int32_type_t.
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @param[in]     content    The content1 that was passed to the callback function.
 * @param[out]    dest       The read datum.
 * @returns       The result of the read callback, probably the amount of read bytes.
 */
ssize_t ubjson_get_i32(ubjson_cookie_t *__restrict cookie, ssize_t content, int32_t *dest);

/**
 * @brief         Call if type1 of the callback was UBJSON_TYPE_INT64.
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @param[in]     content    The content1 that was passed to the callback function.
 * @param[out]    dest       The read datum.
 * @returns       The result of the read callback, probably the amount of read bytes.
 */
ssize_t ubjson_get_i64(ubjson_cookie_t *__restrict cookie, ssize_t content, int64_t *dest);

/**
 * @brief         Call if type1 of the callback was UBJSON_TYPE_STRING.
 * @details       content1 is the length of the string/blob.
 *                The result is not null-terminated!
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @param[in]     content    The content1 that was passed to the callback function.
 * @param[out]    dest       Buffer to read the string into.
 * @returns       The result of the read callback, probably the amount of read bytes.
 */
ssize_t ubjson_get_string(ubjson_cookie_t *__restrict cookie, ssize_t content, void *dest);

/**
 * @brief         Call if type1 of the callback was UBJSON_TYPE_BOOL.
 * @details       content1 is the value of the bool. The function only exists for symmetry.
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @param[in]     content    The content1 that was passed to the callback function.
 * @param[out]    dest       The read datum.
 * @returns       `1`, the invocation cannot fail.
 */
static inline ssize_t ubjson_get_bool(ubjson_cookie_t *__restrict cookie, ssize_t content, bool *dest)
{
    (void) cookie;
    *dest = content;
    return 1;
}

/**
 * @brief         Call if type1 of the callback was UBJSON_TYPE_FLOAT.
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @param[in]     content    The content1 that was passed to the callback function.
 * @param[out]    dest       The read datum.
 * @returns       The result of the read callback, probably the amount of read bytes.
 */
static inline ssize_t ubjson_get_float(ubjson_cookie_t *__restrict cookie, ssize_t content, float *dest)
{
    (void) content;
    union {
        float f;
        int32_t i;
    } value;
    ubjson_read_callback_result_t result = ubjson_get_i32(cookie, UBJSON_INT32_INT32, &value.i);
    *dest = value.f;
    return result;
}

/**
 * @brief         Call if type1 of the callback was UBJSON_TYPE_DOUBLE.
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @param[in]     content    The content1 that was passed to the callback function.
 * @param[out]    dest       The read datum.
 * @returns       The result of the read callback, probably the amount of read bytes.
 */
static inline ssize_t ubjson_get_double(ubjson_cookie_t *__restrict cookie, ssize_t content, double *dest)
{
    (void) content;
    union {
        double f;
        int64_t i;
    } value;
    ubjson_read_callback_result_t result = ubjson_get_i64(cookie, -1, &value.i);
    *dest = value.f;
    return result;
}

/**
 * @brief         Call if type1 of the callback was UBJSON_ENTER_ARRAY.
 * @details       Inside this call the callback function will be invoked multiple times,
 *                once per array element, with type1=UBJSON_INDEX,
 *                and content1=running index in the array.
 *
 *                Use ubjson_peek_value() to determine the type of the element.
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @returns       The same as ubjson_read().
 */
ubjson_read_callback_result_t ubjson_read_array(ubjson_cookie_t *__restrict cookie);


/**
 * @brief         Call if type1 of the callback was UBJSON_ENTER_OBJECT.
 * @details       Inside this call the callback function will be invoked multiple times,
 *                once per object element, with type1=UBJSON_KEY,
 *                and content1=length of the key string.
 *
 *                First read the key with ubjson_get_string(), then
 *                use ubjson_peek_value() to determine the type of the element.
 * @param[in]     cookie     The cookie that was passed to the callback function.
 * @returns       The same as ubjson_read().
 */
ubjson_read_callback_result_t ubjson_read_object(ubjson_cookie_t *__restrict cookie);

/* ***************************************************************************
 * WRITE FUNCTIONS / DEFINITIONS
 *************************************************************************** */

/**
 * @brief         The first call when you serialize data to UBJSON.
 * @details       There is no corresponding "ubjson_write_finish" function.
 *                The programmer needs to ensure that the API is used correctly.
 *                The library won't complain if you write multiple values that are not
 *                inside an array or object. The result will just not be properly serialized.
 * @param[out]    cookie     The cookie that will be passed to ubjson_write_null() and friends.
 * @param[in]     write_fun  The function that will be called to write data.
 */
static inline void ubjson_write_init(ubjson_cookie_t *__restrict cookie, ubjson_write_t write_fun)
{
    cookie->rw.write = write_fun;
}

/**
 * @brief         Write a null value.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 */
ssize_t ubjson_write_null(ubjson_cookie_t *__restrict cookie);

/**
 * @brief         Write a no-operation value.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_write_noop(ubjson_cookie_t *__restrict cookie);

/**
 * @brief         Write a boolean value.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     value      The boolean value to write.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_write_bool(ubjson_cookie_t *__restrict cookie, bool value);

/**
 * @brief         Write an integer value.
 * @details       The library will determine the smallest serialization for the value itself.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     value      The integer value to write.
 */
ssize_t ubjson_write_i32(ubjson_cookie_t *__restrict cookie, int32_t value);

/**
 * @brief         Write an integer value.
 * @details       The library will determine the smallest serialization for the value itself.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     value      The integer value to write.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_write_i64(ubjson_cookie_t *__restrict cookie, int64_t value);

/**
 * @brief         Write a floating point value.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     value      The integer value to write.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_write_float(ubjson_cookie_t *__restrict cookie, float value);

/**
 * @brief         Write a floating point value.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     value      The integer value to write.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_write_double(ubjson_cookie_t *__restrict cookie, double value);

/**
 * @brief         Write a string or blob.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     value      The string or blob to write.
 * @param[in]     len        The length of the string or blob.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_write_string(ubjson_cookie_t *__restrict cookie, const void *value, size_t len);

/**
 * @brief         Open an array.
 * @details       Write multiple elements inside this array.
 *                Call ubjson_close_array() after the whole content was written.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_open_array(ubjson_cookie_t *__restrict cookie);

/**
 * @brief         Open an array with a known length.
 * @details       Do not call ubjson_close_array().
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     len        Length of the array.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_open_array_len(ubjson_cookie_t *__restrict cookie, size_t len);

/**
 * @brief         Close an array that was opened with ubjson_open_array().
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_close_array(ubjson_cookie_t *__restrict cookie);

/**
 * @brief         Open an object.
 * @details       Write multiple keys inside this object.
 *                Call ubjson_close_object() after the whole content was written.
 *
 *                For each element first write the key with ubjson_write_key(),
 *                then invoke the function to write the value.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_open_object(ubjson_cookie_t *__restrict cookie);

/**
 * @brief         Open an object with a known length.
 * @details       For each element first write the key with ubjson_write_key(),
 *                then invoke the function to write the value.
 *
 *                Do not call ubjson_close_object().
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     len        Number of keys inside the object.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_open_object_len(ubjson_cookie_t *__restrict cookie, size_t len);

/**
 * @brief         Write a key inside an object.
 * @details       For each element first write the key,
 *                then invoke the function to write the value.
 *
 *                It is up to the programmer to ensure that there are no duplicated keys.
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @param[in]     value      The key, should be a UTF-8 string.
 * @param[in]     len        The length of the key.
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_write_key(ubjson_cookie_t *__restrict cookie, const void *value, size_t len);

/**
 * @brief         Close an array that was opened with ubjson_open_object().
 * @param[in]     cookie     The cookie that was initialized with ubjson_write_init().
 * @returns       The result of the supplied @ref ubjson_write_t function.
 */
ssize_t ubjson_close_object(ubjson_cookie_t *__restrict cookie);

#ifdef __cplusplus
}
#endif

#endif /* ifndef UBJSON_H__ */
/** @} */
