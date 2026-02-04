/*
 * SPDX-FileCopyrightText: 2023 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       testcases for the sha512_224 implementation
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

#include "hashes/sha512_224.h"

#include "tests-hashes.h"

/**
 * Hashes obtained using https://emn178.github.io/online-tools/sha512_224.html
 * C-array generated with
 * s=$(echo $hash | sed -e 's/../0x&, /g' | sed 's/, $//'); echo $s;
 *
 * where $hash is the hash of the given string
 */

/**
 * @brief expected hash for test empty
 * i.e. 6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4
 */
static const unsigned char h_empty[] =
   {0x6e, 0xd0, 0xdd, 0x02, 0x80, 0x6f, 0xa8, 0x9e, 0x25, 0xde, 0x06, 0x0c, 0x19, 0xd3,
    0xac, 0x86, 0xca, 0xbb, 0x87, 0xd6, 0xa0, 0xdd, 0xd0, 0x5c, 0x33, 0x3b, 0x84, 0xf4};
static const char *s_empty = "";

/**
 * @brief expected hash for "abc"
 * i.e. 4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa
 */
static const unsigned char h_abc[] =
   {0x46, 0x34, 0x27, 0x0f, 0x70, 0x7b, 0x6a, 0x54, 0xda, 0xae, 0x75, 0x30, 0x46, 0x08,
    0x42, 0xe2, 0x0e, 0x37, 0xed, 0x26, 0x5c, 0xee, 0xe9, 0xa4, 0x3e, 0x89, 0x24, 0xaa};
static const char *s_abc = "abc";
/**
 * @brief expected hash for string from FIPS 180-2 Appendix D.2
 * i.e. 23fec5bb94d60b23308192640b0c453335d664734fe40e7268674af9
 */
static const unsigned char h_abc_long[] =
   {0x23, 0xfe, 0xc5, 0xbb, 0x94, 0xd6, 0x0b, 0x23, 0x30, 0x81, 0x92, 0x64, 0x0b, 0x0c,
    0x45, 0x33, 0x35, 0xd6, 0x64, 0x73, 0x4f, 0xe4, 0x0e, 0x72, 0x68, 0x67, 0x4a, 0xf9};
static const char *s_abc_long =
    "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijkl"
    "mnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

/**
 * @brief expected hash for test long_sequence
 * i.e. 15d26dcdbf77989cc4534ce24b1d3ff9c607767787b96241e358d6b8
 */
static const unsigned char h_long_sequence[] =
   {0x15, 0xd2, 0x6d, 0xcd, 0xbf, 0x77, 0x98, 0x9c, 0xc4, 0x53, 0x4c, 0xe2, 0x4b, 0x1d,
    0x3f, 0xf9, 0xc6, 0x07, 0x76, 0x77, 0x87, 0xb9, 0x62, 0x41, 0xe3, 0x58, 0xd6, 0xb8};
static const char *s_long_sequence =
    {"RIOT is an open-source microkernel-based operating system, designed"
    " to match the requirements of Internet of Things (IoT) devices and"
    " other embedded devices. These requirements include a very low memory"
    " footprint (on the order of a few kilobytes), high energy efficiency"
    ", real-time capabilities, communication stacks for both wireless and"
    " wired networks, and support for a wide range of low-power hardware."};

/**
 * @brief expected hash for { 0xC0, 0xFF, 0xEE }
 * i.e. f16f1e9cf96b5b9caa0d5abd0aa2a10bdd0636d0d1c3de2b4c11e312
 */
static const unsigned char h_coffee[] =
   {0xf1, 0x6f, 0x1e, 0x9c, 0xf9, 0x6b, 0x5b, 0x9c, 0xaa, 0x0d, 0x5a, 0xbd, 0x0a, 0xa2,
    0xa1, 0x0b, 0xdd, 0x06, 0x36, 0xd0, 0xd1, 0xc3, 0xde, 0x2b, 0x4c, 0x11, 0xe3, 0x12};
static const uint8_t s_coffe[] = { 0xC0, 0xFF, 0xEE };

static int calc_and_compare_hash(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA512_224_DIGEST_LENGTH];
    sha512_224_context_t sha512_224;

    sha512_224_init(&sha512_224);
    sha512_224_update(&sha512_224, (uint8_t*)str, strlen(str));
    sha512_224_final(&sha512_224, hash);

    return (memcmp(expected, hash, SHA512_224_DIGEST_LENGTH) == 0);
}

static int calc_and_compare_hash_wrapper(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA512_224_DIGEST_LENGTH];

    sha512_224((uint8_t*)str, strlen(str), hash);

    return (memcmp(expected, hash, SHA512_224_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_224_hash_sequence_empty(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_empty, h_empty));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_empty, h_empty));
}

static void test_hashes_sha512_224_hash_sequence_abc(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_abc, h_abc));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_abc, h_abc));
}

static void test_hashes_sha512_224_hash_sequence_abc_long(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_abc_long, h_abc_long));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_abc_long, h_abc_long));
}

static void test_hashes_sha512_224_hash_long_sequence(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_long_sequence, h_long_sequence));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_long_sequence, h_long_sequence));
}

static void test_hashes_sha512_224_hash_sequence_binary(void)
{
    static uint8_t hash[SHA512_224_DIGEST_LENGTH];
    sha512_224(s_coffe, sizeof(s_coffe), hash);
    TEST_ASSERT(memcmp(h_coffee, hash, SHA512_224_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_224_hash_update_twice(void)
{
    static const char teststring[] = "abcdef";

    static uint8_t hash_update_once[SHA512_224_DIGEST_LENGTH];
    static uint8_t hash_update_twice[SHA512_224_DIGEST_LENGTH];
    sha512_224_context_t sha512_224;

    sha512_224_init(&sha512_224);
    sha512_224_update(&sha512_224, (uint8_t*)teststring, sizeof(teststring));
    sha512_224_final(&sha512_224, hash_update_once);

    sha512_224_init(&sha512_224);
    sha512_224_update(&sha512_224, (uint8_t*)teststring, 3);
    sha512_224_update(&sha512_224, (uint8_t*)&teststring[3], sizeof(teststring)-3);
    sha512_224_final(&sha512_224, hash_update_twice);

    TEST_ASSERT(memcmp(hash_update_once, hash_update_twice, SHA512_224_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_224_hash_clear_ctx(void)
{
    static uint8_t hash[SHA512_224_DIGEST_LENGTH];
    sha512_224_context_t sha512_224;

    sha512_224_init(&sha512_224);
    sha512_224_update(&sha512_224, s_empty, strlen(s_empty));
    sha512_224_final(&sha512_224, hash);

    TEST_ASSERT(memcmp(h_empty, hash, SHA512_224_DIGEST_LENGTH) == 0);
    for (size_t i=0; i<sizeof(sha512_224_context_t); i++) {
        if (((uint8_t*)&sha512_224)[i] != 0)
            TEST_FAIL("sha512_224_context_t not completely cleared.");
    }
}

Test *tests_hashes_sha512_224_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha512_224_hash_sequence_empty),
        new_TestFixture(test_hashes_sha512_224_hash_sequence_abc),
        new_TestFixture(test_hashes_sha512_224_hash_sequence_abc_long),
        new_TestFixture(test_hashes_sha512_224_hash_long_sequence),
        new_TestFixture(test_hashes_sha512_224_hash_sequence_binary),
        new_TestFixture(test_hashes_sha512_224_hash_update_twice),
        new_TestFixture(test_hashes_sha512_224_hash_clear_ctx),
    };

    EMB_UNIT_TESTCALLER(hashes_sha512_224_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha512_224_tests;
}
