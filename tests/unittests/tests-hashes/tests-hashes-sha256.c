/*
 * SPDX-FileCopyrightText: 2013 Christian Mehlis <mehlis@inf.fu-berlin.de>
 * SPDX-FileCopyrightText: 2014 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * SPDX-FileCopyrightText: 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       testcases for the sha256 implementation
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * @}
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"

#include "hashes/sha256.h"

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
static const unsigned char h01[] = {0x3e, 0xda, 0x9f, 0xfe, 0x55, 0x37, 0xa5, 0x88,
                                    0xf5, 0x4d, 0x0b, 0x2a, 0x45, 0x3e, 0x5f, 0xa9,
                                    0x32, 0x98, 0x6d, 0x0b, 0xc0, 0xf9, 0x55, 0x69,
                                    0x24, 0xf5, 0xc2, 0x37, 0x9b, 0x2c, 0x91, 0xb0};

/**
 * @brief expected hash for test 02
 * i.e. a144d0b4d285260ebbbab6840baceaa09eab3e157443c9458de764b7262c8ace
 */
static const unsigned char h02[] = {0xa1, 0x44, 0xd0, 0xb4, 0xd2, 0x85, 0x26, 0x0e,
                                    0xbb, 0xba, 0xb6, 0x84, 0x0b, 0xac, 0xea, 0xa0,
                                    0x9e, 0xab, 0x3e, 0x15, 0x74, 0x43, 0xc9, 0x45,
                                    0x8d, 0xe7, 0x64, 0xb7, 0x26, 0x2c, 0x8a, 0xce};

/**
 * @brief expected hash for test 03
 * i.e. 9f839169d293276d1b799707d2171ac1fd5b78d0f3bc7693dbed831524dd2d77
 */
static const unsigned char h03[] = {0x9f, 0x83, 0x91, 0x69, 0xd2, 0x93, 0x27, 0x6d,
                                    0x1b, 0x79, 0x97, 0x07, 0xd2, 0x17, 0x1a, 0xc1,
                                    0xfd, 0x5b, 0x78, 0xd0, 0xf3, 0xbc, 0x76, 0x93,
                                    0xdb, 0xed, 0x83, 0x15, 0x24, 0xdd, 0x2d, 0x77};

/**
 * @brief expected hash for test 04
 * i.e. 6c5fe2a8e3de58a5e5ac061031a8e802ae1fb9e7197862ec1aedf236f0e23475
 */
static const unsigned char h04[] = {0x6c, 0x5f, 0xe2, 0xa8, 0xe3, 0xde, 0x58, 0xa5,
                                    0xe5, 0xac, 0x06, 0x10, 0x31, 0xa8, 0xe8, 0x02,
                                    0xae, 0x1f, 0xb9, 0xe7, 0x19, 0x78, 0x62, 0xec,
                                    0x1a, 0xed, 0xf2, 0x36, 0xf0, 0xe2, 0x34, 0x75};

/**
 * @brief expected hash for test digits_letters
 * i.e. 945ab9d52b069923680c2c067fa6092cbbd9234cf7a38628f3033b2d54d3d3bf
 */
static const unsigned char hdigits_letters[] =
                                   {0x94, 0x5a, 0xb9, 0xd5, 0x2b, 0x06, 0x99, 0x23,
                                    0x68, 0x0c, 0x2c, 0x06, 0x7f, 0xa6, 0x09, 0x2c,
                                    0xbb, 0xd9, 0x23, 0x4c, 0xf7, 0xa3, 0x86, 0x28,
                                    0xf3, 0x03, 0x3b, 0x2d, 0x54, 0xd3, 0xd3, 0xbf};

/**
 * @brief expected hash for test pangramm
 * i.e. d32b568cd1b96d459e7291ebf4b25d007f275c9f13149beeb782fac0716613f8
 */
static const unsigned char hpangramm[] =
                                   {0xd3, 0x2b, 0x56, 0x8c, 0xd1, 0xb9, 0x6d, 0x45,
                                    0x9e, 0x72, 0x91, 0xeb, 0xf4, 0xb2, 0x5d, 0x00,
                                    0x7f, 0x27, 0x5c, 0x9f, 0x13, 0x14, 0x9b, 0xee,
                                    0xb7, 0x82, 0xfa, 0xc0, 0x71, 0x66, 0x13, 0xf8};

/**
 * @brief expected hash for test pangramm_no_more
 * i.e. 78206a866dbb2bf017d8e34274aed01a8ce405b69d45db30bafa00f5eeed7d5e
 */
static const unsigned char hpangramm_no_more[] =
                                   {0x78, 0x20, 0x6a, 0x86, 0x6d, 0xbb, 0x2b, 0xf0,
                                    0x17, 0xd8, 0xe3, 0x42, 0x74, 0xae, 0xd0, 0x1a,
                                    0x8c, 0xe4, 0x05, 0xb6, 0x9d, 0x45, 0xdb, 0x30,
                                    0xba, 0xfa, 0x00, 0xf5, 0xee, 0xed, 0x7d, 0x5e};

/**
 * @brief expected hash for test empty
 * i.e. e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
 */
static const unsigned char hempty[] =
                                   {0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
                                    0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
                                    0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
                                    0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};

/**
 * @brief wrong hash for test failing_compare (changed last byte from 3b to 3c)
 * i.e. c19d3bf8588897076873f1a0a106ba840ca46bd1179d592953acecc4df59593c
 */
static const unsigned char hfailing_compare[] =
                                   {0xc1, 0x9d, 0x3b, 0xf8, 0x58, 0x88, 0x97, 0x07,
                                    0x68, 0x73, 0xf1, 0xa0, 0xa1, 0x06, 0xba, 0x84,
                                    0x0c, 0xa4, 0x6b, 0xd1, 0x17, 0x9d, 0x59, 0x29,
                                    0x53, 0xac, 0xec, 0xc4, 0xdf, 0x59, 0x59, 0x3c};

