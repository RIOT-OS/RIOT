/*
 * SPDX-FileCopyrightText: 2018 INRIA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       HACL* crypto library tests
 *
 * @author      Benjamin Beurdouche <benjamin.beurdouche@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <haclnacl.h>
#include "random.h"
#include "embUnit.h"

static const char message[] = "0123456789abcdef";
static char r[sizeof(message)];

#define MLEN (sizeof(message) + crypto_box_ZEROBYTES)

static unsigned char alice_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char alice_sk[crypto_box_SECRETKEYBYTES];
static unsigned char bob_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char bob_sk[crypto_box_SECRETKEYBYTES];
static unsigned char m[MLEN];
static unsigned char c[MLEN];
static const unsigned char n[crypto_box_NONCEBYTES];
static unsigned char result[MLEN];

static void setUp(void)
{
    /* Initialize */
    random_init(0);
}

static void test_hacl_01(void)
{
    int res;

    /* Creating keypair ALICE... */
    crypto_box_keypair(alice_pk, alice_sk);

    /* Creating keypair BOB... */
    crypto_box_keypair(bob_pk, bob_sk);

    memset(m, 0, crypto_box_ZEROBYTES);
    memcpy(m + crypto_box_ZEROBYTES, message, MLEN - crypto_box_ZEROBYTES);

    /* Encrypting using pk_bob... */
    crypto_box(c, m, MLEN, n, bob_pk, alice_sk);

    memset(result, '\0', sizeof(result));

    /* Decrypting... */
    res = crypto_box_open(result, c, MLEN, n, alice_pk, bob_sk);

    TEST_ASSERT_EQUAL_INT(0, res);

    memset(r, 0, sizeof(r));
    memcpy(r, result + crypto_box_ZEROBYTES, MLEN - crypto_box_ZEROBYTES);

    TEST_ASSERT_EQUAL_STRING((const char*)message, (const char*)r);
}

Test *tests_hacl(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hacl_01)
    };

    EMB_UNIT_TESTCALLER(hacl_tests, setUp, NULL, fixtures);
    return (Test*)&hacl_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_hacl());
    TESTS_END();
    return 0;
}
