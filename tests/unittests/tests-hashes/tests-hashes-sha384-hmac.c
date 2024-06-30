/*
 * Copyright (C) 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * Copyright 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "psa/crypto.h"

#include "embUnit/embUnit.h"

#include "hashes/sha384.h"

#include "tests-hashes.h"
#include <psa_crypto/psa/crypto_types.h>
#include <psa_crypto/psa/crypto_struct.h>

static int compare_str_vs_digest(const char *str,
                                 const uint8_t hash[SHA384_DIGEST_LENGTH])
{
    char ch[3] = {0, 0, 0};
    size_t iter_hash = 0;
    size_t str_length = strlen((char *)str);
    for (size_t i = 0; i < str_length; i += 2)
    {
        ch[0] = str[i];
        ch[1] = str[i + 1];

        if (hash[iter_hash++] != strtol(ch, NULL, 16))
        {
            return 0;
        }
    }
    return 1;
}

static void test_hmac_sha384(const unsigned char *key,
                             size_t key_len, const unsigned char *message, size_t message_len,
                             void *hmac)
{

    // init the PSA crypto library
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS)
    {
        printf("psa_crypto_init failed with status: %d\n", status);
        return;
    }

    psa_mac_operation_t operation = psa_mac_operation_init();
    psa_key_attributes_t attributes = psa_key_attributes_init();
    psa_key_id_t key_handle = 0;

    // setting key attributes
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_384));
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(key_len));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);

    // importing the key
    status = psa_import_key(&attributes, key, key_len, &key_handle);
    if (status != PSA_SUCCESS)
    {
        printf("psa_import_key failed with status: %d\n", status);
        return;
    }

    // hmac init
    status = psa_mac_sign_setup(&operation, key_handle, PSA_ALG_HMAC(PSA_ALG_SHA_384));
    if (status != PSA_SUCCESS)
    {
        printf("psa_mac_sign_setup failed with status: %d\n", status);
        return;
    }

    status = psa_mac_update(&operation, message, message_len);
    if (status != PSA_SUCCESS)
    {
        printf("psa_mac_update failed with status: %d\n", status);
        return;
    }
    size_t hmac_length;
    status = psa_mac_sign_finish(&operation, hmac, SHA384_DIGEST_LENGTH, &hmac_length);
    if (status != PSA_SUCCESS)
    {
        printf("psa_mac_sign_finish failed with status: %d\n", status);
    }

    // destroy the key in the key slot if it exists
    status = psa_destroy_key(key_handle);
    if (status != PSA_SUCCESS && status != PSA_ERROR_DOES_NOT_EXIST)
    {
        printf("psa_destroy_key failed with status: %d\n", status);
        return;
    }
}

static void test_hashes_hmac_sha384_hash_sequence(void)
{
    unsigned char key[64];
    /* prepare an empty key */
    memset((void *)key, 0x0, 64);
    static uint8_t hmac[SHA384_DIGEST_LENGTH];

    /* use an empty message */
    const unsigned char *m = (unsigned char *)"";
    size_t m_len = strlen((char *)m);
    test_hmac_sha384(key, sizeof(key), m, m_len, hmac);

    TEST_ASSERT(compare_str_vs_digest(
        "6c1f2ee938fad2e24bd91298474382ca218c75db3d83e114b3d4367776d14d3551289e75e8209cd4b792302840234adc", hmac));

    /* use a real message */
    static const unsigned char str[] = "The quick brown fox jumps over the lazy dog";
    key[0] = 'k';
    key[1] = 'e';
    key[2] = 'y';

    test_hmac_sha384(key, sizeof(key), str, strlen((char *)str), hmac);

    TEST_ASSERT(compare_str_vs_digest(
        "d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237", hmac));
}

/*
        The following testcases are taken from:
        https://tools.ietf.org/html/rfc4868#section-2.7.1
*/

static void test_hashes_hmac_sha384_hash_PRF1(void)
{
    /* Test Case PRF-1: */
    static const unsigned char strPRF1[] = "Hi There";
    unsigned char key[20];
    static unsigned char hmac[SHA384_DIGEST_LENGTH];
    memset(key, 0x0b, sizeof(key));

    test_hmac_sha384(key, sizeof(key), strPRF1, strlen((char *)strPRF1), hmac);

    TEST_ASSERT(compare_str_vs_digest(
        "afd03944d84895626b0825f4ab46907f15f9dadbe4101ec682aa034c7cebc59cfaea9ea9076ede7f4af152e8b2fa9cb6", hmac));
}

