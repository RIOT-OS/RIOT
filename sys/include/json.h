/*
 * Copyright (C) 2014-2016  René Kijewski  <rene.kijewski@fu-berlin.de>
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
 * @defgroup    sys_json JSON Data Interchange Format
 * @ingroup     sys
 * @brief       A library to read and write JSON
 *              ([ECMA-404](http://www.ecma-international.org/publications/standards/Ecma-404.htm))
 *              serialized data.
 * @{
 *
 * @file
 * @brief       Headers for the JSON module
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef JSON_H
#define JSON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Result of JSON read and write functions.
 */
typedef enum {
    JSON_OKAY,                /**< success / do continue */
    JSON_INVALID_DATA,        /**< invalid marker or type */
    JSON_PREMATURELY_ENDED,   /**< the stream (abruptly) ended */
    JSON_READ_UNICODE,        /**< you have to invoke json_read_unicode() */
} json_result_t;

/* ***************************************************************************
 * WRITE FUNCTIONS
 *************************************************************************** */

/**
 * @defgroup    sys_json_write JSON serialization
 * @brief       The functions used to serialize data into a JSON stream.
 * @{
 */

struct json_write_cookie;

/**
 * @brief   See @ref json_write_cookie.
 */
typedef struct json_write_cookie json_write_cookie_t;

/**
 * @brief   Prototype for a JSON write callback.
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 * @param[in] buf               Buffer to write.
 * @param[in] len               Length of the buffer to write.
 *
 * @returns `> 0` number of written bytes.
 * @returns `< 0` to indicate an error.
 */
typedef ssize_t (*json_write_t)(json_write_cookie_t *cookie, const char *buf, size_t len);

/**
 * @brief   Internal state of the write cookie.
 * @internal
 */
typedef enum {
    JSON_WRITE_STATE_SOMEWHERE, /**< None of the other states. */
    JSON_WRITE_STATE_IN_STRING, /**< json_write_string() was last called */
    JSON_WRITE_STATE_IN_OBJECT, /**< json_write_object_open() was last called */
    JSON_WRITE_STATE_IN_ARRAY,  /**< json_write_array_open() was last called */
    JSON_WRITE_STATE_IN_KEY,    /**< json_write_object_key() was last called */
    JSON_WRITE_STATE_DONE,      /**< json_write_finish() was called  */
} json_write_state_t;

/**
 * @brief   The cookie to store and pass all the information needed to serialize JSON data.
 *
 * @see @ref container_of
 * @see json_write_init()
 */
struct json_write_cookie {
    json_write_t write;       /**< The write function passed to json_write_init(). */
    json_write_state_t state; /**< The internal state of the stream. */
};

/**
 * @brief   Initializer for JSON serialization streams.
 *
 * @param[out] cookie           The cookie to subsequently use.
 * @param[in] write             The write function to call back.
 * @see json_write_finish()
 */
static inline void json_write_init(json_write_cookie_t *cookie, json_write_t write)
{
    cookie->write = write;
    cookie->state = JSON_WRITE_STATE_SOMEWHERE;
}

/**
 * @brief End a JSON serialization stream.
 *
 * @details An empty stream without any data is invalid.
 * @see json_write_init()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 */
json_result_t json_write_finish(json_write_cookie_t *cookie);

