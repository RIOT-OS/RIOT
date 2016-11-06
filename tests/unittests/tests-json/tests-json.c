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

#include <stdio.h>

static int _cookie_read(json_read_cookie_t *cookie)
{
    tests_json_read_cookie_t *c = _get_cookie(cookie);
    if (c->pos >= c->length) {
        return -1;
    }

    return (unsigned char) c->data[c->pos++];
}

void tests_json_setup_cookie(tests_json_read_cookie_t *c, const char *data, size_t size)
{
    json_read_init(&c->cookie, _cookie_read);
    c->data = data;
    c->pos = 0;
    c->length = size;
}

static ssize_t _cookie_write(json_write_cookie_t *cookie, const char *buf, size_t len)
{
    tests_json_write_cookie_t *c = _get_write_cookie(cookie);
    for (size_t i = 0; i < len; ++i) {
        if (tests_json_write_cookie_is_end(c)) {
            printf("\nTried to write past end at position %zu (null-index)\n", len);
            return -1;
        }

        char expected = c->expected[c->pos];
        char actual = buf[i];
        if (expected != actual) {
            printf("\nError at position %zu (null-index). Expected <0x%02x '%c'>, got <0x%02x '%c'>.\n",
                   len, (unsigned) expected, expected, (unsigned) actual, actual);
            return -1;
        }

        ++c->pos;
    }
    return len;
}

void tests_json_setup_write_cookie(tests_json_write_cookie_t *c, const char *expected, size_t len)
{
    json_write_init(&c->cookie, _cookie_write);
    c->expected = expected;
    c->pos = 0;
    c->length = len;
}

void tests_json_read_digit(json_read_cookie_t *cookie, int digit, bool *okay)
{
    *okay = false;

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

    char n[2] = "X";
    size_t n_len = 0;
    bool n_done;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_number(cookie, n,
                                                      sizeof(n), &n_len,
                                                      &n_done));
    TEST_ASSERT_EQUAL_INT(1, n_len);
    TEST_ASSERT(n_done);
    char should_be[2] = { '0' + digit, 0 };
    TEST_ASSERT_EQUAL_STRING(should_be, n);

    *okay = true;
}

#define TESTS_JSON_NEW_TEST_FIXTURE(NAME) new_TestFixture(tests_json_##NAME),

EMB_UNIT_TESTFIXTURES(tests_json_fixtures) {
    MAP(TESTS_JSON_NEW_TEST_FIXTURE, TESTS_JSON_ALL)
    new_TestFixture(tests_json_parsing)
};
EMB_UNIT_TESTCALLER(tests_json_tests, NULL, NULL, tests_json_fixtures);

void tests_json(void)
{
    TESTS_RUN((Test *) &tests_json_tests);
}
