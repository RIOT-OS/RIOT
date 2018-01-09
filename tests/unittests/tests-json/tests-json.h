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
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``json`` module
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef TESTS_JSON_H
#define TESTS_JSON_H

#include "embUnit.h"
#include "../map.h"

#include "kernel_defines.h"
#include "json.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct {
    const char *content;
    const char *name;
    const size_t size;
} json_parsing_test_t;

extern const json_parsing_test_t tests_json_parsing_tests[];
extern const size_t tests_json_parsing_tests_count;

void tests_json_parsing(void);
void tests_json(void);

typedef struct {
    json_read_cookie_t cookie;
    const char *data;
    size_t pos, length;
} tests_json_read_cookie_t;

static inline tests_json_read_cookie_t *_get_cookie(json_read_cookie_t *c)
{
    return container_of(c, tests_json_read_cookie_t, cookie);
}

void tests_json_read_digit(json_read_cookie_t *cookie, int digit, bool *okay);

#define READ_DIGIT(DIGIT)                                                     \
    do {                                                                      \
        bool __okay = false;                                                  \
        tests_json_read_digit(&c.cookie, (DIGIT), &__okay);                   \
        if (!__okay) {                                                        \
            return;                                                           \
        }                                                                     \
    } while (0)

void tests_json_setup_cookie(tests_json_read_cookie_t *c, const char *data, size_t size);

typedef struct {
    json_write_cookie_t cookie;
    const char *expected;
    size_t pos, length;
} tests_json_write_cookie_t;

static inline tests_json_write_cookie_t *_get_write_cookie(json_write_cookie_t *c)
{
    return container_of(c, tests_json_write_cookie_t, cookie);
}

void tests_json_setup_write_cookie(tests_json_write_cookie_t *c, const char *expected, size_t len);

static inline bool tests_json_write_cookie_is_end(const tests_json_write_cookie_t *c)
{
    return c->pos == c->length;
}

#define TESTS_JSON_UTILITIES                                                  \
        utilities_number_type,                                                \
        utilities_codepoint_to_utf8_1_min,                                    \
        utilities_codepoint_to_utf8_1_avg,                                    \
        utilities_codepoint_to_utf8_1_max,                                    \
        utilities_codepoint_to_utf8_2_min,                                    \
        utilities_codepoint_to_utf8_2_avg,                                    \
        utilities_codepoint_to_utf8_2_max,                                    \
        utilities_codepoint_to_utf8_3_min,                                    \
        utilities_codepoint_to_utf8_3_avg,                                    \
        utilities_codepoint_to_utf8_3_max,                                    \
        utilities_codepoint_to_utf8_4_min,                                    \
        utilities_codepoint_to_utf8_4_avg,                                    \
        utilities_codepoint_to_utf8_4_max,                                    \
        utilities_codepoint_to_utf8_invalid_min,                              \
        utilities_codepoint_to_utf8_invalid_max

#define TESTS_JSON_READ_NUMBER                                                \
        read_number_invalid_prefix,                                           \
        read_number_invalid_format,                                           \
        read_number_zero,                                                     \
        read_number_integer,                                                  \
        read_number_float

#define TESTS_JSON_READ_STRING                                                \
        read_empty_string,                                                    \
        read_ascii_string,                                                    \
        read_escaped_ascii_string,                                            \
        read_unicode_string1,                                                 \
        read_unicode_string2,                                                 \
        read_string_bits

#define TESTS_JSON_READ_OBJECT                                                \
        read_empty_object,                                                    \
        read_object

#define TESTS_JSON_READ_ARRAY                                                 \
        read_empty_array,                                                     \
        read_array,                                                           \
        read_nested_array

#define TESTS_JSON_READ_LITERAL                                               \
        read_true,                                                            \
        read_false,                                                           \
        read_null,                                                            \
        read_illegal_literal

#define TESTS_JSON_READ_ALL                                                   \
        TESTS_JSON_READ_NUMBER,                                               \
        TESTS_JSON_READ_STRING,                                               \
        TESTS_JSON_READ_OBJECT,                                               \
        TESTS_JSON_READ_ARRAY,                                                \
        TESTS_JSON_READ_LITERAL

#define TESTS_JSON_WRITE_STRING                                               \
        write_empty_string,                                                   \
        write_empty_string_bits

#ifndef MODULE_ATMEGA_COMMON
#   define IFNOT_ATMEGA(X) X,
#else
#   define IFNOT_ATMEGA(X)
#endif

#define TESTS_JSON_WRITE_NUMBER                                               \
        write_zero,                                                           \
        write_pos_int,                                                        \
        write_neg_int,                                                        \
        IFNOT_ATMEGA(write_zero64)                                            \
        IFNOT_ATMEGA(write_pos_int64)                                         \
        IFNOT_ATMEGA(write_neg_int64)                                         \
        write_zero_float,                                                     \
        write_pos_float,                                                      \
        write_neg_float

#define TESTS_JSON_WRITE_LITERAL                                              \
        write_true,                                                           \
        write_false,                                                          \
        write_null

#define TESTS_JSON_WRITE_ARRAY                                                \
         write_array_empty,                                                   \
         write_array_singleton,                                               \
         write_array,                                                         \
         write_array_nested

#define TESTS_JSON_WRITE_OBJECT                                               \
         write_object_empty,                                                  \
         write_object_singleton,                                              \
         write_object,                                                        \
         write_object_nested

#define TESTS_JSON_WRITE_ALL                                                  \
        TESTS_JSON_WRITE_STRING,                                              \
        TESTS_JSON_WRITE_NUMBER,                                              \
        TESTS_JSON_WRITE_LITERAL,                                             \
        TESTS_JSON_WRITE_ARRAY,                                               \
        TESTS_JSON_WRITE_OBJECT

#define TESTS_JSON_ALL                                                        \
        TESTS_JSON_UTILITIES,                                                 \
        TESTS_JSON_READ_ALL,                                                  \
        TESTS_JSON_WRITE_ALL

#define TESTS_JSON_PROTOTYPE(NAME) void tests_json_##NAME(void);
MAP(TESTS_JSON_PROTOTYPE, TESTS_JSON_ALL)

#ifdef __cplusplus
}
#endif

#endif /* TESTS_JSON_H */
/** @} */
