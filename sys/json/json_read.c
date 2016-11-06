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

#include "json.h"

#define _DO_OR_FAIL(FUN, ...)                                                 \
    do {                                                                      \
        result = _json_read_##FUN(__VA_ARGS__);                               \
        if (result != JSON_OKAY) {                                            \
            return result;                                                    \
        }                                                                     \
    } while (0)

static json_result_t _json_read_consume_filled(json_read_cookie_t *cookie, char c)
{
    if (cookie->pushback == c) {
        cookie->pushback = -1;
        return JSON_OKAY;
    }
    return JSON_INVALID_DATA;
}

static json_result_t _json_read_fill_pushback(json_read_cookie_t *cookie)
{
    if (cookie->pushback >= 0) {
        return JSON_OKAY;
    }

    while (1) {
        int c = cookie->read(cookie);
        if (c < 0) {
            return JSON_PREMATURELY_ENDED;
        }

        cookie->pushback = (unsigned char) c;
        return JSON_OKAY;
    }
}

static json_result_t _json_read_advance_to_data(json_read_cookie_t *cookie)
{
    while (1) {
        json_result_t result;
        _DO_OR_FAIL(fill_pushback, cookie);

        char c = cookie->pushback;
        if ((c == '\t') || (c == '\n') || (c == '\r') || (c == ' ')) {
            cookie->pushback = -1;
            continue;
        }

        return JSON_OKAY;
    }
}

static json_result_t _json_read_consume(json_read_cookie_t *cookie, char c)
{
    json_result_t result;
    _DO_OR_FAIL(advance_to_data, cookie);
    return _json_read_consume_filled(cookie, c);
}

static json_result_t _json_read_consume_next(json_read_cookie_t *cookie, char c)
{
    json_result_t result;
    _DO_OR_FAIL(fill_pushback, cookie);
    return _json_read_consume_filled(cookie, c);
}

json_result_t json_read_peek(json_read_cookie_t *cookie, json_type_t *type)
{
    json_result_t result;
    _DO_OR_FAIL(advance_to_data, cookie);

    switch (cookie->pushback) {
        case '{':
            *type = JSON_OBJECT;
            return JSON_OKAY;

        case '[':
            *type = JSON_ARRAY;
            return JSON_OKAY;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '-':
            *type = JSON_NUMBER;
            return JSON_OKAY;

        case '"':
            *type = JSON_STRING;
            return JSON_OKAY;

        case 't':
            *type = JSON_TRUE;
            return JSON_OKAY;

        case 'f':
            *type = JSON_FALSE;
            return JSON_OKAY;

        case 'n':
            *type = JSON_NULL;
            return JSON_OKAY;

        default:
            return JSON_INVALID_DATA;
    }
}

json_result_t json_read_object(json_read_cookie_t *cookie)
{
    json_result_t result;
    _DO_OR_FAIL(consume, cookie, '{');

    cookie->state = JSON_READ_STATE_IN_OBJECT;
    return JSON_OKAY;
}

json_result_t json_read_array(json_read_cookie_t *cookie)
{
    json_result_t result;
    _DO_OR_FAIL(consume, cookie, '[');

    cookie->state = JSON_READ_STATE_IN_ARRAY;
    return JSON_OKAY;
}

json_result_t json_read_array_next(json_read_cookie_t *cookie, bool *array_closed)
{
    if (cookie->state == JSON_READ_STATE_IN_ARRAY) {
        if (_json_read_consume(cookie, ']') == JSON_OKAY) {
            *array_closed = true;
        }
        else {
            *array_closed = false;
        }
        cookie->state = JSON_READ_STATE_SOMEWHERE;
    }
    else {
        if (_json_read_consume(cookie, ',') == JSON_OKAY) {
            *array_closed = false;
        }
        else if (_json_read_consume(cookie, ']') == JSON_OKAY) {
            *array_closed = true;
        }
        else {
            return JSON_INVALID_DATA;
        }
    }
    return JSON_OKAY;
}

static json_result_t _json_read_in_unicode_ucs2(json_read_cookie_t *cookie,
                                                uint16_t *value)
{
    *value = 0;
    for (int i = 0; i < 4; ++i) {
        json_result_t result;
        _DO_OR_FAIL(fill_pushback, cookie);

        uint8_t c = (uint8_t) cookie->pushback;
        cookie->pushback = -1;

        uint8_t offset;
        if (('0' <= c) && (c <= '9')) {
            offset = '0' - 0x0;
        }
        else if (('A' <= c) && (c <= 'F')) {
            offset = 'A' - 0xA;
        }
        else if (('a' <= c) && (c <= 'f')) {
            offset = 'a' - 0xa;
        }
        else {
            return JSON_INVALID_DATA;
        }

        *value = (*value << 4) | (c - offset);
    }
    return JSON_OKAY;
}

static json_result_t _json_read_in_unicode(json_read_cookie_t *cookie)
{
    json_result_t result;

    uint16_t value1;
    _DO_OR_FAIL(in_unicode_ucs2, cookie, &value1);
    if (!((0xD800 <= value1) && (value1 <= 0xDBFF))) {
        cookie->pushback = value1;
        return JSON_OKAY;
    }

    _DO_OR_FAIL(consume_next, cookie, '\\');
    _DO_OR_FAIL(consume_next, cookie, 'u');

    uint16_t value2;
    _DO_OR_FAIL(in_unicode_ucs2, cookie, &value2);
    if (!((0xDC00 <= value2) && (value2 <= 0xDFFF))) {
        return JSON_INVALID_DATA;
    }

    cookie->pushback = (int32_t) (((uint32_t) value1 << 10) + value2 - 0x35FDC00ul);
    return JSON_OKAY;
}

