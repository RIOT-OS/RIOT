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

#include "tests-ubjson.h"

typedef enum {
    BEFORE_ARRAY_1 = __LINE__,
    IN_ARRAY_1 = __LINE__,

    BEFORE_ARRAY_2 = __LINE__,
    IN_ARRAY_2 = __LINE__,

    BEFORE_END = __LINE__,
} test_ubjson_empty_array_state_t;

typedef struct {
    ubjson_cookie_t cookie;
    test_ubjson_empty_array_state_t state;
} test_ubjson_empty_array_receiver_cookie_t;

static void test_ubjson_empty_array_receiver_callback_sub(
        ubjson_cookie_t *restrict cookie,
        ubjson_type_t type1, ssize_t content1,
        ubjson_type_t type2, ssize_t content2,
        ubjson_read_callback_result_t *result)
{
    *result = UBJSON_ABORTED;

    (void) content1, (void) type2, (void) content2;

    test_ubjson_empty_array_receiver_cookie_t *state;
    state = container_of(cookie, test_ubjson_empty_array_receiver_cookie_t, cookie);

    switch (state->state) {
        case BEFORE_ARRAY_1:
            TEST_ASSERT_EQUAL_INT(UBJSON_ENTER_ARRAY, type1);
            state->state = IN_ARRAY_1;
            TEST_ASSERT_EQUAL_INT(UBJSON_OKAY, ubjson_read_array(cookie));
            state->state = BEFORE_ARRAY_2;
            break;

        case IN_ARRAY_1:
            TEST_FAIL("The empty array was not empty (1)");
            break;

        case BEFORE_ARRAY_2:
            TEST_ASSERT_EQUAL_INT(UBJSON_ENTER_ARRAY, type1);
            state->state = IN_ARRAY_2;
            TEST_ASSERT_EQUAL_INT(UBJSON_OKAY, ubjson_read_array(cookie));
            state->state = BEFORE_END;
            break;

        case IN_ARRAY_2:
            TEST_FAIL("The empty array was not empty (2)");
            break;

        case BEFORE_END:
            TEST_FAIL("Content after the end");
            break;

        default:
            TEST_FAIL("The cookie was corrupted");
            break;
    }

    *result = UBJSON_OKAY;
}

static ubjson_read_callback_result_t test_ubjson_empty_array_receiver_callback(
        ubjson_cookie_t *restrict cookie,
        ubjson_type_t type1, ssize_t content1,
        ubjson_type_t type2, ssize_t content2)
{
    ubjson_read_callback_result_t result;
    test_ubjson_empty_array_receiver_callback_sub(cookie, type1, content1, type2, content2, &result);
    return result;
}

static void test_ubjson_empty_array_receiver(void)
{
    test_ubjson_empty_array_receiver_cookie_t state;
    state.state = BEFORE_ARRAY_1;

    TEST_ASSERT_EQUAL_INT(UBJSON_OKAY,
                          ubjson_read(&state.cookie, test_ubjson_read_fun,
                                      test_ubjson_empty_array_receiver_callback));
    TEST_ASSERT_EQUAL_INT(BEFORE_ARRAY_2, state.state);

    TEST_ASSERT_EQUAL_INT(UBJSON_OKAY,
                          ubjson_read(&state.cookie, test_ubjson_read_fun,
                                      test_ubjson_empty_array_receiver_callback));
    TEST_ASSERT_EQUAL_INT(BEFORE_END, state.state);
}

#undef EMBUNIT_ERROR_RETURN
#define EMBUNIT_ERROR_RETURN

static void test_ubjson_empty_array_sender(void)
{
    ubjson_cookie_t cookie;
    ubjson_write_init(&cookie, test_ubjson_write_fun);

    TEST_ASSERT(ubjson_open_array(&cookie) > 0);
    TEST_ASSERT(ubjson_close_array(&cookie) > 0);
    TEST_ASSERT(ubjson_open_array_len(&cookie, 0) > 0);
}

void test_ubjson_empty_array(void)
{
    test_ubjson_test(test_ubjson_empty_array_sender,
                     test_ubjson_empty_array_receiver);
}
