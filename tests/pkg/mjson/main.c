/*
 * SPDX-FileCopyrightText: 2023 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for mjson
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include "embUnit.h"
#include "mjson.h"

void test_mjson_encode(void)
{
    /* Print into a statically allocated buffer */
    char buf[100];
    mjson_snprintf(buf, sizeof(buf), "{%Q:%d}", "a", 123);

    static const char expected[] = "{\"a\":123}";

    TEST_ASSERT_EQUAL_INT(0, strcmp(expected, buf));
}

void test_mjson_decode(void)
{
    static const char input[] = "{\"a\":1,\"b\":[2,false]}";  /* {"a":1,"b":[2,false]} */

    double dval = 0;
    int res = mjson_get_number(input, strlen(input), "$.a", &dval);
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_INT(1, dval);

    const char *buf;
    int buf_len;
    res = mjson_find(input, strlen(input), "$.b", &buf, &buf_len);
    TEST_ASSERT_EQUAL_INT(91, res);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, "[2,false]", buf_len));

    res = mjson_get_number(input, strlen(input), "$.b[0]", &dval);
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_INT(2, dval);

    int ival = 0;
    res = mjson_get_bool(input, strlen(input), "$.b[1]", &ival);
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_INT(0, ival);

}

Test *tests_mjson(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mjson_encode),
        new_TestFixture(test_mjson_decode),
    };

    EMB_UNIT_TESTCALLER(mjson_tests, NULL, NULL, fixtures);
    return (Test*)&mjson_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mjson());
    TESTS_END();

    return 0;
}
