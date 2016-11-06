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

#include <string.h>

#define ADVANCE(RESULT)                                                       \
    do {                                                                      \
        ++str;                                                                \
        --len;                                                                \
        if (len == 0) {                                                       \
            return (RESULT);                                                  \
        }                                                                     \
    } while (0)

json_result_t json_number_type(const char *str, size_t len, json_number_t *type)
{
    /* Format: (0|[1-9][0-9]*)([.][0-9]+)?([eE][+-]?[0-9]+)?
     */

    if (len == 0) {
        return JSON_PREMATURELY_ENDED;
    }
    else if (*str == '-') {
        ADVANCE(JSON_INVALID_DATA);
    }

    if (*str == '0') {
        *type = JSON_NUMBER_ZERO;
        ADVANCE(JSON_OKAY);
    }
    else if (('1' <= *str) && (*str <= '9')) {
        *type = JSON_NUMBER_INTEGER;
        while (1) {
            ADVANCE(JSON_OKAY);
            if (!(('0' <= *str) && (*str <= '9'))) {
                break;
            }
        }
    }
    else {
        return JSON_INVALID_DATA;
    }

    *type = JSON_NUMBER_FLOAT;

    if (*str == '.') {
        ADVANCE(JSON_INVALID_DATA);

        if (!(('0' <= *str) && (*str <= '9'))) {
            return JSON_INVALID_DATA;
        }
        while (1) {
            ADVANCE(JSON_OKAY);
            if (!(('0' <= *str) && (*str <= '9'))) {
                break;
            }
        }
    }

    if ((*str == 'E') || (*str == 'e')) {
        ADVANCE(JSON_INVALID_DATA);
        if ((*str == '+') || (*str == '-')) {
            ADVANCE(JSON_INVALID_DATA);
        }

        if (!(('0' <= *str) && (*str <= '9'))) {
            return JSON_INVALID_DATA;
        }
        while (1) {
            ADVANCE(JSON_OKAY);
            if (!(('0' <= *str) && (*str <= '9'))) {
                break;
            }
        }
    }

    return JSON_INVALID_DATA;
}

json_result_t json_codepoint_to_utf8(uint32_t codepoint, char *output, size_t *len)
{
    if (codepoint < 0x80) {
        *len = 1;
        *output = (uint8_t) codepoint;
        return JSON_OKAY;
    }

    unsigned len_;
    if (codepoint <= 0x7FF) {
        len_ = 2;
    }
    else if (codepoint <= 0xFFFF) {
        len_ = 3;
    }
    else if (codepoint <= 0x10FFFF) {
        len_ = 4;
    }
    else {
        return JSON_INVALID_DATA;
    }
    *len = len_;

    char c = ((1 << len_) - 1) << (8 - len_);
    --len_;
    *output = c | (codepoint >> (6 * len_));
    while (1) {
        --len_;
        ++output;
        *output = 0x80 | ((codepoint >> (6 * len_)) & 0x3f);

        if (len_ == 0) {
            break;
        }
    }
    return JSON_OKAY;
}

int64_t json_number_to_int(const char *str, size_t len)
{
    if (len == 0) {
        len = strlen(str);
    }

    bool neg = false;
    if (*str == '-') {
        neg = true;
        ++str;
        --len;
    }

    uint64_t result = 0;
    while (len > 0) {
        result *= 10;
        result += *str - '0';
        --len, ++str;
    }

    return (int64_t) (neg ? ~result + 1 : result);
}

float json_number_to_float(const char *str, size_t len)
{
    if (len == 0) {
        len = strlen(str);
    }

    bool neg = false;
    if (*str == '-') {
        neg = true;
        ++str;
        --len;
    }

    float result = 0.0;
    while ((len > 0) && (*str != '.')) {
        result *= 10.f;
        result += *str - '0';
        --len, ++str;
    }

    if ((len > 0) && (*str == 0)) {
        float factor = 1.f;
        while ((len > 0) && (*str != '.')) {
            factor /= 10.f;
            result += (*str - '0') * factor;
            --len, ++str;
        }
    }

    return neg ? -result : result;
}
