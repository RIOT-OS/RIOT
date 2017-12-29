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

#include "json.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>

static inline bool _json_write_buf(json_write_cookie_t *cookie,
                                   const char *str, size_t len)
{
    while (len > 0) {
        ssize_t wrote = cookie->write(cookie, str, len);
        if (wrote < 0) {
            return false;
        }
        len -= (size_t) wrote;
        str += (size_t) wrote;
    }
    return true;
}

#define _WRITE_BUF(STR, LEN)                                                  \
    do {                                                                      \
        if (!_json_write_buf(cookie, STR, LEN)) {                             \
            return JSON_PREMATURELY_ENDED;                                    \
        }                                                                     \
    } while (0)

#define _WRITE_STR(STR)                                                       \
    do {                                                                      \
        static const char __str[sizeof(STR) - 1] = STR;                       \
        _WRITE_BUF(__str, sizeof(__str));                                     \
    } while (0)

json_result_t json_write_object_open(json_write_cookie_t *cookie)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_KEY:
            _WRITE_STR("\":{");
            break;

        case JSON_WRITE_STATE_SOMEWHERE:
            _WRITE_STR("{");
            break;

        case JSON_WRITE_STATE_IN_STRING:
        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_IN_ARRAY:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
    cookie->state = JSON_WRITE_STATE_IN_OBJECT;
    return JSON_OKAY;
}

json_result_t json_write_object_close(json_write_cookie_t *cookie)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_STRING:
            _WRITE_STR("\"}");
            break;

        case JSON_WRITE_STATE_SOMEWHERE:
        case JSON_WRITE_STATE_IN_OBJECT:
            _WRITE_STR("}");
            break;

        case JSON_WRITE_STATE_IN_ARRAY:
        case JSON_WRITE_STATE_IN_KEY:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
    cookie->state = JSON_WRITE_STATE_SOMEWHERE;
    return JSON_OKAY;
}

static json_result_t _json_write_in_string(json_write_cookie_t *cookie,
                                           const char *string, size_t len)
{
    while (len > 0) {
        size_t bare_characters_len = 0;
        while (bare_characters_len < len) {
            unsigned char c = string[bare_characters_len];
            if ((c == '"') || (c == '\\') || (c < ' ')) {
                break;
            }
            else {
                ++bare_characters_len;
            }
        }

        if (bare_characters_len > 0) {
            ssize_t wrote = cookie->write(cookie, string, bare_characters_len);
            if (wrote < 0) {
                return JSON_PREMATURELY_ENDED;
            }

            string += wrote;
            len -= (size_t) wrote;
            if (len == 0) {
                return JSON_OKAY;
            }
        }

        unsigned char c = (unsigned char) *string;
        ++string;

        static const char HEX[16] = "0123456789ABCDEF";
        char escaped[6] = "\\u00xy";
        escaped[4] = HEX[c / 16];
        escaped[5] = HEX[c % 16];
        _WRITE_BUF(escaped, sizeof(escaped));
    }
    return JSON_OKAY;
}

json_result_t json_write_object_key(json_write_cookie_t *cookie,
                                    const char *string, size_t len)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_STRING:
            _WRITE_STR("\",\"");
            break;

        case JSON_WRITE_STATE_SOMEWHERE:
            _WRITE_STR(",\"");
            break;

        case JSON_WRITE_STATE_IN_OBJECT:
            _WRITE_STR("\"");
            break;

        case JSON_WRITE_STATE_IN_KEY:
            break;

        case JSON_WRITE_STATE_IN_ARRAY:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
    cookie->state = JSON_WRITE_STATE_IN_KEY;
    return _json_write_in_string(cookie, string, len);
}

json_result_t json_write_array_open(json_write_cookie_t *cookie)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_KEY:
            _WRITE_STR("\":[");
            break;

        case JSON_WRITE_STATE_IN_ARRAY:
        case JSON_WRITE_STATE_SOMEWHERE:
            _WRITE_STR("[");
            break;

        case JSON_WRITE_STATE_IN_STRING:
        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
    cookie->state = JSON_WRITE_STATE_IN_ARRAY;
    return JSON_OKAY;
}

