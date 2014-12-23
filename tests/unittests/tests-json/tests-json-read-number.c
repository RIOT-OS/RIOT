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
#include <stdio.h>

#define TEST_NUMBER_INVALID_PREFIX(STR)                                       \
    do {                                                                      \
        static const char str[sizeof(STR) - 1] = STR;                         \
        tests_json_read_cookie_t c;                                           \
        tests_json_setup_cookie(&c, str, sizeof(str));                        \
                                                                              \
        json_type_t type;                                                     \
        TEST_ASSERT_EQUAL_INT(JSON_INVALID_DATA,                              \
                              json_read_peek(&c.cookie, &type));              \
    } while (0)

void tests_json_read_number_invalid_prefix(void)
{
    TEST_NUMBER_INVALID_PREFIX("+0");
    TEST_NUMBER_INVALID_PREFIX(".1");
    TEST_NUMBER_INVALID_PREFIX("e1");
}

static void _number_invalid_format(const char *str, size_t len, bool *okay)
{
    *okay = false;

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, str, len);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

    char buffer[len];
    buffer[len] = 0;

    size_t len_out = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_number(&c.cookie, buffer, len,
                                                      &len_out, &done));
    TEST_ASSERT_EQUAL_INT(len, len_out);
    TEST_ASSERT(done);
    TEST_ASSERT_EQUAL_INT(0, memcmp(str, buffer, len));

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));

    json_number_t num_type;
    TEST_ASSERT_EQUAL_INT(JSON_INVALID_DATA, json_number_type(buffer, len, &num_type));

    *okay = true;
}

#define TEST_NUMBER_INVALID_FORMAT(STR)                                       \
    do {                                                                      \
        static const char str[sizeof(STR) - 1] = STR;                         \
        bool okay;                                                            \
        _number_invalid_format(str, sizeof(str), &okay);                      \
        if (!okay) {                                                          \
            return;                                                           \
        }                                                                     \
    } while (0)

void tests_json_read_number_invalid_format(void)
{
    TEST_NUMBER_INVALID_FORMAT("01");
    TEST_NUMBER_INVALID_FORMAT("01.2");
    TEST_NUMBER_INVALID_FORMAT("1.2.3");
    TEST_NUMBER_INVALID_FORMAT("1.2e1.1");
}

static void _test_number_int_(const char *str, size_t len, int64_t expected_value,
                              bool *okay, json_number_t expected_type)
{
    *okay = false;

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, str, len);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

    char buffer[len];
    buffer[len] = 0;

    size_t len_out = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_number(&c.cookie, buffer, len,
                                                      &len_out, &done));
    TEST_ASSERT_EQUAL_INT(len, len_out);
    TEST_ASSERT(done);
    TEST_ASSERT_EQUAL_INT(0, memcmp(str, buffer, len));

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));

    json_number_t num_type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_number_type(buffer, len, &num_type));
    TEST_ASSERT_EQUAL_INT(expected_type, num_type);

    TEST_ASSERT_EQUAL_INT(expected_value, json_number_to_int(str, len));
    TEST_ASSERT_EQUAL_INT(expected_value, json_number_to_int(buffer, 0));

    *okay = true;
}

static inline void _test_number_zero(const char *str, size_t len,
                                     int64_t expected_value, bool *okay)
{
    _test_number_int_(str, len, expected_value, okay, JSON_NUMBER_ZERO);
}

static inline void _test_number_int(const char *str, size_t len,
                                    int64_t expected_value, bool *okay)
{
    _test_number_int_(str, len, expected_value, okay, JSON_NUMBER_INTEGER);
}

static void _assert_float(float expected, float actual, bool *okay)
{
    float diff = expected - actual;
    if (diff < 0) {
        diff = -diff;
    }

    if (diff > 1e-7) {
        *okay = false;
        char err[64];
        snprintf(err, sizeof(err), "Expected=%.7e, Actual=%.7e", expected, actual);
        TEST_FAIL(err);
    }
}

static void _test_number_float(const char *str, size_t len, float expected, bool *okay)
{
    *okay = false;

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, str, len);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

    char buffer[len + 1];
    buffer[len] = 0;

    size_t len_out = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_number(&c.cookie, buffer, len,
                                                      &len_out, &done));
    TEST_ASSERT_EQUAL_INT(len, len_out);
    TEST_ASSERT(done);
    TEST_ASSERT_EQUAL_INT(0, memcmp(str, buffer, len));

    json_number_t num_type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_number_type(buffer, len, &num_type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER_FLOAT, num_type);

    _assert_float(expected, json_number_to_float(str, len), okay);
    if (!okay) {
        return;
    }

    _assert_float(expected, json_number_to_float(buffer, 0), okay);
}

#define TEST_NUMBER(TYPE, TOKEN)                                              \
    do {                                                                      \
        static const char str[sizeof(#TOKEN) - 1] = #TOKEN;                   \
        bool okay;                                                            \
        _test_number_##TYPE(str, sizeof(str), TOKEN, &okay);                  \
        if (!okay) {                                                          \
            return;                                                           \
        }                                                                     \
    } while (0)

void tests_json_read_number_zero(void)
{
    TEST_NUMBER(zero, 0);
    TEST_NUMBER(zero, -0);
}

void tests_json_read_number_integer(void)
{
    TEST_NUMBER(int, 1);
    TEST_NUMBER(int, -1);
    TEST_NUMBER(int, 1234);
    TEST_NUMBER(int, -1234);
    TEST_NUMBER(int, 2147483647);
    TEST_NUMBER(int, -2147483648);
}

void tests_json_read_number_float(void)
{
    TEST_NUMBER(float, 0.0);
    TEST_NUMBER(float, -0.0);
    TEST_NUMBER(float, 0e0);

    TEST_NUMBER(float, 0.1);
    TEST_NUMBER(float, 1234.0);

    TEST_NUMBER(float, 1234e1);
    TEST_NUMBER(float, 1234e01);
    TEST_NUMBER(float, 1234e+1);
    TEST_NUMBER(float, 1234e+01);
    TEST_NUMBER(float, 1234e-1);
    TEST_NUMBER(float, 1234e-01);

    TEST_NUMBER(float, 1234E1);
    TEST_NUMBER(float, 1234E01);
    TEST_NUMBER(float, 1234E+1);
    TEST_NUMBER(float, 1234E+01);
    TEST_NUMBER(float, 1234E-1);
    TEST_NUMBER(float, 1234E-01);

    TEST_NUMBER(float, 1234.5e1);
    TEST_NUMBER(float, 1234.5e01);
    TEST_NUMBER(float, 1234.5e+1);
    TEST_NUMBER(float, 1234.5e+01);
    TEST_NUMBER(float, 1234.5e-1);
    TEST_NUMBER(float, 1234.5e-01);

    TEST_NUMBER(float, 1234.5E1);
    TEST_NUMBER(float, 1234.5E01);
    TEST_NUMBER(float, 1234.5E+1);
    TEST_NUMBER(float, 1234.5E+01);
    TEST_NUMBER(float, 1234.5E-1);
    TEST_NUMBER(float, 1234.5E-01);
}
