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

static void tests_json_write_literal(const char *expected,
                                     json_result_t (*fun)(json_write_cookie_t *))
{
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, expected, strlen(expected));

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, fun(&c.cookie));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_true(void)
{
    tests_json_write_literal("true", json_write_true);
}

void tests_json_write_false(void)
{
    tests_json_write_literal("false", json_write_false);
}

void tests_json_write_null(void)
{
    tests_json_write_literal("null", json_write_null);
}
