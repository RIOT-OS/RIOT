/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       testcases for the sha224 implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"

#include "hashes/sha224.h"

#include "tests-hashes.h"

/**
 * @brief expected hash for test 01
 * i.e. 3eda9ffe5537a588f54d0b2a453e5fa932986d0bc0f9556924f5c2379b2c91b0
 *
 * converted using:
 * s=$(echo '<hash string>' | sed -e 's/../0x&, /g' | sed 's/, $//'); echo {$s}\;
 *
 * where <hash string> is the above sequence of characters 3e...b0
 */

/**
 * @brief expected hash for test empty
 * i.e. d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f
 */
static const unsigned char hempty[] =
                                   {0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9, 0x47,
                                    0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4, 0x15, 0xa2,
                                    0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a, 0xc5, 0xb3, 0xe4,
                                    0x2f};

/**
 * @brief expected hash for "abc"
 *        (from FIPS 180-2 Appendix B.1)
 */
static const unsigned char h_abc[] =
                                   {0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22, 0x86,
                                    0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3, 0x2a, 0xad,
                                    0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7, 0xe3, 0x6c, 0x9d,
                                    0xa7};
/**
 * @brief expected hash for "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *        (from FIPS 180-2 Appendix B.2)
 */
static const unsigned char h_abc_long[] =
                                    {0x75, 0x38, 0x8b, 0x16, 0x51, 0x27, 0x76, 0xcc, 0x5d,
                                     0xba, 0x5d, 0xa1, 0xfd, 0x89, 0x01, 0x50, 0xb0, 0xc6,
                                     0x45, 0x5c, 0xb4, 0xf5, 0x8b, 0x19, 0x52, 0x52, 0x25,
                                     0x25};

/**
 * @brief expected hash for test long_sequence
 * i.e. 34674aab568a93190425d9f9d7e96ff835635630e3d2638218a9103b
 */
static const unsigned char hlong_sequence[] =
                                     {0x34, 0x67, 0x4a, 0xab, 0x56, 0x8a, 0x93, 0x19, 0x04,
                                      0x25, 0xd9, 0xf9, 0xd7, 0xe9, 0x6f, 0xf8, 0x35, 0x63,
                                      0x56, 0x30, 0xe3, 0xd2, 0x63, 0x82, 0x18, 0xa9, 0x10,
                                      0x3b};

static int calc_and_compare_hash(const char *str, const unsigned char *expected)
{
    static unsigned char hash[28];
    sha224_context_t sha224;

    sha224_init(&sha224);
    sha224_update(&sha224, (uint8_t*)str, strlen(str));
    sha224_final(&sha224, hash);

    return (memcmp(expected, hash, 28) == 0);
}

static int calc_and_compare_hash_wrapper(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA224_DIGEST_LENGTH];

    sha224((uint8_t*)str, strlen(str), hash);

    return (memcmp(expected, hash, SHA224_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha224_hash_sequence_empty(void)
{
    const char *teststring = "";
    TEST_ASSERT(calc_and_compare_hash(teststring, hempty));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, hempty));
}

static void test_hashes_sha224_hash_sequence_abc(void)
{
    char *teststring = "abc";
    TEST_ASSERT(calc_and_compare_hash(teststring, h_abc));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h_abc));
}

static void test_hashes_sha224_hash_sequence_abc_long(void)
{
    char *teststring = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    TEST_ASSERT(calc_and_compare_hash(teststring, h_abc_long));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h_abc_long));
}

static void test_hashes_sha256_hash_long_sequence(void)
{
    static const char *teststring =
        {"RIOT is an open-source microkernel-based operating system, designed"
         " to match the requirements of Internet of Things (IoT) devices and"
         " other embedded devices. These requirements include a very low memory"
         " footprint (on the order of a few kilobytes), high energy efficiency"
         ", real-time capabilities, communication stacks for both wireless and"
         " wired networks, and support for a wide range of low-power hardware."};
    TEST_ASSERT(calc_and_compare_hash(teststring, hlong_sequence));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, hlong_sequence));
}

Test *tests_hashes_sha224_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha224_hash_sequence_empty),
        new_TestFixture(test_hashes_sha224_hash_sequence_abc),
        new_TestFixture(test_hashes_sha224_hash_sequence_abc_long),
        new_TestFixture(test_hashes_sha256_hash_long_sequence),
    };

    EMB_UNIT_TESTCALLER(hashes_sha224_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha224_tests;
}
