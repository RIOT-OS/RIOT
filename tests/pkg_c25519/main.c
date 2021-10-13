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
 * @brief       Tests c25519 package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "edsign.h"
#include "ed25519.h"
#include "embUnit.h"
#include "random.h"

static uint8_t message[] = "0123456789abcdef";

static uint8_t sign_sk[EDSIGN_SECRET_KEY_SIZE];
static uint8_t sign_pk[EDSIGN_PUBLIC_KEY_SIZE];
static uint8_t signature[EDSIGN_SIGNATURE_SIZE];

static void setUp(void)
{
    /* Initialize */
    random_init(0);
}

static void test_c25519_signverify(void)
{
    int res;
    /* Creating keypair ... */
    random_bytes(sign_sk, sizeof(sign_sk));
    ed25519_prepare(sign_sk);
    edsign_sec_to_pub(sign_pk, sign_sk);

    /* Sign */
    edsign_sign(signature, sign_pk, sign_sk, message, sizeof(message));

    /* Verifying... */
    res = edsign_verify(signature, sign_pk, message, sizeof(message));
    TEST_ASSERT(res);
}

static void test_c25519_verifynegative(void)
{
    int res;

    /* changing message */
    message[0] = 'A';

    /* Verifying... */
    res = edsign_verify(signature, sign_pk, message, sizeof(message));
    TEST_ASSERT_EQUAL_INT(0, res);
}

Test *tests_c25519(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_c25519_signverify),
        new_TestFixture(test_c25519_verifynegative)
    };

    EMB_UNIT_TESTCALLER(c25519_tests, setUp, NULL, fixtures);
    return (Test*)&c25519_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_c25519());
    TESTS_END();

    return 0;
}
