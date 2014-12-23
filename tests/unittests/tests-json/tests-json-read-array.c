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

void tests_json_read_empty_array(void)
{
    static const char DATA[] = "[]";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_ARRAY, type);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));

    bool closed;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
    TEST_ASSERT(closed);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_array(void)
{
    static const char DATA[] = "[1 ,2, 3]";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
    TEST_ASSERT_EQUAL_INT(JSON_ARRAY, type);

    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));

    for (int i = 1; i <= 3; ++i) {
        bool closed;
        TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
        TEST_ASSERT(!closed);

        TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_peek(&c.cookie, &type));
        TEST_ASSERT_EQUAL_INT(JSON_NUMBER, type);

        READ_DIGIT(i);
    }

    bool closed;
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
    TEST_ASSERT(closed);

    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}

void tests_json_read_nested_array(void)
{
    static const char DATA[] = "[[[1,2],[[]],[3,4]]]";

    tests_json_read_cookie_t c;
    tests_json_setup_cookie(&c, DATA, sizeof(DATA) - 1);

    bool closed;

    // [   --   [[1,2],[[]],[3,4]]]
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));
        // [   --   [1,2],[[]],[3,4]]]
        TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
        TEST_ASSERT(!closed);
        TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));
            // [   --   1,2],[[]],[3,4]]]
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
            TEST_ASSERT(!closed);
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));
                // 1   --   2],[[]],[3,4]]]
                TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
                TEST_ASSERT(!closed);
                READ_DIGIT(1);
                // 2   --   ],[[]],[3,4]]]
                TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
                TEST_ASSERT(!closed);
                READ_DIGIT(2);
            // ]   --   [[]],[3,4]]]
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
            TEST_ASSERT(closed);

            // [   --   []],[3,4]]]
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
            TEST_ASSERT(!closed);
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));
                // [   --   ]],[3,4]]]
                TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
                TEST_ASSERT(!closed);
                TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));
                // ]   --   ],[3,4]]]
                TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
                TEST_ASSERT(closed);
            // ]   --   [3,4]]]
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
            TEST_ASSERT(closed);

            // [   --   3,4]]]
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
            TEST_ASSERT(!closed);
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array(&c.cookie));
                // 3   --   4]]]
                TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
                TEST_ASSERT(!closed);
                READ_DIGIT(3);
                // 4   --   ]]]
                TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
                TEST_ASSERT(!closed);
                READ_DIGIT(4);
            // ]   --   ]]
            TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
            TEST_ASSERT(closed);
        // ]   --   ]
        TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
        TEST_ASSERT(closed);
    // ]   --   done
    TEST_ASSERT_EQUAL_INT(JSON_OKAY, json_read_array_next(&c.cookie, &closed));
    TEST_ASSERT(closed);

    json_type_t type;
    TEST_ASSERT_EQUAL_INT(JSON_PREMATURELY_ENDED, json_read_peek(&c.cookie, &type));
}
