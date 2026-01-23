/*
 * SPDX-FileCopyrightText: 2018 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the libhydrogen package
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#include "embUnit.h"
#include "hydrogen.h"

static char context[] = "examples";
static char message[] = "0123456789abcdef";

/* This performs setup, but should never fail */
static void test_hydro_init(void)
{
    TEST_ASSERT(hydro_init() == 0);
}

/* Test public-key signatures */
static void test_hydro_signverify(void)
{
    hydro_sign_keypair key_pair;

    hydro_sign_keygen(&key_pair);

    uint8_t signature[hydro_sign_BYTES];

    hydro_sign_create(signature, message, sizeof message, context, key_pair.sk);

    int res = hydro_sign_verify(signature, message, sizeof message, context, key_pair.pk);

    TEST_ASSERT(res == 0);
}

/* Test secret-key encryption */
static void test_hydro_secretbox_encryptdecrypt(void)
{
    uint8_t key[hydro_secretbox_KEYBYTES];
    uint8_t ciphertext[hydro_secretbox_HEADERBYTES + sizeof message];

    hydro_secretbox_keygen(key);
    hydro_secretbox_encrypt(ciphertext, message, sizeof message, 0, context, key);

    char decrypted[sizeof message];
    int res = hydro_secretbox_decrypt(
        decrypted,
        ciphertext,
        hydro_secretbox_HEADERBYTES + sizeof message,
        0,
        context,
        key
        );

    TEST_ASSERT(res == 0);
}

Test *tests_libhydrogen(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hydro_init),
        new_TestFixture(test_hydro_signverify),
        new_TestFixture(test_hydro_secretbox_encryptdecrypt),
    };
    EMB_UNIT_TESTCALLER(libhydrogen_tests, NULL, NULL, fixtures);
    return (Test *)&libhydrogen_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_libhydrogen());
    TESTS_END();
    return 0;
}
