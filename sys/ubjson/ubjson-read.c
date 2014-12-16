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
 * @ingroup     sys
 * @{
 * @file
 * @brief       Universal Binary JSON deserializer
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include "ubjson-internal.h"
#include "ubjson.h"
#include "byteorder.h"

#define READ_MARKER()                                  \
    do {                                               \
        result = _ubjson_read_marker(cookie, &marker); \
        if (result != UBJSON_OKAY) {                   \
            return result;                             \
        }                                              \
    } while (0)

static ubjson_read_callback_result_t _ubjson_read_marker(ubjson_cookie_t *restrict cookie,
                                                         char *marker)
{
    while (1) {
        ssize_t bytes_read;

        *marker = cookie->marker;
        if (!*marker) {
            bytes_read = cookie->rw.read(cookie, marker, 1);
        }
        else {
            cookie->marker = 0;
            bytes_read = 1;
        }

        if (bytes_read == 1) {
            return UBJSON_OKAY;
        }
        else if (bytes_read != 0) {
            return UBJSON_PREMATURELY_ENDED;
        }
    }
}

ssize_t ubjson_get_string(ubjson_cookie_t *restrict cookie, ssize_t length, void *dest_)
{
    ssize_t total = 0;
    char *dest = dest_;
    while (total < length) {
        ssize_t read = cookie->rw.read(cookie, dest, length - total);
        if (read < 0) {
            return read;
        }
        total += read;
        dest += read;
    }
    return total;
}

ssize_t ubjson_get_i32(ubjson_cookie_t *restrict cookie, ssize_t content, int32_t *dest)
{
    static const int8_t LENGHTS[] = { 1, 1, 2, 4 };

    union {
        int8_t i8;
        uint8_t u8;
        network_uint16_t i16;
        network_uint32_t i32;
    } value;

    int type = content;

    ssize_t result = ubjson_get_string(cookie, LENGHTS[type], &value);
    if (result > 0) {
        switch (type) {
            case UBJSON_INT32_INT8:
                *dest = value.i8;
                break;
            case UBJSON_INT32_UINT8:
                *dest = value.u8;
                break;
            case UBJSON_INT32_INT16:
                *dest = (int16_t) byteorder_ntohs(value.i16);
                break;
            case UBJSON_INT32_INT32:
                *dest = (int32_t) byteorder_ntohl(value.i32);
                break;
        }
    }
    return result;
}

ssize_t ubjson_get_i64(ubjson_cookie_t *restrict cookie, ssize_t content, int64_t *dest)
{
    (void) content;

    network_uint64_t buf;
    ssize_t result = ubjson_get_string(cookie, 8, &buf);
    *dest = byteorder_ntohll(buf);
    return result;
}

static ubjson_read_callback_result_t _ubjson_read_length(ubjson_cookie_t *restrict cookie,
                                                         ssize_t *len)
{
    ubjson_type_t type;
    ssize_t content = 0;
    ubjson_read_callback_result_t result = ubjson_peek_value(cookie, &type, &content);
    if (result != UBJSON_OKAY) {
        return result;
    }

    int64_t len64;
    ssize_t read;
    if (type == UBJSON_TYPE_INT32) {
        int32_t len32;
        read = ubjson_get_i32(cookie, content, &len32);
        len64 = len32;
    }
    else if (type == UBJSON_TYPE_INT64) {
        read = ubjson_get_i64(cookie, content, &len64);
    }
    else {
        return UBJSON_INVALID_DATA;
    }

    if (read <= 0) {
        return UBJSON_PREMATURELY_ENDED;
    }
    else if ((ssize_t) len64 < 0) {
        return UBJSON_SIZE_ERROR;
    }
    else if (len64 < 0) {
        return UBJSON_INVALID_DATA;
    }

    *len = len64;
    return UBJSON_OKAY;
}

static ubjson_read_callback_result_t _ubjson_get_call(ubjson_cookie_t *restrict cookie,
                                                      char marker,
                                                      ubjson_type_t *type, ssize_t *content)
{
    *content = -1;

    switch (marker) {
        case UBJSON_MARKER_NULL:
            *type = UBJSON_TYPE_NULL;
            break;

        case UBJSON_MARKER_NOOP:
            *type = UBJSON_TYPE_NOOP;
            break;

        case UBJSON_MARKER_TRUE:
        case UBJSON_MARKER_FALSE:
            *type = UBJSON_TYPE_BOOL,
            *content = (marker == UBJSON_MARKER_TRUE);
            break;

        case UBJSON_MARKER_INT8:
            *content = UBJSON_INT32_INT8;
            *type = UBJSON_TYPE_INT32;
            break;

        case UBJSON_MARKER_UINT8:
            *content = UBJSON_INT32_UINT8;
            *type = UBJSON_TYPE_INT32;
            break;

        case UBJSON_MARKER_INT16:
            *content = UBJSON_INT32_INT16;
            *type = UBJSON_TYPE_INT32;
            break;

        case UBJSON_MARKER_INT32:
            *content = UBJSON_INT32_INT32;
            *type = UBJSON_TYPE_INT32;
            break;

        case UBJSON_MARKER_INT64:
            *type = UBJSON_TYPE_INT64;
            break;

        case UBJSON_MARKER_CHAR:
            *type = UBJSON_TYPE_STRING;
            break;

        case UBJSON_MARKER_FLOAT32:
            *type = UBJSON_TYPE_FLOAT;
            break;

        case UBJSON_MARKER_FLOAT64:
            *type = UBJSON_TYPE_DOUBLE;
            break;

        case UBJSON_MARKER_STRING: {
            ubjson_read_callback_result_t result = _ubjson_read_length(cookie, content);
            if (result != UBJSON_OKAY) {
                return result;
            }
            *type = UBJSON_TYPE_STRING;
            break;
        }

        case UBJSON_MARKER_ARRAY_START:
            *type = UBJSON_ENTER_ARRAY;
            break;

        case UBJSON_MARKER_OBJECT_START:
            *type = UBJSON_ENTER_OBJECT;
            break;

     /* NOTE: High-precision numbers are not implemented, yet. Implement if needed.
      *
      * case UBJSON_MARKER_HP_NUMBER:
      *     ...
      *     break;
      */

        case UBJSON_MARKER_ARRAY_END:
        case UBJSON_MARKER_OBJECT_END:
        case UBJSON_MARKER_COUNT:
        case UBJSON_MARKER_TYPE:
        default:
            return UBJSON_INVALID_DATA;
    }

    return UBJSON_OKAY;
}

typedef bool (*_ubjson_read_struct_continue)(ubjson_cookie_t *restrict cookie, char marker,
                                             ubjson_read_callback_result_t *result,
                                             ssize_t count, ssize_t index,
                                             ubjson_type_t *type1, ssize_t *content1);

static ubjson_read_callback_result_t _ubjson_read_struct(ubjson_cookie_t *restrict cookie,
                                                         _ubjson_read_struct_continue get_continue)
{
    ubjson_read_callback_result_t result;
    ssize_t count = -1;
    char marker, type_marker = 0;

    READ_MARKER();

    if (marker == UBJSON_MARKER_TYPE) {
        READ_MARKER();
        if (marker == 0) {
            return UBJSON_INVALID_DATA;
        }
        type_marker = marker;
        READ_MARKER();
    }

    if (marker == UBJSON_MARKER_COUNT) {
        result = _ubjson_read_length(cookie, &count);
        if (result != UBJSON_OKAY) {
            return result;
        }
        READ_MARKER();
    }

    cookie->marker = marker;

    if ((type_marker != 0) && (count < 0)) {
        /* If a type is specified, a count must be specified as well.
         * Otherwise a ']' could either be data (e.g. the character ']'),
         * or be meant to close the array.
         */
        return UBJSON_INVALID_DATA;
    }

    for (ssize_t index = 0; (count < 0) || (index < count); ++index) {
        ubjson_type_t type1;
        ssize_t content1;

        READ_MARKER();
        if (!get_continue(cookie, marker, &result, count, index, &type1, &content1)
            || (result != UBJSON_OKAY)) {
            break;
        }

        result = cookie->callback.read(cookie, type1, content1,
                                               UBJSON_ABSENT, (unsigned char) type_marker);
        if (result != UBJSON_OKAY) {
            break;
        }
    }

    return result;
}

static bool _ubjson_read_array_continue(ubjson_cookie_t *restrict cookie, char marker,
                                        ubjson_read_callback_result_t *result,
                                        ssize_t count, ssize_t index,
                                        ubjson_type_t *type1, ssize_t *content1)
{
    if (marker == UBJSON_MARKER_ARRAY_END) {
        if (count >= 0) {
            *result = UBJSON_INVALID_DATA;
        }
        return false;
    }

    cookie->marker = marker;

    *type1 = UBJSON_INDEX;
    *content1 = index;
    return true;
}

static bool _ubjson_read_object_continue(ubjson_cookie_t *restrict cookie, char marker,
                                         ubjson_read_callback_result_t *result,
                                         ssize_t count, ssize_t index,
                                         ubjson_type_t *type1, ssize_t *content1)
{
    (void) index;

    if (marker == UBJSON_MARKER_OBJECT_END) {
        if (count >= 0) {
            *result = UBJSON_INVALID_DATA;
        }
        return false;
    }

    cookie->marker = marker;

    *type1 = UBJSON_KEY;
    *result = _ubjson_read_length(cookie, content1);
    return true;
}

ubjson_read_callback_result_t ubjson_read_array(ubjson_cookie_t *restrict cookie)
{
    return _ubjson_read_struct(cookie, _ubjson_read_array_continue);
}

ubjson_read_callback_result_t ubjson_read_object(ubjson_cookie_t *restrict cookie)
{
    return _ubjson_read_struct(cookie, _ubjson_read_object_continue);
}

ubjson_read_callback_result_t ubjson_read_next(ubjson_cookie_t *restrict cookie)
{
    char marker;
    ubjson_read_callback_result_t result;
    READ_MARKER();

    ubjson_type_t type;
    ssize_t content;
    result = _ubjson_get_call(cookie, marker, &type, &content);
    if (result != UBJSON_OKAY) {
        return result;
    }

    return cookie->callback.read(cookie, type, content, UBJSON_ABSENT, 0);
}

ubjson_read_callback_result_t ubjson_peek_value(ubjson_cookie_t *restrict cookie,
                                                ubjson_type_t *type, ssize_t *content)
{
    char marker = (char) *content;
    if (marker == 0) {
        ubjson_read_callback_result_t result;
        READ_MARKER();
    }
    return _ubjson_get_call(cookie, marker, type, content);
}
