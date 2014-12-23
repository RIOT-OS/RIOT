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

#define TEST_NUMBER(STR, EXPECTED_OUTCOME, EXPECTED_TYPE)                     \
    do {                                                                      \
        static const char str[sizeof(STR) - 1] = STR;                         \
        json_number_t type;                                                   \
        TEST_ASSERT_EQUAL_INT(EXPECTED_OUTCOME,                               \
                              json_number_type(str, sizeof(str), &type));     \
        if ((EXPECTED_OUTCOME) == JSON_OKAY) {                                \
            TEST_ASSERT_EQUAL_INT(EXPECTED_TYPE, type);                       \
        }                                                                     \
    } while (0)

void tests_json_utilities_number_type(void)
{
    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_number_type(NULL, 0, NULL));

    TEST_NUMBER("+0",      JSON_INVALID_DATA, -1);
    TEST_NUMBER("01",      JSON_INVALID_DATA, -1);
    TEST_NUMBER("01.2",    JSON_INVALID_DATA, -1);
    TEST_NUMBER("1.2.3",   JSON_INVALID_DATA, -1);
    TEST_NUMBER(".1",      JSON_INVALID_DATA, -1);
    TEST_NUMBER("1.2e1.1", JSON_INVALID_DATA, -1);
    TEST_NUMBER("e1",      JSON_INVALID_DATA, -1);
    TEST_NUMBER(" 1",      JSON_INVALID_DATA, -1);
    TEST_NUMBER("1 ",      JSON_INVALID_DATA, -1);

    TEST_NUMBER("0",  JSON_OKAY, JSON_NUMBER_ZERO);
    TEST_NUMBER("-0", JSON_OKAY, JSON_NUMBER_ZERO);

    TEST_NUMBER("1",     JSON_OKAY, JSON_NUMBER_INTEGER);
    TEST_NUMBER("-1",    JSON_OKAY, JSON_NUMBER_INTEGER);
    TEST_NUMBER("1234",  JSON_OKAY, JSON_NUMBER_INTEGER);
    TEST_NUMBER("-1234", JSON_OKAY, JSON_NUMBER_INTEGER);
    TEST_NUMBER("12345678901234567890123456789012345678901234567890",
                         JSON_OKAY, JSON_NUMBER_INTEGER);

    TEST_NUMBER("0.0",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("-0.0", JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("0e0",  JSON_OKAY, JSON_NUMBER_FLOAT);

    TEST_NUMBER("0.1",    JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.0", JSON_OKAY, JSON_NUMBER_FLOAT);

    TEST_NUMBER("1234e1",   JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234e01",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234e+1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234e+01", JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234e-1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234e-01", JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234E1",   JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234E01",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234E+1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234E+01", JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234E-1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234E-01", JSON_OKAY, JSON_NUMBER_FLOAT);

    TEST_NUMBER("1234.5e1",   JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5e01",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5e+1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5e+01", JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5e-1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5e-01", JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5E1",   JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5E01",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5E+1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5E+01", JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5E-1",  JSON_OKAY, JSON_NUMBER_FLOAT);
    TEST_NUMBER("1234.5E-01", JSON_OKAY, JSON_NUMBER_FLOAT);
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
