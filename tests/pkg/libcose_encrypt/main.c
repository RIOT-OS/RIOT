/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for pkg libcose encryption
 *
 * @author     Koen Zandberg <koen@bergzand.net>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cose.h"
#include "cose/sign.h"
#include "cose/crypto.h"
#include "embUnit.h"
#include "memarray.h"
#include "random.h"

static uint8_t payload[] = "This is the content.";
static uint8_t buf[2048];
static uint8_t plaintext[2048];
static uint8_t kid[] = "sec-256";

static void setUp(void)
{
    /* Initialize */
    random_init(0);
    /* Clear buffer */
    memset(plaintext, 0, sizeof(plaintext));
    memset(buf, 0, sizeof(buf));
}

static void _test_encrypt_generic(cose_algo_t algo)
{
    uint8_t *out;
    uint8_t key_bytes[64];
    static const uint8_t nonce_bytes[32] = { 0 };
    cose_encrypt_t crypt;
    cose_key_t key;

    cose_crypto_keygen(key_bytes, sizeof(key_bytes), algo);

    cose_key_init(&key); /* Generate key */
    cose_key_set_kid(&key, kid, sizeof(kid) - 1);
    cose_key_set_keys(&key, 0, algo, NULL, NULL, key_bytes);

    cose_encrypt_init(&crypt, COSE_FLAGS_ENCRYPT0);
    cose_encrypt_add_recipient(&crypt, &key);
    cose_encrypt_set_payload(&crypt, payload, sizeof(payload) - 1);
    cose_encrypt_set_algo(&crypt, COSE_ALGO_DIRECT);
    COSE_ssize_t len = cose_encrypt_encode(&crypt, buf, sizeof(buf), nonce_bytes, &out);
    cose_encrypt_dec_t decrypt;

    TEST_ASSERT_EQUAL_INT(0, cose_encrypt_decode(&decrypt, out, len));
    size_t plaintext_len = 0;

    TEST_ASSERT_EQUAL_INT(0,
                          cose_encrypt_decrypt(&decrypt, NULL, &key, buf, sizeof(buf), plaintext,
                                               &plaintext_len));
    TEST_ASSERT_EQUAL_INT(sizeof(payload) - 1, plaintext_len);
}

#ifdef HAVE_ALGO_CHACHA20POLY1305
static void test_libcose_chacha20poly1305(void)
{
    _test_encrypt_generic(COSE_ALGO_CHACHA20POLY1305);
}
#endif

Test *tests_libcose_encrypt(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
#ifdef HAVE_ALGO_CHACHA20POLY1305
        new_TestFixture(test_libcose_chacha20poly1305),
#endif
    };

    EMB_UNIT_TESTCALLER(libcose_encrypt_tests, setUp, NULL, fixtures);
    return (Test *)&libcose_encrypt_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_libcose_encrypt());
    TESTS_END();
    return 0;
}
