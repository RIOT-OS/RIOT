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

#define STR_VALUE(X) { #X, X }

void tests_json_read_number_invalid_prefix(void)
{
    static const char *const strs[] = { "+0", ".1", "e1", NULL };
    for (const char *const *str = strs; *str; ++str) {
        tests_json_read_cookie_t c;
        tests_json_setup_cookie(&c, *str, strlen(*str));

        json_type_t type;
        TEST_ASSERT_EQUAL_INT(JSON_INVALID_DATA, json_read_peek(&c.cookie, &type));
    }
}

void tests_json_read_number_invalid_format(void)
{
    static const char *const strs[] = { "01", "01.2", "1.2.3", "1.2e1.1", NULL };
    for (const char *const *str = strs; *str; ++str) {
        size_t len = strlen(*str);

        tests_json_read_cookie_t c;
        tests_json_setup_cookie(&c, *str, len);

        json_type_t type;
        TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
        TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

        char buffer[8];
        buffer[sizeof(buffer) - 1] = 0;

        size_t len_out = 0;
        bool done;
        TEST_ASSERT_EQUAL_INT(JSON_OKAY,
                              json_read_number(&c.cookie, buffer, len, &len_out, &done));
        TEST_ASSERT_EQUAL_INT(len, len_out);
        TEST_ASSERT(done);
        TEST_ASSERT_EQUAL_INT(0, memcmp(*str, buffer, len));

        TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));

        json_number_t num_type;
        TEST_ASSERT_EQUAL_INT(JSON_INVALID_DATA, json_number_type(buffer, len, &num_type));
    }
}

static void _test_number_int(const char *str, int64_t expected_value,
                             bool *okay, json_number_t expected_type)
{
    *okay = false;

    size_t len = strlen(str);

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, str, len);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

    char buffer[16];
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

#ifndef MODULE_ATMEGA_COMMON
    TEST_ASSERT_EQUAL_INT(expected_value, json_number_to_int(str, len));
    TEST_ASSERT_EQUAL_INT(expected_value, json_number_to_int(buffer, 0));
#endif /*ifndef MODULE_ATMEGA_COMMON*/

    *okay = true;
}

void tests_json_read_number_zero(void)
{
    static const char *const list[] = { "0", "-0", NULL };

    bool okay = true;
    for (const char *const *str = list; okay && *str; ++str) {
        _test_number_int(*str, 0, &okay, JSON_NUMBER_ZERO);
    }
}

void tests_json_read_number_integer(void)
{
    struct item_t {
        const char *str;
        int32_t value;
    };

    static const struct item_t list[] = {
        STR_VALUE(1),
        STR_VALUE(-1),
        STR_VALUE(1234),
        STR_VALUE(-1234),
        STR_VALUE(2147483647),
        STR_VALUE(-2147483648),
        { NULL, 0 },
    };

    bool okay = true;
    for (const struct item_t *item = list; okay && item->str; ++item) {
        _test_number_int(item->str, item->value, &okay, JSON_NUMBER_INTEGER);
    }
}

#ifndef MODULE_ATMEGA_COMMON
static void _assert_float(float expected, float actual, bool *okay)
{
    float diff = expected - actual;
    if (diff < 0) {
        diff = -diff;
    }

    if (diff > 1e-7) {
        *okay = false;
        char err[64];
        snprintf(err, sizeof(err), "Expected=%.7e, Actual=%.7e", (double) expected, (double) actual);
        TEST_FAIL(err);
    }
}
#endif /*ifndef MODULE_ATMEGA_COMMON*/

static void _test_number_float(const char *str, float expected, bool *okay)
{
    *okay = false;

    size_t len = strlen(str);

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, str, len);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

    char buffer[16];
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

#ifndef MODULE_ATMEGA_COMMON
    _assert_float(expected, json_number_to_float(str, len), okay);
    if (!*okay) {
        return;
    }

    _assert_float(expected, json_number_to_float(buffer, 0), okay);
#endif /*ifndef MODULE_ATMEGA_COMMON*/
}

void tests_json_read_number_float(void)
{
    struct item_t {
        const char *str;
        float value;
    };

    static const struct item_t list[] = {
        STR_VALUE(0.0),
        STR_VALUE(-0.0),
        STR_VALUE(0e0),

        STR_VALUE(0.1),
        STR_VALUE(1234.0),

        STR_VALUE(1234e1),
        STR_VALUE(1234e01),
        STR_VALUE(1234e+1),
        STR_VALUE(1234e+01),
        STR_VALUE(1234e-1),
        STR_VALUE(1234e-01),

        STR_VALUE(1234E1),
        STR_VALUE(1234E01),
        STR_VALUE(1234E+1),
        STR_VALUE(1234E+01),
        STR_VALUE(1234E-1),
        STR_VALUE(1234E-01),

        STR_VALUE(1234.5e1),
        STR_VALUE(1234.5e01),
        STR_VALUE(1234.5e+1),
        STR_VALUE(1234.5e+01),
        STR_VALUE(1234.5e-1),
        STR_VALUE(1234.5e-01),

        STR_VALUE(1234.5E1),
        STR_VALUE(1234.5E01),
        STR_VALUE(1234.5E+1),
        STR_VALUE(1234.5E+01),
        STR_VALUE(1234.5E-1),
        STR_VALUE(1234.5E-01),

        { NULL, 0.0 },
    };

    bool okay = true;
    for (const struct item_t *item = list; okay && item->str; ++item) {
        _test_number_float(item->str, item->value, &okay);
    }
}