/**
 * @brief expected hash for test long_sequence
 * i.e. 06c84971e2831c48b8293144c762e3236a78217354896185b14a3a84f7cd8066
 */
static const unsigned char hlong_sequence[] =
                                   {0x06, 0xc8, 0x49, 0x71, 0xe2, 0x83, 0x1c, 0x48,
                                    0xb8, 0x29, 0x31, 0x44, 0xc7, 0x62, 0xe3, 0x23,
                                    0x6a, 0x78, 0x21, 0x73, 0x54, 0x89, 0x61, 0x85,
                                    0xb1, 0x4a, 0x3a, 0x84, 0xf7, 0xcd, 0x80, 0x66};

/**
 * @brief expected hash for "abc"
 *        (from FIPS 180-2 Appendix B.1)
 */
static const unsigned char h_fips_oneblock[] =
                                   {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
                                    0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
                                    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
                                    0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad};
/**
 * @brief expected hash for "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
 *        (from FIPS 180-2 Appendix B.2)
 */
static const unsigned char h_fips_multiblock[] =
                                    {0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
                                     0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
                                     0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
                                     0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1};

static int calc_and_compare_hash(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA256_DIGEST_LENGTH];
    sha256_context_t sha256;

    sha256_init(&sha256);
    sha256_update(&sha256, (uint8_t*)str, strlen(str));
    sha256_final(&sha256, hash);

    return (memcmp(expected, hash, SHA256_DIGEST_LENGTH) == 0);
}

static int calc_and_compare_hash_wrapper(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA256_DIGEST_LENGTH];

    sha256((uint8_t*)str, strlen(str), hash);

    return (memcmp(expected, hash, SHA256_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha256_hash_sequence_01(void)
{
    static const char *teststring = "1234567890_1";
    TEST_ASSERT(calc_and_compare_hash(teststring, h01));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h01));
}

static void test_hashes_sha256_hash_sequence_02(void)
{
    static const char *teststring = "1234567890_2";
    TEST_ASSERT(calc_and_compare_hash(teststring, h02));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h02));
}

static void test_hashes_sha256_hash_sequence_03(void)
{
    static const char *teststring = "1234567890_3";
    TEST_ASSERT(calc_and_compare_hash(teststring, h03));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h03));
}

static void test_hashes_sha256_hash_sequence_04(void)
{
    static const char *teststring = "1234567890_4";
    TEST_ASSERT(calc_and_compare_hash(teststring, h04));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h04));
}

static void test_hashes_sha256_hash_sequence_digits_letters(void)
{
    static const char *teststring =
        "0123456789abcde-0123456789abcde-0123456789abcde-0123456789abcde-";
    TEST_ASSERT(calc_and_compare_hash(teststring, hdigits_letters));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, hdigits_letters));
}

static void test_hashes_sha256_hash_sequence_pangramm(void)
{
    static const char *teststring = "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern";
    TEST_ASSERT(calc_and_compare_hash(teststring, hpangramm));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, hpangramm));
}

static void test_hashes_sha256_hash_sequence_pangramm_no_more(void)
{
    /* exchanged `z` with `k` of the first word `Fran[z|k]` */
    static const char *teststring = "Frank jagt im komplett verwahrlosten Taxi quer durch Bayern";
    TEST_ASSERT(calc_and_compare_hash(teststring, hpangramm_no_more));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, hpangramm_no_more));
}

static void test_hashes_sha256_hash_sequence_empty(void)
{
    static const char *teststring = "";
    TEST_ASSERT(calc_and_compare_hash(teststring, hempty));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, hempty));
}

static void test_hashes_sha256_hash_sequence_failing_compare(void)
{
    /* failing compare (sha256 switched last byte of expected hash from `3b` to `3c`) */
    static const char *teststring = "This test fails!";
    TEST_ASSERT(!calc_and_compare_hash(teststring, hfailing_compare));
    TEST_ASSERT(!calc_and_compare_hash_wrapper(teststring, hfailing_compare));
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

static void test_hashes_sha256_hash_sequence_abc(void)
{
    static const char *teststring = "abc";
    TEST_ASSERT(calc_and_compare_hash(teststring, h_fips_oneblock));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h_fips_oneblock));
}

static void test_hashes_sha256_hash_sequence_abc_long(void)
{
    static const char *teststring = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    TEST_ASSERT(calc_and_compare_hash(teststring, h_fips_multiblock));
    TEST_ASSERT(calc_and_compare_hash_wrapper(teststring, h_fips_multiblock));
}

Test *tests_hashes_sha256_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha256_hash_sequence_01),
        new_TestFixture(test_hashes_sha256_hash_sequence_02),
        new_TestFixture(test_hashes_sha256_hash_sequence_03),
        new_TestFixture(test_hashes_sha256_hash_sequence_04),

        new_TestFixture(test_hashes_sha256_hash_sequence_digits_letters),
        new_TestFixture(test_hashes_sha256_hash_sequence_pangramm),
        new_TestFixture(test_hashes_sha256_hash_sequence_pangramm_no_more),
        new_TestFixture(test_hashes_sha256_hash_sequence_empty),
        new_TestFixture(test_hashes_sha256_hash_sequence_failing_compare),

        new_TestFixture(test_hashes_sha256_hash_long_sequence),

        new_TestFixture(test_hashes_sha256_hash_sequence_abc),
        new_TestFixture(test_hashes_sha256_hash_sequence_abc_long),
    };

    EMB_UNIT_TESTCALLER(hashes_sha256_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha256_tests;
}
