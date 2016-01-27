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

#include "crypto/sha256.h"

#include "tests-crypto.h"

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

static void test_crypto_sha256_hash_sequence(void)
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

Test *tests_crypto_sha256_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_sha256_hash_sequence),
    };

    EMB_UNIT_TESTCALLER(crypto_sha256_tests, NULL, NULL,
                        fixtures);

    return (Test *)&crypto_sha256_tests;
}
