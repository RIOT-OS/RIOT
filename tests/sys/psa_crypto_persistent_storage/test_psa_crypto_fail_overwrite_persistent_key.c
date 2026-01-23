/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Verify that PSA Crypto keys cannot be overwritten in storage
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit.h"
#include "psa/crypto.h"
#include "tests_psa_persistent_storage.h"

static psa_key_id_t key_id = 1;

/**
 * @brief   A persistently stored key must not be overwritten by new key with same ID
 */
static void test_psa_fail_overwrite_existing_key(void)
{
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT;

    psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 128);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
    psa_set_key_id(&attr, key_id);

    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
                                      (PSA_KEY_LIFETIME_PERSISTENT, PSA_KEY_LOCATION_LOCAL_STORAGE);
    psa_set_key_lifetime(&attr, lifetime);

    /* Import persistent key */
    TEST_ASSERT_PSA_SUCCESS(psa_import_key(&attr, KEY_128, AES_128_KEY_SIZE, &key_id));

    /* Import different key with same ID, should fail */
    TEST_ASSERT_PSA_ALREADY_EXISTS(psa_import_key(&attr, OVERWRITE_KEY_128, AES_128_KEY_SIZE, &key_id));
}

/**
 * @brief   After deleting a persistent key, a new key with same ID can be imported
 */
static void test_psa_delete_key_and_import_key_with_same_id(void)
{
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT;

    psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 128);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
    psa_set_key_id(&attr, key_id);

    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
                                      (PSA_KEY_LIFETIME_PERSISTENT, PSA_KEY_LOCATION_LOCAL_STORAGE);
    psa_set_key_lifetime(&attr, lifetime);

    /* Destroy persistent key */
    TEST_ASSERT_PSA_SUCCESS(psa_destroy_key(key_id));

    /* Import different key with same ID, should succeed */
    TEST_ASSERT_PSA_SUCCESS(psa_import_key(&attr, OVERWRITE_KEY_128, AES_128_KEY_SIZE, &key_id));

    /* Destroy persistent key again, to clean up */
    TEST_ASSERT_PSA_SUCCESS(psa_destroy_key(key_id));
}

Test* tests_psa_fail_overwrite_existing_key(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_psa_fail_overwrite_existing_key),
        new_TestFixture(test_psa_delete_key_and_import_key_with_same_id),
    };

    EMB_UNIT_TESTCALLER(tests_psa_fail_overwrite_existing_key_tests, NULL, NULL, fixtures);

    return (Test *)&tests_psa_fail_overwrite_existing_key_tests;
}