/**
 * @brief   Open an object to serialize.
 *
 * @details Inside the object call json_write_object_key() to write a key,
 *          then invoke any other write function to write the value associated to this key.
 *          Repeat until you are done, then call json_write_object_close().
 *          An empty object is valid, too.
 * @see json_write_object_close()
 * @see json_write_object_key()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_object_open(json_write_cookie_t *cookie);

/**
 * @brief   Closes an object.
 *
 * @see json_write_object_open()
 * @see json_write_object_key()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_object_close(json_write_cookie_t *cookie);

/**
 * @brief   Write a key inside an object.
 *
 * @details Invoke this function multiple times directly to concatenate this substring.
 *          You could write out a string `"test"` character by character if you want to.
 *          An empty key is valid, too.
 * @see json_write_object_open()
 * @see json_write_object_close()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 * @param[in] string            The (substring of the) key to write.
 * @param[in] len               The length of @p string.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_object_key(json_write_cookie_t *cookie, const char *string, size_t len);

/**
 * @brief   Open an array to serialize.
 *
 * @see json_write_array_close()
 * @see json_write_array_next()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_array_open(json_write_cookie_t *cookie);

/**
 * @brief   Separate items inside the array.
 *
 * @details Inside the array call opened with json_write_array_open() call
 *          json_write_array_next() to separate values.
 *          Calling json_write_array_next() directly after opening the array
 *          is valid but not needed.
 *          Calling json_write_array_next() before closing the array with
 *          json_write_array_close() without putting an item is invalid.
 * @see json_write_object_open()
 * @see json_write_array_next()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_array_close(json_write_cookie_t *cookie);

/**
 * @brief   Close an array.
 *
 * @details See description in json_write_array_close().
 * @see json_write_object_open()
 * @see json_write_array_close()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_array_next(json_write_cookie_t *cookie);

/**
 * @brief   Write a single integer value.
 *
 * @details JSON does not differentiate between integers and floating point numbers.
 * @warning JSON is an unframed format. If the data to deliver consists of a single number,
 *          then there is a fair chance that it will not be processed correctly by the decoder side.
 * @see json_write_float()
 * @see json_write_int64()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 * @param[in] value             The value to put into the data stream.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_int(json_write_cookie_t *cookie, int32_t value);

/**
 * @brief   Write a floating point number.
 *
 * @details JSON does not differentiate between integers and floating point numbers.
 * @warning The number must be finite, i.e. it must not be NaN, +Inf or -Inf.
 *          Illegal inputs are not caught but the stream will be broken.
 * @warning JSON is an unframed format. If the data to deliver consists of a single number,
 *          then there is a fair chance that it will not be processed correctly by the decoder side.
 * @see json_write_int()
 * @see json_write_int64()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 * @param[in] value             The value to put into the data stream.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_float(json_write_cookie_t *cookie, float value);

#ifndef MODULE_ATMEGA_COMMON
/**
 * @brief   Write a single integer value.
 *
 * @details JSON does not differentiate between integers and floating point numbers.
 * @warning JSON is an unframed format. If the data to deliver consists of a single number,
 *          then there is a fair chance that it will not be processed correctly by the decoder side.
 * @see json_write_int()
 * @see json_write_float()
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 * @param[in] value             The value to put into the data stream.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_int64(json_write_cookie_t *cookie, int64_t value);
#endif

/**
 * @brief   Write a string.
 *
 * @details The character encoding should be UTF-8, but any ASCII-compatible encoding will work
 *          (e.g. Latin-1, UTF-8, but not UTF-7, UTF-16).
 *
 *          Call this function multiple times to concatenate multiple substrings.
 *
 *          To put an object key use json_write_object_key() instead.
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 * @param[in] string            The substring to put into the stream.
 * @param[in] len               The length of the substring.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_string(json_write_cookie_t *cookie, const char *string, size_t len);

/**
 * @brief   Write the literal `true`.
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_true(json_write_cookie_t *cookie);

/**
 * @brief   Write the literal `false`.
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_false(json_write_cookie_t *cookie);

/**
 * @brief   Write the literal `null`.
 *
 * @param[in] cookie            The cookie, initialized with json_write_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream is inside an invalid state.
 * @returns @ref JSON_PREMATURELY_ENDED if the data could not be written.
 */
json_result_t json_write_null(json_write_cookie_t *cookie);

/**
 * @}
 */

/* ***************************************************************************
 * READ FUNCTIONS
 *************************************************************************** */

/**
 * @defgroup    sys_json_read JSON deserialization
 * @brief       The functions used to deserialize data from a JSON stream.
 * @{
 */

/**
 * @brief   Indicator for a JSON type.
 *
 * @see json_read_peek()
 */
typedef enum {
    /**
     * @brief An object: `{}`
     * @see json_write_object_open()
     * @see json_write_object_key()
     * @see json_write_object_close()
     * @see json_read_object()
     * @see json_read_object_next()
     */
    JSON_OBJECT,

    /**
     * @brief An array: `[]`
     * @see json_write_array_open()
     * @see json_write_array_next()
     * @see json_write_array_close()
     * @see json_read_array()
     * @see json_read_array_next()
     */
    JSON_ARRAY,

    /**
     * @brief A number: `42`, `47.11`, ...
     * @see json_write_int()
     * @see json_write_int64()
     * @see json_write_float()
     * @see json_read_number()
     * @see json_number_type()
     */
    JSON_NUMBER,

    /**
     * @brief A string: `"RIOT"`
     * @see json_write_string()
     * @see json_read_string()
     */
    JSON_STRING,

    /**
     * @brief The literal `true`
     * @see json_write_true()
     * @see json_read_true()
     */
    JSON_TRUE,

    /**
     * @brief The literal `false`
     * @see json_write_false()
     * @see json_read_false()
     */
    JSON_FALSE,

    /**
     * @brief The literal `null`
     * @see json_write_null()
     * @see json_read_null()
     */
    JSON_NULL,
} json_type_t;

