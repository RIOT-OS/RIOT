/*
 * SPDX-FileCopyrightText: 2023 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       testcases for the sha512_256 implementation
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"

#include "hashes/sha512_256.h"

#include "tests-hashes.h"

/**
 * Hashes obtained using https://emn178.github.io/online-tools/sha512_256.html
 * C-array generated with
 * s=$(echo $hash | sed -e 's/../0x&, /g' | sed 's/, $//'); echo $s;
 *
 * where $hash is the hash of the given string
 */

/**
 * @brief expected hash for test empty
 * i.e. c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a
 */
static const unsigned char h_empty[] =
   {0xc6, 0x72, 0xb8, 0xd1, 0xef, 0x56, 0xed, 0x28, 0xab, 0x87, 0xc3, 0x62, 0x2c, 0x51, 0x14, 0x06,
    0x9b, 0xdd, 0x3a, 0xd7, 0xb8, 0xf9, 0x73, 0x74, 0x98, 0xd0, 0xc0, 0x1e, 0xce, 0xf0, 0x96, 0x7a};
static const char *s_empty = "";

/**
 * @brief expected hash for "abc"
 * i.e. 53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23
 */
static const unsigned char h_abc[] =
   {0x53, 0x04, 0x8e, 0x26, 0x81, 0x94, 0x1e, 0xf9, 0x9b, 0x2e, 0x29, 0xb7, 0x6b, 0x4c, 0x7d, 0xab,
    0xe4, 0xc2, 0xd0, 0xc6, 0x34, 0xfc, 0x6d, 0x46, 0xe0, 0xe2, 0xf1, 0x31, 0x07, 0xe7, 0xaf, 0x23};
static const char *s_abc = "abc";
/**
 * @brief expected hash for string from FIPS 180-2 Appendix D.2
 * i.e. 3928e184fb8690f840da3988121d31be65cb9d3ef83ee6146feac861e19b563a
 */
static const unsigned char h_abc_long[] =
   {0x39, 0x28, 0xe1, 0x84, 0xfb, 0x86, 0x90, 0xf8, 0x40, 0xda, 0x39, 0x88, 0x12, 0x1d, 0x31, 0xbe,
    0x65, 0xcb, 0x9d, 0x3e, 0xf8, 0x3e, 0xe6, 0x14, 0x6f, 0xea, 0xc8, 0x61, 0xe1, 0x9b, 0x56, 0x3a};
static const char *s_abc_long =
    "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijkl"
    "mnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

/**
 * @brief expected hash for test long_sequence
 * i.e. 25c09a60bbdd1cd422bfd4c740c6c4a6acb734027e39e2ae7740858bd1022c5f
 */
static const unsigned char h_long_sequence[] =
   {0x25, 0xc0, 0x9a, 0x60, 0xbb, 0xdd, 0x1c, 0xd4, 0x22, 0xbf, 0xd4, 0xc7, 0x40, 0xc6, 0xc4, 0xa6,
    0xac, 0xb7, 0x34, 0x02, 0x7e, 0x39, 0xe2, 0xae, 0x77, 0x40, 0x85, 0x8b, 0xd1, 0x02, 0x2c, 0x5f};
static const char *s_long_sequence =
    {"RIOT is an open-source microkernel-based operating system, designed"
    " to match the requirements of Internet of Things (IoT) devices and"
    " other embedded devices. These requirements include a very low memory"
    " footprint (on the order of a few kilobytes), high energy efficiency"
    ", real-time capabilities, communication stacks for both wireless and"
    " wired networks, and support for a wide range of low-power hardware."};

/**
 * @brief expected hash for { 0xC0, 0xFF, 0xEE }
 * i.e. 22680446a2d2ec571ae5ec2b45f59c70211b5fcf44894c02bd242f7b05b24870
 */
