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

void tests_json_write_zero(void)
{
    static const char EXPECTED[] = "0";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_int(&c.cookie, 0));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_pos_int(void)
{
    static const char EXPECTED[] = "4711";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_int(&c.cookie, 4711));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_neg_int(void)
{
    static const char EXPECTED[] = "-4711";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_int(&c.cookie, -4711));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

#ifndef MODULE_ATMEGA_COMMON
void tests_json_write_zero64(void)
{
    static const char EXPECTED[] = "0";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_int64(&c.cookie, 0));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_pos_int64(void)
{
    static const char EXPECTED[] = "4711";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_int64(&c.cookie, 4711));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_neg_int64(void)
{
    static const char EXPECTED[] = "-4711";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_int64(&c.cookie, -4711));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}
#endif /*ifndef MODULE_ATMEGA_COMMON*/

void tests_json_write_zero_float(void)
{
    /* FIXME: this test sucks. There are many representations for a floating point zero
     *        including 0, 0.0, 0.00, ..., 0e0, 0.0e+0, 0e-0, ... */
    static const char EXPECTED[] = "0.000000000e+00";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_float(&c.cookie, 0.0));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_pos_float(void)
{
    /* FIXME: this test sucks. There are many representations for the same floating point number */

    static const char EXPECTED[] = "4.711000061e+01";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_float(&c.cookie, 47.11));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}

void tests_json_write_neg_float(void)
{
    /* FIXME: this test sucks. There are many representations for the same floating point number */

    static const char EXPECTED[] = "-4.711000061e+01";
    tests_json_write_cookie_t c;
    tests_json_setup_write_cookie(&c, EXPECTED, sizeof(EXPECTED) - 1);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_float(&c.cookie, -47.11));
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_write_finish(&c.cookie));

    TEST_ASSERT(tests_json_write_cookie_is_end(&c));
}
