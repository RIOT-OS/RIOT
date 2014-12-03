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

#include <limits.h>

#define WRITE_CALL(FUN, ...)                                                  \
    do {                                                                      \
        ssize_t wrote = (FUN)(__VA_ARGS__);                                   \
        if (wrote < 0) {                                                      \
            return wrote;                                                     \
        }                                                                     \
        result += wrote;                                                      \
    } while (0)

#define WRITE_BUF(BUF, COUNT) \
    WRITE_CALL(cookie->rw.write, cookie, (BUF), (COUNT))

#define WRITE_MARKER(MARKER)                                                  \
    do {                                                                      \
        char marker_buf[] = { (char) (MARKER) };                              \
        WRITE_BUF(marker_buf, 1);                                             \
    } while (0)

#define MARKER_FUN(NAME, MARKER)                                              \
    ssize_t NAME(ubjson_cookie_t *restrict cookie)                            \
    {                                                                         \
        ssize_t result = 0;                                                   \
        WRITE_MARKER((MARKER));                                               \
        return result;                                                        \
    }

MARKER_FUN(ubjson_write_null, UBJSON_MARKER_NULL)
MARKER_FUN(ubjson_write_noop, UBJSON_MARKER_NOOP)

MARKER_FUN(ubjson_open_array, UBJSON_MARKER_ARRAY_START)
MARKER_FUN(ubjson_close_array, UBJSON_MARKER_ARRAY_END)

MARKER_FUN(ubjson_open_object, UBJSON_MARKER_OBJECT_START)
MARKER_FUN(ubjson_close_object, UBJSON_MARKER_OBJECT_END)

ssize_t ubjson_write_bool(ubjson_cookie_t *restrict cookie, bool value)
{
    static const char marker_false[] = { UBJSON_MARKER_FALSE };
    static const char marker_true[] = { UBJSON_MARKER_TRUE };
    return cookie->rw.write(cookie, value ? &marker_false : &marker_true, 1);
}

ssize_t ubjson_write_i32(ubjson_cookie_t *restrict cookie, int32_t value)
{
    ssize_t result = 0;
    if ((INT8_MIN <= value) && (value <= INT8_MAX)) {
        WRITE_MARKER(UBJSON_MARKER_INT8);
        WRITE_MARKER((uint8_t) value);
    }
    else if ((0 <= value) && (value <= UINT8_MAX)) {
        WRITE_MARKER(UBJSON_MARKER_UINT8);
        WRITE_MARKER((uint8_t) value);
    }
    else if ((INT16_MIN <= value) && (value <= UINT16_MAX)) {
        WRITE_MARKER(UBJSON_MARKER_INT16);
        network_uint16_t buf = byteorder_htons((uint16_t) value);
        WRITE_BUF(&buf, sizeof(buf));
    }
    else {
        WRITE_MARKER(UBJSON_MARKER_INT32);
        network_uint32_t buf = byteorder_htonl((uint32_t) value);
        WRITE_BUF(&buf, sizeof(buf));
    }
    return result;
}

ssize_t ubjson_write_i64(ubjson_cookie_t *restrict cookie, int64_t value)
{
    if ((INT32_MIN <= value) && (value <= INT32_MAX)) {
        return ubjson_write_i32(cookie, (int32_t) value);
    }

    ssize_t result = 0;
    network_uint64_t buf = byteorder_htonll((uint64_t) value);
    WRITE_BUF(&buf, sizeof(buf));
    return result;
}

ssize_t ubjson_write_float(ubjson_cookie_t *restrict cookie, float value)
{
    union {
        float f;
        uint32_t i;
    } v = { .f = value };

    ssize_t result = 0;
    WRITE_MARKER(UBJSON_MARKER_FLOAT32);
    network_uint32_t buf = byteorder_htonl(v.i);
    WRITE_BUF(&buf, sizeof(buf));
    return result;
}

ssize_t ubjson_write_double(ubjson_cookie_t *restrict cookie, double value)
{
    union {
        double f;
        uint64_t i;
    } v = { .f = value };

    ssize_t result = 0;
    WRITE_MARKER(UBJSON_MARKER_FLOAT64);
    network_uint64_t buf = byteorder_htonll(v.i);
    WRITE_BUF(&buf, sizeof(buf));
    return result;
}

ssize_t ubjson_write_string(ubjson_cookie_t *restrict cookie, const void *value, size_t len)
{
    ssize_t result = 0;
    WRITE_MARKER(UBJSON_MARKER_STRING);
    WRITE_CALL(ubjson_write_key, cookie, value, len);
    return result;
}

static ssize_t _ubjson_write_length(ubjson_cookie_t *restrict cookie, size_t len)
{
    ssize_t result = 0;
    WRITE_MARKER(UBJSON_MARKER_COUNT);
    WRITE_CALL(ubjson_write_i64, cookie, (int64_t) len);
    return result;
}

ssize_t ubjson_open_array_len(ubjson_cookie_t *restrict cookie, size_t len)
{
    ssize_t result = 0;
    WRITE_CALL(ubjson_open_array, cookie);
    if (len > 0) {
        WRITE_CALL(_ubjson_write_length, cookie, len);
    }
    else {
        WRITE_CALL(ubjson_close_array, cookie);
    }
    return result;
}

ssize_t ubjson_open_object_len(ubjson_cookie_t *restrict cookie, size_t len)
{
    ssize_t result = 0;
    WRITE_CALL(ubjson_open_object, cookie);
    if (len > 0) {
        WRITE_CALL(_ubjson_write_length, cookie, len);
    }
    else {
        WRITE_CALL(ubjson_close_object, cookie);
    }
    return result;
}

ssize_t ubjson_write_key(ubjson_cookie_t *restrict cookie, const void *value, size_t len)
{
    ssize_t result = 0;
    WRITE_CALL(ubjson_write_i64, cookie, (int64_t) len);
    WRITE_BUF(value, len);
    return result;
}