static const unsigned char h_coffee[] =
   {0x22, 0x68, 0x04, 0x46, 0xa2, 0xd2, 0xec, 0x57, 0x1a, 0xe5, 0xec, 0x2b, 0x45, 0xf5, 0x9c, 0x70,
    0x21, 0x1b, 0x5f, 0xcf, 0x44, 0x89, 0x4c, 0x02, 0xbd, 0x24, 0x2f, 0x7b, 0x05, 0xb2, 0x48, 0x70};
static const uint8_t s_coffe[] = { 0xC0, 0xFF, 0xEE };

static int calc_and_compare_hash(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA512_256_DIGEST_LENGTH];
    sha512_256_context_t sha512_256;

    sha512_256_init(&sha512_256);
    sha512_256_update(&sha512_256, (uint8_t*)str, strlen(str));
    sha512_256_final(&sha512_256, hash);

    return (memcmp(expected, hash, SHA512_256_DIGEST_LENGTH) == 0);
}

static int calc_and_compare_hash_wrapper(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA512_256_DIGEST_LENGTH];

    sha512_256((uint8_t*)str, strlen(str), hash);

    return (memcmp(expected, hash, SHA512_256_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_256_hash_sequence_empty(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_empty, h_empty));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_empty, h_empty));
}

static void test_hashes_sha512_256_hash_sequence_abc(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_abc, h_abc));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_abc, h_abc));
}

static void test_hashes_sha512_256_hash_sequence_abc_long(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_abc_long, h_abc_long));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_abc_long, h_abc_long));
}

static void test_hashes_sha512_256_hash_long_sequence(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_long_sequence, h_long_sequence));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_long_sequence, h_long_sequence));
}

static void test_hashes_sha512_256_hash_sequence_binary(void)
{
    static uint8_t hash[SHA512_256_DIGEST_LENGTH];
    sha512_256(s_coffe, sizeof(s_coffe), hash);
    TEST_ASSERT(memcmp(h_coffee, hash, SHA512_256_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_256_hash_update_twice(void)
{
    static const char teststring[] = "abcdef";

    static uint8_t hash_update_once[SHA512_256_DIGEST_LENGTH];
    static uint8_t hash_update_twice[SHA512_256_DIGEST_LENGTH];
    sha512_256_context_t sha512_256;

    sha512_256_init(&sha512_256);
    sha512_256_update(&sha512_256, (uint8_t*)teststring, sizeof(teststring));
    sha512_256_final(&sha512_256, hash_update_once);

    sha512_256_init(&sha512_256);
    sha512_256_update(&sha512_256, (uint8_t*)teststring, 3);
    sha512_256_update(&sha512_256, (uint8_t*)&teststring[3], sizeof(teststring)-3);
    sha512_256_final(&sha512_256, hash_update_twice);

    TEST_ASSERT(memcmp(hash_update_once, hash_update_twice, SHA512_256_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_256_hash_clear_ctx(void)
{
    static uint8_t hash[SHA512_256_DIGEST_LENGTH];
    sha512_256_context_t sha512_256;

    sha512_256_init(&sha512_256);
    sha512_256_update(&sha512_256, s_empty, strlen(s_empty));
    sha512_256_final(&sha512_256, hash);

    TEST_ASSERT(memcmp(h_empty, hash, SHA512_256_DIGEST_LENGTH) == 0);
    for (size_t i=0; i<sizeof(sha512_256_context_t); i++) {
        if (((uint8_t*)&sha512_256)[i] != 0)
            TEST_FAIL("sha512_256_context_t not completely cleared.");
    }
}

Test *tests_hashes_sha512_256_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha512_256_hash_sequence_empty),
        new_TestFixture(test_hashes_sha512_256_hash_sequence_abc),
        new_TestFixture(test_hashes_sha512_256_hash_sequence_abc_long),
        new_TestFixture(test_hashes_sha512_256_hash_long_sequence),
        new_TestFixture(test_hashes_sha512_256_hash_sequence_binary),
        new_TestFixture(test_hashes_sha512_256_hash_update_twice),
        new_TestFixture(test_hashes_sha512_256_hash_clear_ctx),
    };

    EMB_UNIT_TESTCALLER(hashes_sha512_256_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha512_256_tests;
}
