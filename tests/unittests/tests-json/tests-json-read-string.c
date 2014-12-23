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

void tests_json_read_empty_string(void)
{
    static const char DATA[] = "\"\"";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_STRING, type);

    char data[1];
    size_t len = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data, sizeof(data), &len, &done));
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT(done);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_ascii_string(void)
{
    static const char DATA[] = "\"abcdef\"";
    static const char SHOULD_BE[] = "abcdef";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_STRING, type);

    char data[sizeof(SHOULD_BE)];
    size_t len = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data, sizeof(data), &len, &done));
    TEST_ASSERT_EQUAL_INT(sizeof(data) - 1, len);
    TEST_ASSERT(done);

    data[len] = 0;
    TEST_ASSERT_EQUAL_STRING(SHOULD_BE, data);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_escaped_ascii_string(void)
{
    static const char DATA[] = "\"A\\\"B\\\\C\\/D\\bE\\fF\\nG\\rH\\tI\\u0020J\"";
    static const char SHOULD_BE[] = "A\"B\\C/D\bE\fF\nG\rH\tI J";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_STRING, type);

    char data[sizeof(SHOULD_BE)];
    size_t len = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data, sizeof(data), &len, &done));
    TEST_ASSERT_EQUAL_INT(sizeof(data) - 1, len);
    TEST_ASSERT(done);

    data[len] = 0;
    TEST_ASSERT_EQUAL_STRING(SHOULD_BE, data);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_unicode_string1(void)
{
    static const char DATA[] = "\"abc\\u20ACdef\"";
    static const char SHOULD_BE[] = "abcXdef";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_STRING, type);

    char data[sizeof(SHOULD_BE)];
    size_t len = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_READ_UNICODE, json_read_string(&c.cookie, data, sizeof(data), &len, &done));
    TEST_ASSERT_EQUAL_INT(3, len);
    TEST_ASSERT(!done);

    TEST_ASSERT_EQUAL_INT(0x20AC, json_read_unicode(&c.cookie));

    data[len++] = 'X';

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data + len, sizeof(data) - len, &len, &done));
    TEST_ASSERT_EQUAL_INT(sizeof(data) - 1, len);
    TEST_ASSERT(done);

    data[len] = 0;
    TEST_ASSERT_EQUAL_STRING(SHOULD_BE, data);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_unicode_string2(void)
{
    static const char DATA[] = "\"abc\\uD834\\uDD1Edef\"";
    static const char SHOULD_BE[] = "abcXdef";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_STRING, type);

    char data[sizeof(SHOULD_BE)];
    size_t len = 0;
    bool done;
    TEST_ASSERT_EQUAL_INT(JSON_READ_UNICODE, json_read_string(&c.cookie, data, sizeof(data), &len, &done));
    TEST_ASSERT_EQUAL_INT(3, len);
    TEST_ASSERT(!done);

    TEST_ASSERT_EQUAL_INT(0x1D11E, json_read_unicode(&c.cookie));

    data[len++] = 'X';

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data + len, sizeof(data) - len, &len, &done));
    TEST_ASSERT_EQUAL_INT(sizeof(data) - 1, len);
    TEST_ASSERT(done);

    data[len] = 0;
    TEST_ASSERT_EQUAL_STRING(SHOULD_BE, data);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_string_bits(void)
{
    static const char DATA[] = "\"abcde\"";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_STRING, type);

    char data[3] = { 0, 0, 0 };
    size_t read_total = 0;
    bool done;

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data, 2, &read_total, &done));
    TEST_ASSERT_EQUAL_INT(2, read_total);
    TEST_ASSERT_EQUAL_STRING("ab", data);
    TEST_ASSERT(!done);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data, 2, &read_total, &done));
    TEST_ASSERT_EQUAL_INT(4, read_total);
    TEST_ASSERT_EQUAL_STRING("cd", data);
    TEST_ASSERT(!done);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, data, 2, &read_total, &done));
    TEST_ASSERT_EQUAL_INT(5, read_total);
    TEST_ASSERT_EQUAL_STRING("ed"/*sic*/, data);
    TEST_ASSERT(done);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}
