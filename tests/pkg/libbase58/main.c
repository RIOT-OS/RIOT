/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for pkg libbase58
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "libbase58.h"
#include "embUnit.h"

static void test_libbase58_01(void)
{
    const char source[] = "base 58 test string";
    const char encoded[] = "2NVj5VV1YMqTHot6N2xPBQnbqnUF";
    char decoded[sizeof(source)] = {0};
    char target[64] = {0};
    size_t target_len = sizeof(target);
    size_t decoded_len = sizeof(decoded);

    /* testing encoding */
    b58enc(target, &target_len, source, sizeof(source));

    TEST_ASSERT_EQUAL_INT(target_len, sizeof(encoded));
    TEST_ASSERT(strcmp(target, encoded) == 0);

    /* testing decoding */
    b58tobin(decoded, &decoded_len, target, target_len - 1);

    TEST_ASSERT(memcmp(source, decoded, sizeof(source)) == 0);
}

Test *tests_libbase58(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_libbase58_01),
    };

    EMB_UNIT_TESTCALLER(libbase58_tests, NULL, NULL, fixtures);
    return (Test *)&libbase58_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_libbase58());
    TESTS_END();
    return 0;
}
