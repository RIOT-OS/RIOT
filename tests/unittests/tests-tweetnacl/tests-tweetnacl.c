/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       tweetnacl NaCl crypto library tests
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * @}
 */

#include <string.h>

#include <tweetnacl.h>
#include "embUnit.h"
#include "tests-tweetnacl.h"

static const char message[] = "0123456789abcdef";
static char r[sizeof(message)];

#define MLEN (sizeof(message) + crypto_box_ZEROBYTES)
#define SMLEN (sizeof(message) + crypto_sign_BYTES)

static unsigned char alice_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char alice_sk[crypto_box_SECRETKEYBYTES];
static unsigned char bob_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char bob_sk[crypto_box_SECRETKEYBYTES];
static unsigned char m[MLEN];
static unsigned char c[MLEN];
static const unsigned char n[crypto_box_NONCEBYTES];
static unsigned char result[MLEN];

static unsigned char sign_sk[crypto_sign_SECRETKEYBYTES];
static unsigned char sign_pk[crypto_sign_PUBLICKEYBYTES];
static unsigned char verify_result[SMLEN];
static unsigned char sm[SMLEN];

static void setUp(void)
{
    /* Initialize */
    random_init(0);
}

static void tearDown(void)
{
    /* Finalize */
}

static void test_tweetnacl_01(void)
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

    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", (const char*)r);
}

static void test_tweetnacl_02(void)
{
    int res;

    unsigned long long int smlen;
    unsigned long long int verify_result_len;

    memset(sm, '\0', SMLEN);

    /* Creating keypair ... */
    crypto_sign_keypair(sign_pk, sign_sk);

    /* Sign */
    crypto_sign(sm, &smlen, (const unsigned char *)message, sizeof(message), sign_sk);

    /* Verifying... */
    res = crypto_sign_open(verify_result, &verify_result_len, sm, smlen, sign_pk);

    TEST_ASSERT_EQUAL_INT(SMLEN, smlen);
    TEST_ASSERT_EQUAL_INT(0, res);

    memset(r, 0, sizeof(r));
    memcpy(r, verify_result + crypto_sign_BYTES, SMLEN - crypto_sign_BYTES);

    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", (const char*)r);
}

static void test_tweetnacl_03(void)
{
    int res;

    unsigned long long int verify_result_len;

    /* changing message at random position (10) */
    sm[crypto_sign_BYTES + 10] = 'A';

    /* Verifying... */
    res = crypto_sign_open(verify_result, &verify_result_len, sm, SMLEN, sign_pk);

    TEST_ASSERT_EQUAL_INT(-1, res);
}

Test *tests_tweetnacl_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_tweetnacl_01),
        new_TestFixture(test_tweetnacl_02),
        new_TestFixture(test_tweetnacl_03)
    };

    EMB_UNIT_TESTCALLER(tweetnacl_tests, setUp, tearDown, fixtures);
    return (Test*)&tweetnacl_tests;
}

void tests_tweetnacl(void)
{
    TESTS_RUN(tests_tweetnacl_all());
}
