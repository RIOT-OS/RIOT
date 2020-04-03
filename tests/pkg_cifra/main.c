/*
 * Copyright (C) 2019 Koen Zandberg
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
 * @brief       Tests cifra package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "embUnit.h"
#include "random.h"
#include "aes.h"
#include "modes.h"

#define SECRET_KEY_LEN  32

static uint8_t message[] = "0123456789abcdef";
static uint8_t nonce[] = "0123456789abcdef";

static uint8_t cipher[sizeof(message)];
static uint8_t tag[16];

static uint8_t decrypted[sizeof(message)];

static uint8_t key[SECRET_KEY_LEN];

static void setUp(void)
{
    /* Initialize */
    random_init(0);

    random_bytes(key, SECRET_KEY_LEN);
}

static void test_cifra_gcm(void)
{
    cf_aes_context aes;
    cf_aes_init(&aes, key, SECRET_KEY_LEN);

    cf_gcm_encrypt(&cf_aes, &aes,
                   message, sizeof(message),
                   NULL, 0,
                   nonce, sizeof(nonce),
                   cipher,
                   tag, 16);

    int res = cf_gcm_decrypt(&cf_aes, &aes,
                             cipher, sizeof(message),
                             NULL, 0,
                             nonce, sizeof(nonce),
                             tag, 16,
                             decrypted);

    TEST_ASSERT(res == 0);
    TEST_ASSERT_EQUAL_STRING((char *)message, (char *)decrypted);

    /* Flip bit in the ciphertext */
    cipher[0] ^= 0x01;

    res = cf_gcm_decrypt(&cf_aes, &aes,
                         cipher, sizeof(message),
                         NULL, 0,
                         nonce, sizeof(nonce),
                         tag, 16,
                         decrypted);

    TEST_ASSERT(res == 1);

    /* Flip bit in the ciphertext back */
    cipher[0] ^= 0x01;

    /* Flip bit in the key */
    key[0] ^= 0x01;

    /* Reimport the key */
    cf_aes_init(&aes, key, SECRET_KEY_LEN);

    res = cf_gcm_decrypt(&cf_aes, &aes,
                         cipher, sizeof(message),
                         NULL, 0,
                         nonce, sizeof(nonce),
                         tag, 16,
                         decrypted);

    TEST_ASSERT(res == 1);
}

Test *tests_cifra(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_cifra_gcm),
    };

    EMB_UNIT_TESTCALLER(cifra_tests, setUp, NULL, fixtures);
    return (Test*)&cifra_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_cifra());
    TESTS_END();

    return 0;
}