static json_result_t _json_read_in_string(json_read_cookie_t *cookie,
                                          char *buffer, size_t len, size_t *total,
                                          bool *done)
{
    *done = false;

    while (len > 0) {
        char c;
        json_result_t consume_result = _json_read_consume_next(cookie, '"');
        if (consume_result == JSON_PREMATURELY_ENDED) {
            /* truncated string */
            return JSON_PREMATURELY_ENDED;
        }
        else if (consume_result == JSON_OKAY) {
            *done = true;
            return JSON_OKAY;
        }
        else if (_json_read_consume_next(cookie, '\\') == JSON_OKAY) {
            json_result_t result;

            _DO_OR_FAIL(fill_pushback, cookie);
            switch (cookie->pushback) {
                case '"':
                case '\\':
                case '/':
                    c = cookie->pushback;
                    break;

                case 'b':
                    c = '\b';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;

                case 'u':
                    cookie->pushback = -1;
                    _DO_OR_FAIL(in_unicode, cookie);
                    if ((cookie->pushback >= 0) && (cookie->pushback < 128)) {
                        c = cookie->pushback;
                    }
                    else {
                        return JSON_READ_UNICODE;
                    }
                    break;

                default:
                    return JSON_INVALID_DATA;
            }
        }
        else if (cookie->pushback < ' ') {
            /* Control characters (U+0000 through U+001F) are forbidden per
             * http://rfc7159.net/rfc7159#rfc.section.7
             */
            return JSON_INVALID_DATA;
        }
        else {
            c = cookie->pushback;
        }

        cookie->pushback = -1;
        *buffer = c;

        ++buffer;
        --len;
        ++*total;
    }

    return JSON_OKAY;
}

json_result_t json_read_object_next(json_read_cookie_t *cookie,
                                    bool *object_closed,
                                    char *key_buffer, size_t key_len_in,
                                    size_t *key_len_out, bool *key_read)
{
    json_result_t result;

    if (cookie->state != JSON_READ_STATE_IN_SEQUENCE) {
        result = _json_read_consume(cookie, '}');
        if (result == JSON_OKAY) {
            *object_closed = true;
            cookie->state = JSON_READ_STATE_SOMEWHERE;
            return JSON_OKAY;
        }
        else if (result == JSON_INVALID_DATA) {
            *object_closed = false;
        }
        else {
            return result;
        }

        if (cookie->state != JSON_READ_STATE_IN_OBJECT) {
            _DO_OR_FAIL(consume_filled, cookie, ',');
        }

        _DO_OR_FAIL(consume, cookie, '"');
        cookie->state = JSON_READ_STATE_IN_SEQUENCE;
    }

    _DO_OR_FAIL(in_string, cookie, key_buffer, key_len_in, key_len_out, key_read);
    if (*key_read) {
        _DO_OR_FAIL(consume, cookie, ':');
        cookie->state = JSON_READ_STATE_SOMEWHERE;
    }

    return JSON_OKAY;
}

json_result_t json_read_string(json_read_cookie_t *cookie,
                               char *buffer, size_t len_in,
                               size_t *len_out, bool *done)
{
    json_result_t result;

    if (cookie->state != JSON_READ_STATE_IN_SEQUENCE) {
        _DO_OR_FAIL(consume, cookie, '"');
        cookie->state = JSON_READ_STATE_IN_SEQUENCE;
    }

    _DO_OR_FAIL(in_string, cookie, buffer, len_in, len_out, done);
    if (*done) {
        cookie->state = JSON_READ_STATE_SOMEWHERE;
    }

    return JSON_OKAY;
}

json_result_t json_read_number(json_read_cookie_t *cookie,
                               char *buffer, size_t len_in,
                               size_t *len_out, bool *done)
{
    if (cookie->state != JSON_READ_STATE_IN_SEQUENCE) {
        json_result_t result;
        _DO_OR_FAIL(advance_to_data, cookie);
        char c = cookie->pushback;
        if ((c == '-') || (('0' <= c) && (c <= '9'))) {
            cookie->state = JSON_READ_STATE_IN_SEQUENCE;
        }
        else {
            return JSON_INVALID_DATA;
        }
    }

    while (1) {
        json_result_t result = _json_read_fill_pushback(cookie);
        char c = cookie->pushback;

        if (result != JSON_OKAY) {
            break;
        }
        else if (len_in-- == 0) {
            *done = false;
            return JSON_OKAY;
        }
        else if (('0' <= c && c <= '9') ||
                 (c == '-') || (c == '+') || (c == '.') || ((c | 0x20) == 'e')) {
            *buffer++ = cookie->pushback;
            cookie->pushback = -1;
            ++*len_out;
        }
        else {
            break;
        }
    }

    cookie->state = JSON_READ_STATE_SOMEWHERE;
    *done = true;
    return JSON_OKAY;
}

static json_result_t _json_read_token(json_read_cookie_t *cookie, const char *str)
{
    for (; *str; ++str) {
        json_result_t result;
        _DO_OR_FAIL(consume_next, cookie, *str);
    }
    return JSON_OKAY;
}

json_result_t json_read_true(json_read_cookie_t *cookie)
{
    return _json_read_token(cookie, "true");
}

json_result_t json_read_false(json_read_cookie_t *cookie)
{
    return _json_read_token(cookie, "false");
}

json_result_t json_read_null(json_read_cookie_t *cookie)
{
    return _json_read_token(cookie, "null");
}

json_result_t json_read_read_spaces(json_read_cookie_t *cookie) {
    json_result_t result = _json_read_advance_to_data(cookie);
    if (result == JSON_OKAY) {
        return JSON_INVALID_DATA;
    } else {
        return result; /* i.e. JSON_PREMATURELY_ENDED */
    }
}
