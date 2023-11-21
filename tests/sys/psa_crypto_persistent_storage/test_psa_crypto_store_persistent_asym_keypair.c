/*
 * Copyright (C) 2023 HAW Hamburg
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
 * @brief       Tests CBOR encoding of a PSA Crypto asymmetric key pair slot.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit.h"
#include "psa/crypto.h"
#include "tests_psa_persistent_storage.h"

#define EDDSA_MESSAGE_SIZE  (127)

#define ECC_KEY_SIZE    (255)
#define ECC_KEY_TYPE (PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS))
#define ECC_ALG (PSA_ALG_PURE_EDDSA)

#define ECC_PUBLIC_KEY_SIZE     (PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE))
#define SIGNATURE_SIZE    (PSA_SIGN_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE, ECC_ALG))

static psa_key_attributes_t privkey_attr;
static psa_key_attributes_t pubkey_attr;

static psa_key_id_t privkey_id = 1;
static psa_key_id_t pubkey_id = 2;
static psa_key_id_t overwrite_privkey_id = 3;

static const uint8_t msg[EDDSA_MESSAGE_SIZE] = { 0x0b };

static void _test_setup(void)
{
    uint8_t public_key[ECC_PUBLIC_KEY_SIZE];
    size_t pubkey_length;

    privkey_attr = psa_key_attributes_init();
    pubkey_attr = psa_key_attributes_init();

    psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE;
    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
                                      (PSA_KEY_LIFETIME_PERSISTENT, PSA_KEY_LOCATION_LOCAL_STORAGE);

    psa_set_key_algorithm(&privkey_attr, ECC_ALG);
    psa_set_key_usage_flags(&privkey_attr, usage);
    psa_set_key_type(&privkey_attr, ECC_KEY_TYPE);
    psa_set_key_bits(&privkey_attr, ECC_KEY_SIZE);
    psa_set_key_id(&privkey_attr, privkey_id);
    psa_set_key_lifetime(&privkey_attr, lifetime);

    /* Generate persistent key */
    TEST_ASSERT_PSA_SUCCESS(psa_generate_key(&privkey_attr, &privkey_id));

    /* Export and import public key for verification */
    TEST_ASSERT_PSA_SUCCESS(psa_export_public_key(privkey_id, public_key,
                                                            sizeof(public_key), &pubkey_length));

    psa_set_key_usage_flags(&pubkey_attr, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&pubkey_attr, ECC_ALG);
    psa_set_key_bits(&pubkey_attr, PSA_BYTES_TO_BITS(pubkey_length));
    psa_set_key_type(&pubkey_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS));

    TEST_ASSERT_PSA_SUCCESS(psa_import_key(&pubkey_attr, public_key, pubkey_length, &pubkey_id));
}

static void _test_destroy_keys(int cleanup)
{
    /* Destroy first key */
    TEST_ASSERT_PSA_SUCCESS(psa_destroy_key(privkey_id));

    /* Destroy second key */
    TEST_ASSERT_PSA_SUCCESS(psa_destroy_key(pubkey_id));

    if (cleanup) {
        /* Destroy last key, just to clean up */
        TEST_ASSERT_PSA_SUCCESS(psa_destroy_key(overwrite_privkey_id));
    }
}

/**
 * @brief   A persistently stored key should still be accessible after
 *          overwriting it in local memory
 */
static void test_psa_store_persistent_asym_keypair(void)
{
    uint8_t signature[SIGNATURE_SIZE];
    size_t sig_length;

    _test_setup();

    /* Generate second keypair to overwrite the first one in volatile memory */
    psa_set_key_id(&privkey_attr, overwrite_privkey_id);
    TEST_ASSERT_PSA_SUCCESS(psa_generate_key(&privkey_attr, &overwrite_privkey_id));

    /* Generate message signature with first key */
    TEST_ASSERT_PSA_SUCCESS(psa_sign_message(privkey_id, ECC_ALG, msg, sizeof(msg), signature, sizeof(signature), &sig_length));

    /* Verify signature with public key of first keypair */
    TEST_ASSERT_PSA_SUCCESS(psa_verify_message(pubkey_id, ECC_ALG, msg,
                                                            sizeof(msg), signature, sig_length));

    _test_destroy_keys(1);
}

/**
 * @brief   After destroying a persistent key, it should not be available anymore
 */
static void test_psa_delete_persistent_asym_keypair(void)
{
    uint8_t signature[SIGNATURE_SIZE];
    size_t sig_length;

    _test_setup();

    /* Generate message signature with first key */
    TEST_ASSERT_PSA_SUCCESS(psa_sign_message(privkey_id, ECC_ALG, msg, sizeof(msg), signature, sizeof(signature), &sig_length));

    /* Verify signature with public key of first keypair */
    TEST_ASSERT_PSA_SUCCESS(psa_verify_message(pubkey_id, ECC_ALG, msg,
                                                            sizeof(msg), signature, sig_length));

    _test_destroy_keys(0);

    /* Signature and verification with deleted keys should fail */
    TEST_ASSERT_PSA_DOES_NOT_EXIST(psa_sign_message(privkey_id, ECC_ALG, msg, sizeof(msg),
                                                        signature, sizeof(signature), &sig_length));
    TEST_ASSERT_PSA_DOES_NOT_EXIST(psa_verify_message(pubkey_id, ECC_ALG, msg, sizeof(msg),
                                                                            signature, sig_length));
}

Test* tests_psa_persistent_asym_keypair_storage(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_psa_store_persistent_asym_keypair),
        new_TestFixture(test_psa_delete_persistent_asym_keypair),
    };

    EMB_UNIT_TESTCALLER(tests_psa_persistent_asym_keypair_storage_tests, NULL, NULL, fixtures);

    return (Test *)&tests_psa_persistent_asym_keypair_storage_tests;
}
