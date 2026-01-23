/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>

#include "embUnit/embUnit.h"
#include "crypto/helper.h"

#define VALUE       0xAA

/* Secret to wipe */
static uint8_t secret[20];

void test_crypto_wipe(void)
{
    memset(secret, VALUE, sizeof(secret));
    /* Wipe everything except the last byte */
    crypto_secure_wipe(secret, sizeof(secret) - 1);
    for (size_t i = 0; i < (sizeof(secret) - 1); i++) {
        TEST_ASSERT_EQUAL_INT(0, secret[i]);
    }
    /* Check last byte */
    TEST_ASSERT_EQUAL_INT(VALUE, secret[19]);
}

Test *tests_crypto_helper_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_wipe),
    };
    EMB_UNIT_TESTCALLER(crypto_helper_tests, NULL, NULL, fixtures);
    return (Test *)&crypto_helper_tests;
}
