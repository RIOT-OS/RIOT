/*
 * Copyright (C) 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"

#include "hashes/sha256.h"

#include "tests-hashes.h"

static int compare_str_vs_digest(const char *str,
                                 const unsigned char hash[SHA256_DIGEST_LENGTH])
{
    char ch[3] = { 0, 0, 0 };
    size_t iter_hash = 0;
    size_t str_length = strlen(str);
    for (size_t i = 0; i < str_length; i += 2) {
        ch[0] = str[i];
        ch[1] = str[i + 1];

        if (hash[iter_hash++] != strtol(ch, NULL, 16)) {
            return 0;
        }
    }
    return 1;
}

static void test_hashes_hmac_sha256_hash_sequence(void)
{
    unsigned char key[64];
    /* prepare an empty key */
    memset((void*)key, 0x0, 64);
    static unsigned char hmac[SHA256_DIGEST_LENGTH];

    /* use an empty message */
    const unsigned *m = NULL;
    hmac_sha256(key, sizeof(key), m, 0, hmac);

    TEST_ASSERT(compare_str_vs_digest(
                 "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad", hmac));

    /* use a real message */
    const char str[] = "The quick brown fox jumps over the lazy dog";
    key[0] = 'k';
    key[1] = 'e';
    key[2] = 'y';

    hmac_sha256(key, sizeof(key), (unsigned*)str, strlen(str), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8", hmac));
}

/*
        The followig testcases are taken from:
        https://tools.ietf.org/html/rfc4868#section-2.7.1
*/

static void test_hashes_hmac_sha256_hash_PRF1(void)
{
    /* Test Case PRF-1: */
    const char strPRF1[] = "Hi There";
    unsigned char key[20];
    static unsigned char hmac[SHA256_DIGEST_LENGTH];
    memset(key, 0x0b, sizeof(key));

    hmac_sha256(key, sizeof(key), (unsigned*)strPRF1, strlen(strPRF1), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7", hmac));
}

static void test_hashes_hmac_sha256_hash_PRF2(void)
{
    /* Test Case PRF-2: */
    const char strPRF2[] = "what do ya want for nothing?";
    unsigned char key[4] = {'J', 'e', 'f', 'e'};
    static unsigned char hmac[SHA256_DIGEST_LENGTH];

    hmac_sha256(key, sizeof(key), (unsigned*)strPRF2, strlen(strPRF2), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843", hmac));
}

static void test_hashes_hmac_sha256_hash_PRF3(void)
{
    /* Test Case PRF-3: */
    char strPRF3[50];
    unsigned char key[20];
    static unsigned char hmac[SHA256_DIGEST_LENGTH];

    memset(strPRF3, 0xdd, sizeof(strPRF3));
    memset(key, 0xaa, sizeof(key));

    hmac_sha256(key, sizeof(key), (unsigned*)strPRF3, sizeof(strPRF3), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe", hmac));
}

static void test_hashes_hmac_sha256_hash_PRF4(void)
{
    /* Test Case PRF-4: */
    char strPRF4[50];
    unsigned char key[25];
    static unsigned char hmac[SHA256_DIGEST_LENGTH];

    memset(strPRF4, 0xcd, sizeof(strPRF4));
    /*
    * set key to: 0102030405060708090a0b0c0d0e0f10111213141516171819
    */
    for (size_t i = 0; i < sizeof(key); ++i) {
        key[i] = i+1;
    }

    hmac_sha256(key, sizeof(key), (unsigned*)strPRF4, sizeof(strPRF4), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b", hmac));
}

static void test_hashes_hmac_sha256_hash_PRF5(void)
{
    /* Test Case PRF-5: */
    const char strPRF5[] = "Test Using Larger Than Block-Size Key - Hash Key First";
    unsigned char longKey[131];
    static unsigned char hmac[SHA256_DIGEST_LENGTH];
    memset(longKey, 0xaa, sizeof(longKey));

    hmac_sha256(longKey, sizeof(longKey), (unsigned*)strPRF5, strlen(strPRF5), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54", hmac));
}

static void test_hashes_hmac_sha256_hash_PRF6(void)
{
    /* Test Case PRF-6: */
    const char strPRF6[] = "This is a test using a larger than block-size key and a "
                           "larger than block-size data. The key needs to be hashed "
                           "before being used by the HMAC algorithm.";
    unsigned char longKey[131];
    static unsigned char hmac[SHA256_DIGEST_LENGTH];
    memset(longKey, 0xaa, sizeof(longKey));

    /* the same key is used as above: 131 x 0xa */
    hmac_sha256(longKey, sizeof(longKey), (unsigned*)strPRF6, strlen(strPRF6), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2", hmac));
}

Test *tests_hashes_sha256_hmac_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_hmac_sha256_hash_sequence),
        new_TestFixture(test_hashes_hmac_sha256_hash_PRF1),
        new_TestFixture(test_hashes_hmac_sha256_hash_PRF2),
        new_TestFixture(test_hashes_hmac_sha256_hash_PRF3),
        new_TestFixture(test_hashes_hmac_sha256_hash_PRF4),
        new_TestFixture(test_hashes_hmac_sha256_hash_PRF5),
        new_TestFixture(test_hashes_hmac_sha256_hash_PRF6),
    };

    EMB_UNIT_TESTCALLER(hashes_sha256_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha256_tests;
}
