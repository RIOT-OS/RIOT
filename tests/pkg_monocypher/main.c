/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Monocypher package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "monocypher.h"
#include "embUnit.h"
#include "random.h"

static uint8_t message[] = "0123456789abcdef";

static uint8_t sign_sk[32];
static uint8_t sign_pk[32];
static uint8_t signature[64];

static void setUp(void)
{
    random_init(0);
}

static void test_monocypher_signverify(void)
{
    int res;
    /* Creating keypair ... */
    random_bytes(sign_sk, sizeof(sign_sk));
    crypto_sign_public_key(sign_pk, sign_sk);

    /* Sign */
    crypto_sign(signature, sign_sk, sign_pk, message, sizeof(message));

    /* Verifying... */
    res = crypto_check(signature, sign_pk, message, sizeof(message));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_monocypher_verifynegative(void)
{
    int res;

    /* changing message at random position (10) */
    message[0] = 'A';

    /* Verifying... */
    res = crypto_check(signature, sign_pk, message, sizeof(message));
    TEST_ASSERT_EQUAL_INT(-1, res);
}

Test *tests_monocypher(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_monocypher_signverify),
        new_TestFixture(test_monocypher_verifynegative)
    };

    EMB_UNIT_TESTCALLER(monocypher_tests, setUp, NULL, fixtures);
    return (Test*)&monocypher_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_monocypher());
    TESTS_END();

    return 0;
}
