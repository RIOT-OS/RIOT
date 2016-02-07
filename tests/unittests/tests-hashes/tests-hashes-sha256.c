/*
 * Copyright (C) 2014 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * Copyright (C) 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
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

static int calc_and_compare_hash(const char *str, const char *expected)
{
    static unsigned char hash[SHA256_DIGEST_LENGTH];
    sha256_context_t sha256;

    sha256_init(&sha256);
    sha256_update(&sha256, str, strlen(str));
    sha256_final(hash, &sha256);

    return compare_str_vs_digest(expected, hash);
}

static void test_hashes_sha256_hash_sequence(void)
{
    TEST_ASSERT(calc_and_compare_hash("1234567890_1",
                                      "3eda9ffe5537a588f54d0b2a453e5fa932986d0bc0f9556924f5c2379b2c91b0"));
    TEST_ASSERT(calc_and_compare_hash("1234567890_2",
                                      "a144d0b4d285260ebbbab6840baceaa09eab3e157443c9458de764b7262c8ace"));
    TEST_ASSERT(calc_and_compare_hash("1234567890_3",
                                      "9f839169d293276d1b799707d2171ac1fd5b78d0f3bc7693dbed831524dd2d77"));
    TEST_ASSERT(calc_and_compare_hash("1234567890_4",
                                      "6c5fe2a8e3de58a5e5ac061031a8e802ae1fb9e7197862ec1aedf236f0e23475"));
    TEST_ASSERT(calc_and_compare_hash(
                    "0123456789abcde-0123456789abcde-0123456789abcde-0123456789abcde-",
                    "945ab9d52b069923680c2c067fa6092cbbd9234cf7a38628f3033b2d54d3d3bf"));
    TEST_ASSERT(calc_and_compare_hash(
                    "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern",
                    "d32b568cd1b96d459e7291ebf4b25d007f275c9f13149beeb782fac0716613f8"));
    TEST_ASSERT(calc_and_compare_hash(
                    "Frank jagt im komplett verwahrlosten Taxi quer durch Bayern",
                    "78206a866dbb2bf017d8e34274aed01a8ce405b69d45db30bafa00f5eeed7d5e"));
    TEST_ASSERT(calc_and_compare_hash("",
                                      "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
    TEST_ASSERT(calc_and_compare_hash(
                    "RIOT is an open-source microkernel-based operating system, designed"
                    " to match the requirements of Internet of Things (IoT) devices and"
                    " other embedded devices. These requirements include a very low memory"
                    " footprint (on the order of a few kilobytes), high energy efficiency"
                    ", real-time capabilities, communication stacks for both wireless and"
                    " wired networks, and support for a wide range of low-power hardware.",
                    "06c84971e2831c48b8293144c762e3236a78217354896185b14a3a84f7cd8066"));
    /* test failing sha256 by switching last byte of expected hash from 3b to 3c */
    TEST_ASSERT(!calc_and_compare_hash("This test fails!",
                                      "c19d3bf8588897076873f1a0a106ba840ca46bd1179d592953acecc4df59593c"));
}


static void test_hashes_hmac_sha256_hash_sequence(void)
{
    unsigned char key[64];
    /* prepare an empty key */
    memset((void*)key, 0x0, 64);
    static unsigned char hmac[SHA256_DIGEST_LENGTH];

    /* use an empty message */
    const unsigned *m = NULL;
    hmac_sha256(key, 64, m, 0, hmac);

    TEST_ASSERT(compare_str_vs_digest(
                 "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad", hmac));

    /* use a real message */
    const char str[] = "The quick brown fox jumps over the lazy dog";
    key[0] = 'k';
    key[1] = 'e';
    key[2] = 'y';

    hmac_sha256(key, 3, (unsigned*)str, strlen(str), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8", hmac));

    /*
        The followig testcases are taken from:
        https://tools.ietf.org/html/rfc4868#section-2.7.1
    */

    /* Test Case PRF-1: */
    const char strPRF1[] = "Hi There";
    memset(key, 0x0b, 20);

    hmac_sha256(key, 20, (unsigned*)strPRF1, strlen(strPRF1), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7", hmac));

    /* Test Case PRF-2: */
    const char strPRF2[] = "what do ya want for nothing?";
    /* clear the key (we used 20 bytes so we clear only 20) */
    memset(key, 0x0, 20);
    key[0] = 'J';
    key[1] = 'e';
    key[2] = 'f';
    key[3] = 'e';

    hmac_sha256(key, 4, (unsigned*)strPRF2, strlen(strPRF2), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843", hmac));

    /* Test Case PRF-3: */
    char strPRF3[50];
    memset(strPRF3, 0xdd, 50);
    memset(key, 0xaa, 20);

    hmac_sha256(key, 20, (unsigned*)strPRF3, 50, hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe", hmac));

    /* Test Case PRF-4: */
    char strPRF4[50];
    memset(strPRF4, 0xcd, 50);
    /* clear the key (we used 20 bytes so we clear only 20) */
    memset(key, 0x0, 20);
    /*
    * set key to: 0102030405060708090a0b0c0d0e0f10111213141516171819
    */
    for (size_t i = 0; i < 25; ++i) {
        key[i] = i+1;
    }

    hmac_sha256(key, 25, (unsigned*)strPRF4, 50, hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b", hmac));

    /* Test Case PRF-5: */
    const char strPRF5[] = "Test Using Larger Than Block-Size Key - Hash Key First";
    unsigned char longKey[131];
    memset(longKey, 0xaa, 131);

    hmac_sha256(longKey, 131, (unsigned*)strPRF5, strlen(strPRF5), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54", hmac));

    /* Test Case PRF-6: */
    const char strPRF6[] = "This is a test using a larger than block-size key and a "
                           "larger than block-size data. The key needs to be hashed "
                           "before being used by the HMAC algorithm.";
    /* the same key is used as above: 131 x 0xa */
    hmac_sha256(longKey, 131, (unsigned*)strPRF6, strlen(strPRF6), hmac);
    TEST_ASSERT(compare_str_vs_digest(
                 "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2", hmac));
}

Test *tests_hashes_sha256_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha256_hash_sequence),
        new_TestFixture(test_hashes_hmac_sha256_hash_sequence),
    };

    EMB_UNIT_TESTCALLER(hashes_sha256_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha256_tests;
}
