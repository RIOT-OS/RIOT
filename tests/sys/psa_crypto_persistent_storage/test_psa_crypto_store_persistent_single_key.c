/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Generate and use a persistently stored key in PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit.h"
#include "psa/crypto.h"
#include "tests_psa_persistent_storage.h"

#define PLAINTEXT_LEN       (32)
#define ENCR_OUTPUT_SIZE    (PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, \
                                                            PSA_ALG_CBC_NO_PADDING, \
                                                            PLAINTEXT_LEN))

static uint8_t PLAINTEXT[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51
};

static psa_key_id_t key_id_1 = 1;
static psa_key_id_t key_id_2 = 2;

static void _test_setup(void)
{
    psa_key_attributes_t attr = psa_key_attributes_init();

    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;

    psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 128);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
    psa_set_key_id(&attr, key_id_1);

    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
                                      (PSA_KEY_LIFETIME_PERSISTENT, PSA_KEY_LOCATION_LOCAL_STORAGE);
    psa_set_key_lifetime(&attr, lifetime);

    /* Import persistent key */
    TEST_ASSERT_PSA_SUCCESS(psa_import_key(&attr, KEY_128, AES_128_KEY_SIZE, &key_id_1));

    /* Import second key to overwrite the first one */
    psa_set_key_id(&attr, key_id_2);
    TEST_ASSERT_PSA_SUCCESS(psa_import_key(&attr, OVERWRITE_KEY_128, AES_128_KEY_SIZE,
                                                                            &key_id_2));
}

static void _test_destroy_keys(void)
{
    /* Destroy first key */
    TEST_ASSERT_PSA_SUCCESS(psa_destroy_key(key_id_1));

    /* Destroy second key */
    TEST_ASSERT_PSA_SUCCESS(psa_destroy_key(key_id_2));
}

/**
 * @brief   A persistently stored key should still be accessible after
 *          overwriting it in local memory
 */
static void test_psa_store_single_persistent_key(void)
{
    uint8_t cipher_out[ENCR_OUTPUT_SIZE];
    uint8_t plain_out[PLAINTEXT_LEN];
    size_t output_len = 0;

    _test_setup();

    /* Generate cipher with first key */
    TEST_ASSERT_PSA_SUCCESS(psa_cipher_encrypt(key_id_1, PSA_ALG_CBC_NO_PADDING, PLAINTEXT,
                                PLAINTEXT_LEN, cipher_out, ENCR_OUTPUT_SIZE, &output_len));

    TEST_ASSERT_PSA_SUCCESS(psa_cipher_decrypt(key_id_1, PSA_ALG_CBC_NO_PADDING, cipher_out,
                                sizeof(cipher_out), plain_out, sizeof(plain_out), &output_len));
    TEST_ASSERT_MESSAGE(0 == memcmp(plain_out, PLAINTEXT, PLAINTEXT_LEN),
                                                                "first key, wrong plaintext");

    /* Generate cipher with second key */
    TEST_ASSERT_PSA_SUCCESS(psa_cipher_encrypt(key_id_2, PSA_ALG_CBC_NO_PADDING, PLAINTEXT,
                                PLAINTEXT_LEN, cipher_out, ENCR_OUTPUT_SIZE, &output_len));

    TEST_ASSERT_PSA_SUCCESS(psa_cipher_decrypt(key_id_2, PSA_ALG_CBC_NO_PADDING, cipher_out,
                                sizeof(cipher_out), plain_out, sizeof(plain_out), &output_len));
    TEST_ASSERT_MESSAGE(0 == memcmp(plain_out, PLAINTEXT, PLAINTEXT_LEN),
                                                                "second key, wrong plaintext");

    _test_destroy_keys();
}

/**
 * @brief   After destroying a persistent key, it should not be available anymore
 */
static void test_psa_delete_single_persistent_key(void)
{
    uint8_t cipher_out[ENCR_OUTPUT_SIZE];
    size_t output_len = 0;

    _test_setup();

    /* Generate cipher with first key */
    TEST_ASSERT_PSA_SUCCESS(psa_cipher_encrypt(key_id_1, PSA_ALG_CBC_NO_PADDING, PLAINTEXT,
                                PLAINTEXT_LEN, cipher_out, ENCR_OUTPUT_SIZE, &output_len));
    /* Generate cipher with second key */
    TEST_ASSERT_PSA_SUCCESS(psa_cipher_encrypt(key_id_2, PSA_ALG_CBC_NO_PADDING, PLAINTEXT,
                                PLAINTEXT_LEN, cipher_out, ENCR_OUTPUT_SIZE, &output_len));

    _test_destroy_keys();

    /* Encryption with deleted keys should fail */
    TEST_ASSERT_PSA_DOES_NOT_EXIST(psa_cipher_encrypt(key_id_1, PSA_ALG_CBC_NO_PADDING, PLAINTEXT,
                                PLAINTEXT_LEN, cipher_out, ENCR_OUTPUT_SIZE, &output_len));
    TEST_ASSERT_PSA_DOES_NOT_EXIST(psa_cipher_encrypt(key_id_2, PSA_ALG_CBC_NO_PADDING,
                                    PLAINTEXT, PLAINTEXT_LEN, cipher_out, ENCR_OUTPUT_SIZE,
                                    &output_len));
}

Test* tests_psa_persistent_single_key_storage(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_psa_store_single_persistent_key),
        new_TestFixture(test_psa_delete_single_persistent_key),
    };

    EMB_UNIT_TESTCALLER(tests_psa_persistent_single_key_storage_tests, NULL, NULL, fixtures);

    return (Test *)&tests_psa_persistent_single_key_storage_tests;
}
