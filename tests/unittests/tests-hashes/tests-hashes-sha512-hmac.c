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

#include "hashes/sha512.h"

#include "tests-hashes.h"
#include <psa_crypto/psa/crypto_types.h>
#include <psa_crypto/psa/crypto_struct.h>

static int compare_str_vs_digest(const char *str,
                                 const uint8_t hash[SHA512_DIGEST_LENGTH])
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

static void test_hmac_sha512(const unsigned char *key,
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

    // uint8_t computed_hmac[SHA512_DIGEST_LENGTH];
    psa_mac_operation_t operation = psa_mac_operation_init();
    psa_key_attributes_t attributes = psa_key_attributes_init();
    psa_key_id_t key_handle = 0;

    // setting key attributes
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_512));
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
    status = psa_mac_sign_setup(&operation, key_handle, PSA_ALG_HMAC(PSA_ALG_SHA_512));
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
    status = psa_mac_sign_finish(&operation, hmac, SHA512_DIGEST_LENGTH, &hmac_length);
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

static void test_hashes_hmac_sha512_hash_sequence(void)
{
    unsigned char key[64];
    /* prepare an empty key */
    memset((void *)key, 0x0, 64);
    static uint8_t hmac[SHA512_DIGEST_LENGTH];

    /* use an empty message */
    const unsigned char *m = (unsigned char *)"";
    size_t m_len = strlen((char *)m);
    test_hmac_sha512(key, sizeof(key), m, m_len, hmac);

    TEST_ASSERT(compare_str_vs_digest(
        "b936cee86c9f87aa5d3c6f2e84cb5a4239a5fe50480a6ec66b70ab5b1f4ac6730c6c515421b327ec1d69402e53dfb49ad7381eb067b338fd7b0cb22247225d47", hmac));

    /* use a real message */
    static const unsigned char str[] = "The quick brown fox jumps over the lazy dog";
    key[0] = 'k';
    key[1] = 'e';
    key[2] = 'y';

    test_hmac_sha512(key, sizeof(key), str, strlen((char *)str), hmac);

    TEST_ASSERT(compare_str_vs_digest(
        "b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a", hmac));
}

/*
        The following testcases are taken from:
        https://tools.ietf.org/html/rfc4868#section-2.7.1
*/

static void test_hashes_hmac_sha512_hash_PRF1(void)
{
    /* Test Case PRF-1: */
    static const unsigned char strPRF1[] = "Hi There";
    unsigned char key[20];
    static unsigned char hmac[SHA512_DIGEST_LENGTH];
    memset(key, 0x0b, sizeof(key));

    test_hmac_sha512(key, sizeof(key), strPRF1, strlen((char *)strPRF1), hmac);

    TEST_ASSERT(compare_str_vs_digest(
        "87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cdedaa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854", hmac));
}

static void test_hashes_hmac_sha512_hash_PRF2(void)
{
    /* Test Case PRF-2: */
    static const unsigned char strPRF2[] = "what do ya want for nothing?";
    unsigned char key[4] = {'J', 'e', 'f', 'e'};
    static unsigned char hmac[SHA512_DIGEST_LENGTH];

    test_hmac_sha512(key, sizeof(key), strPRF2, strlen((char *)strPRF2), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737", hmac));
}

static void test_hashes_hmac_sha512_hash_PRF3(void)
{
    /* Test Case PRF-3: */
    char unsigned strPRF3[50];
    unsigned char key[20];
    static unsigned char hmac[SHA512_DIGEST_LENGTH];

    memset(strPRF3, 0xdd, sizeof(strPRF3));
    memset(key, 0xaa, sizeof(key));

    test_hmac_sha512(key, sizeof(key), strPRF3, sizeof(strPRF3), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb", hmac));
}

static void test_hashes_hmac_sha512_hash_PRF4(void)
{
    /* Test Case PRF-4: */
    char unsigned strPRF4[50];
    unsigned char key[25];
    static unsigned char hmac[SHA512_DIGEST_LENGTH];

    memset(strPRF4, 0xcd, sizeof(strPRF4));
    /*
     * set key to: 0102030405060708090a0b0c0d0e0f10111213141516171819
     */
    for (size_t i = 0; i < sizeof(key); ++i)
    {
        key[i] = i + 1;
    }

    test_hmac_sha512(key, sizeof(key), strPRF4, sizeof(strPRF4), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd", hmac));
}

static void test_hashes_hmac_sha512_hash_PRF5(void)
{
    /* Test Case PRF-5: */
    static const unsigned char strPRF5[] = "Test Using Larger Than Block-Size Key - Hash Key First";
    unsigned char longKey[131];
    unsigned char hashedKey[SHA512_DIGEST_LENGTH];
    static unsigned char hmac[SHA512_DIGEST_LENGTH];
    memset(longKey, 0xaa, sizeof(longKey));

    // hash the key before parsing
    sha512(longKey, sizeof(longKey), hashedKey);

    test_hmac_sha512(hashedKey, SHA512_DIGEST_LENGTH, strPRF5, strlen((char *)strPRF5), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f3526b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598", hmac));
}

static void test_hashes_hmac_sha512_hash_PRF6(void)
{
    /* Test Case PRF-6: */
    static const unsigned char strPRF6[] =
        "This is a test using a larger than block-size key and a "
        "larger than block-size data. The key needs to be hashed "
        "before being used by the HMAC algorithm.";

    unsigned char longKey[131];
    unsigned char hashedKey[SHA512_DIGEST_LENGTH];
    static unsigned char hmac[SHA512_DIGEST_LENGTH];
    memset(longKey, 0xaa, sizeof(longKey));

    // hash the key before parsing
    sha512(longKey, sizeof(longKey), hashedKey);

    /* the same key is used as above: 131 x 0xa */
    test_hmac_sha512(hashedKey, SHA512_DIGEST_LENGTH, strPRF6, strlen((char *)strPRF6), hmac);
    TEST_ASSERT(compare_str_vs_digest(
        "e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58", hmac));
}

Test *tests_hashes_sha512_hmac_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_hashes_hmac_sha512_hash_sequence),
        new_TestFixture(test_hashes_hmac_sha512_hash_PRF1),
        new_TestFixture(test_hashes_hmac_sha512_hash_PRF2),
        new_TestFixture(test_hashes_hmac_sha512_hash_PRF3),
        new_TestFixture(test_hashes_hmac_sha512_hash_PRF4),
        new_TestFixture(test_hashes_hmac_sha512_hash_PRF5),
        new_TestFixture(test_hashes_hmac_sha512_hash_PRF6),
    };

    EMB_UNIT_TESTCALLER(hashes_sha512_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha512_tests;
}