struct json_read_cookie;

/**
 * @brief See @ref json_read_cookie.
 */
typedef struct json_read_cookie json_read_cookie_t;

/**
 * @brief   The function used to read a byte from the input stream.
 *
 * @details If your input is a `char` array, then explicitly cast the return value
 *          to `unsigned char` or an 8-bit input will be interpreted as the end of the stream.
 *
 *          The result does not differentiate between the end of the stream and the end of the input.
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns `0 <= result <= 255`: the read byte.
 * @returns `result < 0`: the stream ended or there was an error.
 */
typedef int (*json_read_t)(json_read_cookie_t *cookie);

/**
 * @brief The internal state of the input stream.
 */
typedef enum {
    JSON_READ_STATE_SOMEWHERE,   /**< No other state applies. */
    JSON_READ_STATE_IN_OBJECT,   /**< The last consumed character was `{`. */
    JSON_READ_STATE_IN_ARRAY,    /**< The last consumed character was `[`. */
    JSON_READ_STATE_IN_SEQUENCE, /**< The stream is inside a sequence (a key, a string or a number). */
} json_read_state_t;

/**
 * @brief   The cookie that passes all the needed state and information to deserialize a JSON stream.
 *
 * @see json_read_init()
 * @see @ref container_of
 */
struct json_read_cookie {
    json_read_t read;        /**< The function that is used to get the next character. */
    json_read_state_t state; /**< The current state of the stream. */
    int32_t pushback;        /**< The last read but not consumed character, or the codepoint to read, or `< 0` if empty. */
};

/**
 * @brief   Initialize a JSON input stream.
 *
 * @details There is no `json_read_finish()`.
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 * @param[in] read              The function that is used to get the next character.
 */
static inline void json_read_init(json_read_cookie_t *cookie, json_read_t read)
{
    cookie->read = read;
    cookie->state = JSON_READ_STATE_SOMEWHERE;
    cookie->pushback = -1;
}

/**
 * @brief   Determine the type of the next item in the stream.
 *
 * @details You may invoke this function multiple times on the same item.
 * @warning Do not call this function inside an array or object before you have called
 *          json_read_object_next() or json_read_array_next().
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 * @param[out] type             The type if the next item.
 *
 * @returns @ref JSON_OKAY if the type if the next item was determined.
 * @returns @ref JSON_INVALID_DATA if the next non-whitespace character is not valid for any data type.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_peek(json_read_cookie_t *cookie, json_type_t *type);
/**
 * @brief   Read out a Unicode codepoint from the stream.
 *
 * @details If json_read_object_next() or json_read_string() returned @ref JSON_READ_UNICODE,
 *          then you need to call this function to read out the code point.
 *          @ref JSON_READ_UNICODE will only occur for `\uXXXX` or `\uXXXX\uYYYY` sequences,
 *          and only if the value was not in ASCII-range `[\u0000 .. \u007F]`.
 * @see json_codepoint_to_utf8()
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns The Unicode codepoint that was read.
 */
static inline uint32_t json_read_unicode(json_read_cookie_t *cookie)
{
    uint32_t codepoint = cookie->pushback;
    cookie->pushback = -1;
    return codepoint;
}

