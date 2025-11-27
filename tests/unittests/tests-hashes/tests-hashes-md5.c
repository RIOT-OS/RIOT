/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Test cases for the MD5 hash implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "hashes/md5.h"

#define MD5_DIGEST_LENGTH           (16U)

static int calc_and_compare_hash(const char *str, const char *expected)
{
    uint8_t hash[MD5_DIGEST_LENGTH];
    char tmp[(2 * MD5_DIGEST_LENGTH) + 1];

    /* calculate hash */
    md5(hash, (const uint8_t *)str, strlen(str));
    /* copy hash to string */
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(&(tmp[i * 2]), "%02x", hash[i]);
    }
    tmp[MD5_DIGEST_LENGTH * 2] = '\0';
    /* compare with result string */
    return strncmp(tmp, expected, strlen(tmp));
}

/* test cases copied from appendix A.5 of RFC 1321 */
static void test_hashes_md5(void)
{
    TEST_ASSERT(calc_and_compare_hash("",
                "d41d8cd98f00b204e9800998ecf8427e") == 0);
    TEST_ASSERT(calc_and_compare_hash("a",
                "0cc175b9c0f1b6a831c399e269772661") == 0);
    TEST_ASSERT(calc_and_compare_hash("abc",
                "900150983cd24fb0d6963f7d28e17f72") == 0);
    TEST_ASSERT(calc_and_compare_hash("message digest",
                "f96b697d7cb7938d525a2f31aaf161d0") == 0);
    TEST_ASSERT(calc_and_compare_hash("abcdefghijklmnopqrstuvwxyz",
                "c3fcd3d76192e4007dfb496cca67e13b") == 0);
    TEST_ASSERT(calc_and_compare_hash(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
                "d174ab98d277d9f5a5611c2c9f419d9f") == 0);
    TEST_ASSERT(calc_and_compare_hash(
                "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
                "57edf4a22be3c955ac49da2e2107b67a") == 0);
}

Test *tests_hashes_md5_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_md5),
    };

    EMB_UNIT_TESTCALLER(test_hashes_md5, NULL, NULL, fixtures);

    return (Test *)&test_hashes_md5;
}
