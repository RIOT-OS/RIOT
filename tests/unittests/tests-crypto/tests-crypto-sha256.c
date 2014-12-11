/*
 * Copyright (C) 2014 Philipp Rosenkranz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "embUnit/embUnit.h"

#include "crypto/sha256.h"

#include "tests-crypto.h"

static int calc_and_compare_hash(const char *str, const char *expected)
{
    static unsigned char hash[SHA256_DIGEST_LENGTH];
    sha256_context_t sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, str, strlen(str));
    sha256_final(hash, &sha256);

    return strncmp((const char *) hash, expected, SHA256_DIGEST_LENGTH);
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
