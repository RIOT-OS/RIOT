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

static void _test_json_read_literal(const char *str,
                                   json_result_t (*fun)(json_read_cookie_t *),
                                   json_type_t expected_type)
{
    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, str, strlen(str));

    json_type_t actual_type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &actual_type));
    TEST_ASSERT_EQUAL_INT(expected_type, actual_type);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, fun(&c.cookie));

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &actual_type));
}

static void _test_json_read_illegal_literal(const char *str,
                                            json_result_t (*fun)(json_read_cookie_t *),
                                            json_type_t expected_type)
{
    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, str, strlen(str));

    json_type_t actual_type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &actual_type));
    TEST_ASSERT_EQUAL_INT(expected_type, actual_type);

    TEST_ASSERT_EQUAL_INT(JSON_INVALID_DATA, fun(&c.cookie));
}

void tests_json_read_true(void)
{
    _test_json_read_literal("true", json_read_true, JSON_TRUE);
}

void tests_json_read_false(void)
{
    _test_json_read_literal("false", json_read_false, JSON_FALSE);
}

void tests_json_read_null(void)
{
    _test_json_read_literal("null", json_read_null, JSON_NULL);
}

void tests_json_read_illegal_literal(void)
{
    _test_json_read_illegal_literal("tRUE",  json_read_true, JSON_TRUE);
    _test_json_read_illegal_literal("fals",  json_read_null, JSON_FALSE);
    _test_json_read_illegal_literal("nu ll", json_read_null, JSON_NULL);
}