static void test_hashes_hmac_sha384_hash_PRF2(void)
{
    /* Test Case PRF-2: */
    static const unsigned char strPRF2[] = "what do ya want for nothing?";
    unsigned char key[4] = {'J', 'e', 'f', 'e'};
    static unsigned char hmac[SHA384_DIGEST_LENGTH];

    test_hmac_sha384(key, sizeof(key), strPRF2, strlen((char *)strPRF2), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e8e2240ca5e69e2c78b3239ecfab21649", hmac));
}

static void test_hashes_hmac_sha384_hash_PRF3(void)
{
    /* Test Case PRF-3: */
    char unsigned strPRF3[50];
    unsigned char key[20];
    static unsigned char hmac[SHA384_DIGEST_LENGTH];

    memset(strPRF3, 0xdd, sizeof(strPRF3));
    memset(key, 0xaa, sizeof(key));

    test_hmac_sha384(key, sizeof(key), strPRF3, sizeof(strPRF3), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "88062608d3e6ad8a0aa2ace014c8a86f0aa635d947ac9febe83ef4e55966144b2a5ab39dc13814b94e3ab6e101a34f27", hmac));
}

static void test_hashes_hmac_sha384_hash_PRF4(void)
{
    /* Test Case PRF-4: */
    char unsigned strPRF4[50];
    unsigned char key[25];
    static unsigned char hmac[SHA384_DIGEST_LENGTH];

    memset(strPRF4, 0xcd, sizeof(strPRF4));
    /*
     * set key to: 0102030405060708090a0b0c0d0e0f10111213141516171819
     */
    for (size_t i = 0; i < sizeof(key); ++i)
    {
        key[i] = i + 1;
    }

    test_hmac_sha384(key, sizeof(key), strPRF4, sizeof(strPRF4), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "3e8a69b7783c25851933ab6290af6ca77a9981480850009cc5577c6e1f573b4e6801dd23c4a7d679ccf8a386c674cffb", hmac));
}

static void test_hashes_hmac_sha384_hash_PRF5(void)
{
    /* Test Case PRF-5: */
    static const unsigned char strPRF5[] = "Test Using Larger Than Block-Size Key - Hash Key First";
    unsigned char longKey[131];
    unsigned char hashedKey[SHA384_DIGEST_LENGTH];
    static unsigned char hmac[SHA384_DIGEST_LENGTH];
    memset(longKey, 0xaa, sizeof(longKey));

    // hash the key before parsing
    sha384(longKey, sizeof(longKey), hashedKey);

    test_hmac_sha384(hashedKey, SHA384_DIGEST_LENGTH, strPRF5, strlen((char *)strPRF5), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "4ece084485813e9088d2c63a041bc5b44f9ef1012a2b588f3cd11f05033ac4c60c2ef6ab4030fe8296248df163f44952", hmac));
}

static void test_hashes_hmac_sha384_hash_PRF6(void)
{
    /* Test Case PRF-6: */
    static const unsigned char strPRF6[] =
        "This is a test using a larger than block-size key and a "
        "larger than block-size data. The key needs to be hashed "
        "before being used by the HMAC algorithm.";

    unsigned char longKey[131];
    unsigned char hashedKey[SHA384_DIGEST_LENGTH];
    static unsigned char hmac[SHA384_DIGEST_LENGTH];
    memset(longKey, 0xaa, sizeof(longKey));

    // hash the key before parsing
    sha384(longKey, sizeof(longKey), hashedKey);

    /* the same key is used as above: 131 x 0xa */
    test_hmac_sha384(hashedKey, SHA384_DIGEST_LENGTH, strPRF6, strlen((char *)strPRF6), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "6617178e941f020d351e2f254e8fd32c602420feb0b8fb9adccebb82461e99c5a678cc31e799176d3860e6110c46523e", hmac));
}

Test *tests_hashes_sha384_hmac_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_hashes_hmac_sha384_hash_sequence),
        new_TestFixture(test_hashes_hmac_sha384_hash_PRF1),
        new_TestFixture(test_hashes_hmac_sha384_hash_PRF2),
        new_TestFixture(test_hashes_hmac_sha384_hash_PRF3),
        new_TestFixture(test_hashes_hmac_sha384_hash_PRF4),
        new_TestFixture(test_hashes_hmac_sha384_hash_PRF5),
        new_TestFixture(test_hashes_hmac_sha384_hash_PRF6),
    };

    EMB_UNIT_TESTCALLER(hashes_sha384_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha384_tests;
}
