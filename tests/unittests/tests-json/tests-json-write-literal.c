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

void tests_json_write_true(void)
{
    static const char EXPECTED[] = "true";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_true(&c.cookie));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_false(void)
{
    static const char EXPECTED[] = "false";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_false(&c.cookie));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_null(void)
{
    static const char EXPECTED[] = "null";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_null(&c.cookie));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}
