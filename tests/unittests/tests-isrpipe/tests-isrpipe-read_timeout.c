/*
 * Copyright (c) 2025 Prime Controls, Inc.(R)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "isrpipe/read_timeout.h"
#include "tests-isrpipe.h"

static void test_read_timeout(void)
{
    uint8_t buffer[2] = {0};
    uint8_t read_buf[2];
    isrpipe_t pipe;
    int res;

    /* prep the pipe */
    isrpipe_init(&pipe, buffer, ARRAY_SIZE(buffer));

    /* test timeout */
    res = isrpipe_read_timeout(&pipe, read_buf, ARRAY_SIZE(read_buf), 1);
    TEST_ASSERT_EQUAL_INT(-ETIMEDOUT, res);

    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&pipe, 1));
    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&pipe, 2));

    /* pipe now holds 1, 2 */

    /* test successful read */
    res = isrpipe_read_timeout(&pipe, read_buf, ARRAY_SIZE(read_buf), 1);
    TEST_ASSERT_EQUAL_INT(ARRAY_SIZE(read_buf), res);

    /* pipe now empty */

    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&pipe, 3));

    /* pipe now holds 3 */

    /* test partial read */
    res = isrpipe_read_timeout(&pipe, read_buf, ARRAY_SIZE(read_buf), 1);
    TEST_ASSERT_EQUAL_INT(1, res);
}

static void test_read_all_timeout(void)
{
    uint8_t buffer[2] = {0};
    uint8_t read_buf[2];
    isrpipe_t pipe;
    int res;

    /* prep the pipe */
    isrpipe_init(&pipe, buffer, ARRAY_SIZE(buffer));

    /* test timeout */
    res = isrpipe_read_all_timeout(&pipe, read_buf, ARRAY_SIZE(read_buf), 1);
    TEST_ASSERT_EQUAL_INT(-ETIMEDOUT, res);

    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&pipe, 1));
    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&pipe, 2));

    /* pipe now holds 1, 2 */

    /* test successful read */
    res = isrpipe_read_all_timeout(&pipe, read_buf, ARRAY_SIZE(read_buf), 1);
    TEST_ASSERT_EQUAL_INT(ARRAY_SIZE(read_buf), res);

    /* pipe now empty */

    TEST_ASSERT_EQUAL_INT(0, isrpipe_write_one(&pipe, 3));

    /* pipe now holds 3 */

    /* test timeout - isrpipe_read_all_timeout() does not allow partial reads */
    res = isrpipe_read_all_timeout(&pipe, read_buf, ARRAY_SIZE(read_buf), 1);
    TEST_ASSERT_EQUAL_INT(-ETIMEDOUT, res);
}

Test *tests_isrpipe_read_timeout_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_read_timeout),
        new_TestFixture(test_read_all_timeout),
    };

    EMB_UNIT_TESTCALLER(isrpipe_tests, NULL, NULL, fixtures);

    return (Test *)&isrpipe_tests;
}
