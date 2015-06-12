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

#include "tests-json.h"

#include <string.h>
#include <stdbool.h>

struct number_test {
    json_result_t expected_outcome;
    json_number_t expected_type;
    const char *const *strs;
};

static const char *const number_test_prematurely_ended[] = {
    "", NULL,
};
static const char *const number_test_invalid[] = {
    "+0", "01", "01.2",
    "1.2.3", ".1", "1.2e1.1", "e1",
    " 1", "1 ",
    NULL,
};
static const char *const number_test_zero[] = {
    "0", "-0", NULL,
};
static const char *const number_test_integer[] = {
    "1", "-1", "1234", "-1234",
    "12345678901234567890123456789012345678901234567890",
    NULL,
};
static const char *const number_test_float[] = {
    "0.0", "-0.0", "0e0",
    "0.1", "1234.0",

    "1234e1", "1234e01", "1234e+1", "1234e+01", "1234e-1", "1234e-01",
    "1234E1", "1234E01", "1234E+1", "1234E+01", "1234E-1", "1234E-01",

    "1234.5e1", "1234.5e01", "1234.5e+1", "1234.5e+01", "1234.5e-1", "1234.5e-01",
    "1234.5E1", "1234.5E01", "1234.5E+1", "1234.5E+01", "1234.5E-1", "1234.5E-01",

    NULL,
};

static const struct number_test number_tests[] = {
    { JSON_PREMATURELY_ENDED, -1,                  number_test_prematurely_ended },
    { JSON_INVALID_DATA,      -1,                  number_test_invalid },
    { JSON_OKAY,              JSON_NUMBER_ZERO,    number_test_zero },
    { JSON_OKAY,              JSON_NUMBER_INTEGER, number_test_integer },
    { JSON_OKAY,              JSON_NUMBER_FLOAT,   number_test_float },
    { 0,                      0,                   NULL },
};

void tests_json_utilities_number_type(void)
{
    for (const struct number_test *number_test = number_tests; number_test->strs; ++number_test) {
        for (const char *const *str = number_test->strs; *str; ++str) {
            json_number_t actual_type;
            json_result_t actual_outcome = json_number_type(*str, strlen(*str), &actual_type);
            TEST_ASSERT_EQUAL_INT(number_test->expected_outcome, actual_outcome);
            if (number_test->expected_outcome == JSON_OKAY) {
                TEST_ASSERT_EQUAL_INT(number_test->expected_type, actual_type);
            }
        }
    }
}

static void _test_codepoint(uint32_t cp, const unsigned char *expected_chars, size_t expected_len)
{
    char actual_chars[4];
    size_t actual_len;
    json_result_t expected_outcome = (expected_len == 0) ? JSON_INVALID_DATA : JSON_OKAY;
    TEST_ASSERT_EQUAL_INT(expected_outcome,
                          json_codepoint_to_utf8(cp, actual_chars, &actual_len));
    if (expected_chars != NULL) {
        TEST_ASSERT_EQUAL_INT(expected_len, actual_len);
        TEST_ASSERT_EQUAL_INT(0, memcmp(expected_chars, actual_chars, actual_len));
    }
}

#define TEST_CODEPOINT(LEN, NAME, CP, ...)                                    \
    void tests_json_utilities_codepoint_to_utf8_##LEN##_##NAME(void)          \
    {                                                                         \
        static const unsigned char expected_chars[(LEN)] = { __VA_ARGS__ };   \
        _test_codepoint((CP), expected_chars, LEN);                           \
    }

TEST_CODEPOINT(1, min, 0x00,       0x00)
TEST_CODEPOINT(1, avg, 0x61,       0x61)
TEST_CODEPOINT(1, max, 0x7F,       0x7F)

TEST_CODEPOINT(2, min, 0x080,      0xC2, 0x80)
TEST_CODEPOINT(2, avg, 0x35F,      0xCD, 0x9F)
TEST_CODEPOINT(2, max, 0x7FF,      0xDF, 0xBF)

TEST_CODEPOINT(3, min, 0x0800,     0xE0, 0xA0, 0x80)
TEST_CODEPOINT(3, avg, 0x20AC,     0xE2, 0x82, 0xAC)
TEST_CODEPOINT(3, max, 0xFFFF,     0xEF, 0xBF, 0xBF)

TEST_CODEPOINT(4, min, 0x010000,   0xF0, 0x90, 0x80, 0x80)
TEST_CODEPOINT(4, avg, 0x02A6B2,   0xF0, 0xAA, 0x9A, 0xB2)
TEST_CODEPOINT(4, max, 0x10FFFF,   0xF4, 0x8F, 0xBF, 0xBF)

void tests_json_utilities_codepoint_to_utf8_invalid_min(void)
{
    _test_codepoint(0x110000, NULL, 0);
}

void tests_json_utilities_codepoint_to_utf8_invalid_max(void)
{
    _test_codepoint(0xFFFFFFFF, NULL, 0);
}