/**
 * @brief   Read an object.
 *
 * @details Call this function if json_read_peek() returned the type @ref JSON_OBJECT,
 *          or if you know that an object follows.
 *
 *          There is no `json_read_object_close()` function.
 *          json_read_object_next() tells you if the object was completely read.
 * @see json_read_object_next()
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the input was not an object.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_object(json_read_cookie_t *cookie);

/**
 * @brief   Read the next object key.
 *
 * @details If json_read_peek() returned @ref JSON_OBJECT, then first call json_read_object().
 *          Then in a loop json_read_object_next(), json_read_peek(), `json_read_XXX()`.
 *          Repeat until @p object_closed is @c true.
 *
 *          You can read the key in substrings.
 *          For that call json_read_object_next() until @p key_read is @c true.
 *          Before you proceed you have to read the whole key.
 * @warning @p key_buffer is not null-terminated. @p key_len_out is not initialized.
 * @see json_read_object()
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 * @param[out] object_closed    There is no next key, the object is finished.
                                In this case all other parameters are left unchanged.
 * @param[out] key_buffer       The buffer to read the key into.
 * @param[in] key_len_in        The length of @p key_buffer.
 * @param[in,out] key_len_out   This variable is incremented for every written byte.
 * @param[out] key_read         @c true if the whole key was read. Spurious on @ref JSON_READ_UNICODE.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_READ_UNICODE if the key contained a Unicode sequence, call json_read_unicode().
 * @returns @ref JSON_INVALID_DATA if the stream contained illegal data.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_object_next(json_read_cookie_t *cookie, bool *object_closed,
                                    char *key_buffer, size_t key_len_in,
                                    size_t *key_len_out, bool *key_read);

/**
 * @brief   Read an array.
 *
 * @details Call this function if json_read_peek() returned the type @ref JSON_ARRAY,
 *          or if you know that an array follows.
 *
 *          There is no `json_read_array_close()` function.
 *          json_read_array_next() tells you if the object was completely read.
 * @see json_read_array_next()
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the input was not an array.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_array(json_read_cookie_t *cookie);

/**
 * @brief   Read the next array item.
 *
 * @details If json_read_peek() returned @ref JSON_ARRAY, then first call json_read_array().
 *          Then in a loop json_read_array_next(), json_read_peek(), `json_read_XXX()`.
 *          Repeat until @p array_closed is @c true.
 * @see json_read_array()
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 * @param[out] array_closed     @c true if the array was finished.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the input was invalid.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_array_next(json_read_cookie_t *cookie, bool *array_closed);

/**
 * @brief   Read a string.
 *
 * @details Call this function if json_read_peek() returned the type @ref JSON_STRING,
 *          or if you know that a string follows.
 *
 *          You can read the string in substrings.
 *          Invoke json_read_string() multiple times until @p done is @c true.
 * @warning @p buffer is not null-terminated. @p len_out is not initialized.
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 * @param[out] buffer           The buffer to write the string into.
 * @param[in] len_in            The size of @p buffer.
 * @param[in,out] len_out       This is incremented for every written byte.
 * @param[out] done             Tells if the string was completely read.
 *                              Spurious on @ref JSON_READ_UNICODE.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_READ_UNICODE if the string contained a Unicode sequence, call json_read_unicode().
 * @returns @ref JSON_INVALID_DATA if the stream contained illegal data.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_string(json_read_cookie_t *cookie,
                               char *buffer, size_t len_in,
                               size_t *len_out, bool *done);

/**
 * @brief   Read a number.
 *
 * @details Call this function if json_read_peek() returned the type @ref JSON_NUMBER,
 *          or if you know that a number follows.
 *
 *          JSON does not differentiate between integers and floating point numbers.
 *          Use json_number_type() to see what type of number the input is.
 *
 *          The input might be broken. This function only tests if the input contains
 *          only characters that can occur in a number, but it does not keep track if
 *          they are in the correct order.
 *
 *          You can read the number in substrings.
 *          Invoke json_read_number() multiple times until @p done it @c true.
 * @warning @p buffer is not null-terminated. @p len_out is not initialized.
 * @see json_number_type()
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 * @param[out] buffer           The buffer to write the number representation into.
 * @param[in] len_in            The size of @p buffer.
 * @param[in,out] len_out       This is incremented for every written byte.
 * @param[out] done             Tells if the number was completely read.
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream contained illegal data.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_number(json_read_cookie_t *cookie,
                               char *buffer, size_t len_in,
                               size_t *len_out, bool *done);

/**
 * @brief   Read a literal `true`.
 *
 * @details Call this function if json_read_peek() returned the type @ref JSON_TRUE,
 *          or if you know that a literal `true` follows.
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream contained illegal data.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_true(json_read_cookie_t *cookie);

/**
 * @brief   Read a literal `false`.
 *
 * @details Call this function if json_read_peek() returned the type @ref JSON_FALSE,
 *          or if you know that a literal `false` follows.
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream contained illegal data.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_false(json_read_cookie_t *cookie);

/**
 * @brief   Read a literal `null`.
 *
 * @details Call this function if json_read_peek() returned the type @ref JSON_NULL,
 *          or if you know that a literal `null` follows.
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns @ref JSON_OKAY on success.
 * @returns @ref JSON_INVALID_DATA if the stream contained illegal data.
 * @returns @ref JSON_PREMATURELY_ENDED if the input stream was ended or had an error.
 */