json_result_t json_write_array_close(json_write_cookie_t *cookie)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_STRING:
            _WRITE_STR("\"]");
            cookie->state = JSON_WRITE_STATE_SOMEWHERE;
            return JSON_OKAY;

        case JSON_WRITE_STATE_IN_ARRAY:
            cookie->state = JSON_WRITE_STATE_SOMEWHERE;
            /* fall through */
        case JSON_WRITE_STATE_SOMEWHERE:
            _WRITE_STR("]");
            return JSON_OKAY;

        case JSON_WRITE_STATE_IN_KEY:
        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
}

json_result_t json_write_array_next(json_write_cookie_t *cookie)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_STRING:
            _WRITE_STR("\",");
            /* fall through */
        case JSON_WRITE_STATE_IN_ARRAY:
            cookie->state = JSON_WRITE_STATE_SOMEWHERE;
            return JSON_OKAY;

        case JSON_WRITE_STATE_SOMEWHERE:
            _WRITE_STR(",");
            return JSON_OKAY;

        case JSON_WRITE_STATE_IN_KEY:
        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
}

static json_result_t _json_write_format(json_write_cookie_t *cookie,
                                        size_t size, const char *format, ...)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_ARRAY:
        case JSON_WRITE_STATE_SOMEWHERE:
            break;

        case JSON_WRITE_STATE_IN_KEY:
            _WRITE_STR("\":");
            break;

        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_IN_STRING:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }

    cookie->state = JSON_WRITE_STATE_SOMEWHERE;

    va_list ap;
    va_start(ap, format);
    char buffer[size];
    int len = vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    if (len < 0) {
        return JSON_INVALID_DATA;
    }
    _WRITE_BUF(buffer, (size_t) len);
    return JSON_OKAY;
}

json_result_t json_write_int(json_write_cookie_t *cookie, int32_t value)
{
    return _json_write_format(cookie, sizeof("-2147483648"), "%" PRId32, value);
}

#ifndef MODULE_ATMEGA_COMMON
json_result_t json_write_int64(json_write_cookie_t *cookie, int64_t value)
{
    return _json_write_format(cookie, sizeof("-9223372036854775808"), "%" PRId64, value);
}
#endif

json_result_t json_write_float(json_write_cookie_t *cookie, float value)
{
    return _json_write_format(cookie, sizeof("-1.797693135e+308"), "%.9e", value);
}

json_result_t json_write_string(json_write_cookie_t *cookie,
                                const char *string, size_t len)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_KEY:
            _WRITE_STR("\":\"");
            break;

        case JSON_WRITE_STATE_SOMEWHERE:
        case JSON_WRITE_STATE_IN_ARRAY:
            _WRITE_STR("\"");
            break;

        case JSON_WRITE_STATE_IN_STRING:
            break;

        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
    cookie->state = JSON_WRITE_STATE_IN_STRING;
    return _json_write_in_string(cookie, string, len);
}

static json_result_t _json_write_token(json_write_cookie_t *cookie,
                                       const char *str, size_t len)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_KEY:
            _WRITE_STR("\":");
            /* fall through */
        case JSON_WRITE_STATE_IN_ARRAY:
            cookie->state = JSON_WRITE_STATE_SOMEWHERE;
            /* fall through */
        case JSON_WRITE_STATE_SOMEWHERE:
            _WRITE_BUF(str, len);
            return JSON_OKAY;

        case JSON_WRITE_STATE_IN_STRING:
        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_DONE:
        default:
            return JSON_INVALID_DATA;
    }
}

json_result_t json_write_true(json_write_cookie_t *cookie)
{
    return _json_write_token(cookie, "true", sizeof("true") - 1);
}

json_result_t json_write_false(json_write_cookie_t *cookie)
{
    return _json_write_token(cookie, "false", sizeof("false") - 1);
}

json_result_t json_write_null(json_write_cookie_t *cookie)
{
    return _json_write_token(cookie, "null", sizeof("null") - 1);
}

json_result_t json_write_finish(json_write_cookie_t *cookie)
{
    switch (cookie->state) {
        case JSON_WRITE_STATE_IN_STRING:
            _WRITE_STR("\"");
            /* fall through */
        case JSON_WRITE_STATE_SOMEWHERE:
            cookie->state = JSON_WRITE_STATE_DONE;
            /* fall through */
        case JSON_WRITE_STATE_DONE:
            return JSON_OKAY;

        case JSON_WRITE_STATE_IN_ARRAY:
        case JSON_WRITE_STATE_IN_OBJECT:
        case JSON_WRITE_STATE_IN_KEY:
        default:
            return JSON_INVALID_DATA;
    }
}
