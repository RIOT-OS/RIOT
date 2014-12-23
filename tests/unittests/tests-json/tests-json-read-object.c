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

void tests_json_read_empty_object(void)
{
    static const char DATA[] = "{}";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_OBJECT, type);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_object(&c.cookie));

    bool closed;
    char key[1] = "X";
    size_t len = 0;
    bool key_read;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_object_next(&c.cookie, &closed,
                                                           key, sizeof(key), &len, &key_read));
    TEST_ASSERT(closed);
    TEST_ASSERT_EQUAL_INT(0, len);
    TEST_ASSERT_EQUAL_INT('X', key[0]);
    /* when closed, then key_read is spurious */

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_object(void)
{
    static const char DATA[] = "{\"key1\":\"value1\",\"key2\":2}";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_OBJECT, type);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_object(&c.cookie));

    bool closed;
    char key[5];
    size_t len = 0;
    bool key_read;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_object_next(&c.cookie, &closed,
                                                           key, sizeof(key), &len, &key_read));
    TEST_ASSERT(!closed);
    TEST_ASSERT_EQUAL_INT(sizeof(key) - 1, len);
    TEST_ASSERT(key_read);
    key[sizeof(key) - 1] = 0;
    TEST_ASSERT_EQUAL_STRING("key1", key);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_STRING, type);

    char value1[7];
    len = 0;
    bool value1_read;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_string(&c.cookie, value1, sizeof(value1),
                                                      &len, &value1_read));
    TEST_ASSERT(value1_read);
    TEST_ASSERT_EQUAL_INT(sizeof(value1) - 1, len);
    value1[sizeof(value1) - 1] = 0;
    TEST_ASSERT_EQUAL_STRING("value1", value1);

    len = 0;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_object_next(&c.cookie, &closed,
                                                           key, 3, &len, &key_read));
    TEST_ASSERT(!closed);
    TEST_ASSERT_EQUAL_INT(3, len);
    TEST_ASSERT(!key_read);
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_object_next(&c.cookie, &closed,
                                                           key + len, 2, &len, &key_read));
    TEST_ASSERT(!closed);
    TEST_ASSERT_EQUAL_INT(4, len);
    TEST_ASSERT(key_read);
    key[sizeof(key) - 1] = 0;
    TEST_ASSERT_EQUAL_STRING("key2", key);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);
    bool okay;
    tests_json_read_digit(&c.cookie, 2, &okay);
    if (!okay) {
        return;
    }

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_object_next(&c.cookie, &closed,
                                                           key, 3, &len, &key_read));
    TEST_ASSERT(closed);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}