json_result_t json_read_null(json_read_cookie_t *cookie);

/**
 * @brief   Strip a range of of whitespace characters between values.
 * @details This function is only useful when you need to make sure that all remaining
 *          data is only spaces/padding. For this usecase @ref JSON_PREMATURELY_ENDED
 *          is the **good** outcome, and @ref JSON_INVALID_DATA would indicate an error.
 *
 *          This function does not return @ref JSON_OKAY in any case.
 *
 * @param[in] cookie            The cookie, initialized with json_read_init().
 *
 * @returns @ref JSON_PREMATURELY_ENDED The input was exhausted.
 * @returns @ref JSON_INVALID_DATA Non-whitespace characters were encountered.
 */
json_result_t json_read_read_spaces(json_read_cookie_t *cookie);

/**
 * @}
 */

/* ***************************************************************************
 * UTILITY FUNCTIONS
 *************************************************************************** */

/**
 * @defgroup    sys_json_util JSON utilities
 * @brief       JSON utility and convenience functions.
 * @{
 */

/**
 * @brief   The type of number number in string representation.
 *
 * @see json_read_number()
 * @see json_number_type()
 */
typedef enum {
    /**
     * @brief   The integer value zero.
     */
    JSON_NUMBER_ZERO,

    /**
     * @brief   An integer.
     *
     * @see json_number_to_int()
     */
    JSON_NUMBER_INTEGER,

    /**
     * @brief   A floating point number.
     */
    JSON_NUMBER_FLOAT,
} json_number_t;

/**
 * @brief   Determine the type of a number read with json_read_number().
 *
 * @details Even though the data is a valid string representation of an integer or float,
 *          the number may still not fit into the respective data type.
 *
 * @param[in] str               The read string.
 * @param[in] len               The length of the read string.
 * @param[out] type             The type of the number.
 *
 * @returns @ref JSON_OKAY if the string is a valid representation of a number.
 * @returns @ref JSON_PREMATURELY_ENDED if @p len == 0.
 * @returns @ref JSON_INVALID_DATA If the data is not a valid number representation.
 */
json_result_t json_number_type(const char *str, size_t len, json_number_t *type);

/**
 * @brief   Convert a string read with json_read_number() into an int64_t.
 *
 * @details Use json_number_type() to determine if this function or json_number_to_float()
 *          is the correct function to invoke.
 * @warning Even though the number represents an integer, it might still not fit into an int64_t.
 *          This case is not caught by this convenience function.
 *
 * @param[in] str               The string read with json_read_number().
 * @param[in] len               The length of the string.
                                Use `0` if you have zero-terminated @p str.
 *
 * @returns The number represented by @p str.
 */
int64_t json_number_to_int(const char *str, size_t len);

/**
 * @brief   Convert a string read with json_read_number() into a float.
 *
 * @details Use json_number_type() to determine if this function or json_number_to_int()
 *          is the correct function to invoke.
 * @warning Even though the number represents an integer, it might still not fit into a float.
 *          This case is not caught by this convenience function.
 *
 * @param[in] str               The string read with json_read_number().
 * @param[in] len               The length of the string.
                                Use `0` if you have zero-terminated @p str.
 *
 * @returns The number represented by @p str.
 */
float json_number_to_float(const char *str, size_t len);

/**
 * @brief   Encode a Unicode codepoint to a UTF-8 sequence.
 *
 * @see json_read_unicode()
 *
 * @param[in] codepoint         The Unicode codepoint to encode.
 * @param[out] output           The encoded data, at least 4 bytes.
 * @param[out] len              The length of the encoded sequence.
 *
 * @returns @ref JSON_OKAY on success
 * @returns @ref JSON_INVALID_DATA if the codepoint of not in the valid range.
 */
json_result_t json_codepoint_to_utf8(uint32_t codepoint, char *output, size_t *len);

/**
 * @}
 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* JSON_H */

/**
 * @}
 */
