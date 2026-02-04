/*
 * SPDX-FileCopyrightText: 2023 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       testcases for the sha384 implementation
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

#include "hashes/sha384.h"

#include "tests-hashes.h"

/**
 * Hashes obtained using:
 * h=$(echo -n '<test string>' | sha384sum - | cut -d ' ' -f1); s=$(echo $h | sed -e 's/../0x&, /g' | sed 's/, $//'); echo $h; echo {$s};
 *
 * where <test string> is the message to hash
 */

/**
 * @brief expected hash for test empty
 * i.e. 38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b
 */
static const unsigned char h_empty[] =
   {0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
    0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43, 0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
    0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb, 0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b};
static const char *s_empty = "";

/**
 * @brief expected hash for "abc"
 *        (from FIPS 180-2 Appendix D.1)
 * i.e. cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7
 */
static const unsigned char h_abc[] =
   {0xcb, 0x00, 0x75, 0x3f, 0x45, 0xa3, 0x5e, 0x8b, 0xb5, 0xa0, 0x3d, 0x69, 0x9a, 0xc6, 0x50, 0x07,
    0x27, 0x2c, 0x32, 0xab, 0x0e, 0xde, 0xd1, 0x63, 0x1a, 0x8b, 0x60, 0x5a, 0x43, 0xff, 0x5b, 0xed,
    0x80, 0x86, 0x07, 0x2b, 0xa1, 0xe7, 0xcc, 0x23, 0x58, 0xba, 0xec, 0xa1, 0x34, 0xc8, 0x25, 0xa7};
static const char *s_abc = "abc";
/**
 * @brief expected hash for FIPS 180-2 Appendix D.2
 * i.e. 09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039
 */
static const unsigned char h_abc_long[] =
   {0x09, 0x33, 0x0c, 0x33, 0xf7, 0x11, 0x47, 0xe8, 0x3d, 0x19, 0x2f, 0xc7, 0x82, 0xcd, 0x1b, 0x47,
    0x53, 0x11, 0x1b, 0x17, 0x3b, 0x3b, 0x05, 0xd2, 0x2f, 0xa0, 0x80, 0x86, 0xe3, 0xb0, 0xf7, 0x12,
    0xfc, 0xc7, 0xc7, 0x1a, 0x55, 0x7e, 0x2d, 0xb9, 0x66, 0xc3, 0xe9, 0xfa, 0x91, 0x74, 0x60, 0x39};
static const char *s_abc_long =
    "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijkl"
    "mnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

/**
 * @brief expected hash for test long_sequence
 * i.e. 1c2aaa49583a5f5507612ddfa87d75a4bfc384c25ed2eca878d77e1614217272b7aee62dc51432fb9b91d773798d3fc1
 */
static const unsigned char h_long_sequence[] =
   {0x1c, 0x2a, 0xaa, 0x49, 0x58, 0x3a, 0x5f, 0x55, 0x07, 0x61, 0x2d, 0xdf, 0xa8, 0x7d, 0x75, 0xa4,
    0xbf, 0xc3, 0x84, 0xc2, 0x5e, 0xd2, 0xec, 0xa8, 0x78, 0xd7, 0x7e, 0x16, 0x14, 0x21, 0x72, 0x72,
    0xb7, 0xae, 0xe6, 0x2d, 0xc5, 0x14, 0x32, 0xfb, 0x9b, 0x91, 0xd7, 0x73, 0x79, 0x8d, 0x3f, 0xc1};
static const char *s_long_sequence =
    {"RIOT is an open-source microkernel-based operating system, designed"
    " to match the requirements of Internet of Things (IoT) devices and"
    " other embedded devices. These requirements include a very low memory"
    " footprint (on the order of a few kilobytes), high energy efficiency"
    ", real-time capabilities, communication stacks for both wireless and"
    " wired networks, and support for a wide range of low-power hardware."};

/**
 * @brief expected hash for { 0xC0, 0xFF, 0xEE }
 * i.e. 011f360db636cfa4c7a61768ad917fe3d95a6bd88a7968ce437b00b63a32b0da911329488b8571224e4245250b62ba86
 *
 * obtained using:
 * h=$(printf '\xc0\xff\xee' | sha384sum - | cut -d ' ' -f1); s=$(echo $h | sed -e 's/../0x&, /g' | sed 's/, $//'); echo $h; echo {$s};
 */
static const unsigned char h_coffee[] =
   {0x01, 0x1f, 0x36, 0x0d, 0xb6, 0x36, 0xcf, 0xa4, 0xc7, 0xa6, 0x17, 0x68, 0xad, 0x91, 0x7f, 0xe3,
    0xd9, 0x5a, 0x6b, 0xd8, 0x8a, 0x79, 0x68, 0xce, 0x43, 0x7b, 0x00, 0xb6, 0x3a, 0x32, 0xb0, 0xda,
    0x91, 0x13, 0x29, 0x48, 0x8b, 0x85, 0x71, 0x22, 0x4e, 0x42, 0x45, 0x25, 0x0b, 0x62, 0xba, 0x86};
static const uint8_t s_coffe[] = { 0xC0, 0xFF, 0xEE };

static int calc_and_compare_hash(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA384_DIGEST_LENGTH];
    sha384_context_t sha384;

    sha384_init(&sha384);
    sha384_update(&sha384, (uint8_t*)str, strlen(str));
    sha384_final(&sha384, hash);

    return (memcmp(expected, hash, SHA384_DIGEST_LENGTH) == 0);
}

static int calc_and_compare_hash_wrapper(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA384_DIGEST_LENGTH];

    sha384((uint8_t*)str, strlen(str), hash);

    return (memcmp(expected, hash, SHA384_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha384_hash_sequence_empty(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_empty, h_empty));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_empty, h_empty));
}

static void test_hashes_sha384_hash_sequence_abc(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_abc, h_abc));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_abc, h_abc));
}

static void test_hashes_sha384_hash_sequence_abc_long(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_abc_long, h_abc_long));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_abc_long, h_abc_long));
}

static void test_hashes_sha384_hash_long_sequence(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_long_sequence, h_long_sequence));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_long_sequence, h_long_sequence));
}

static void test_hashes_sha384_hash_sequence_binary(void)
{
    static uint8_t hash[SHA384_DIGEST_LENGTH];
    sha384(s_coffe, sizeof(s_coffe), hash);
    TEST_ASSERT(memcmp(h_coffee, hash, SHA384_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha384_hash_update_twice(void)
{
    static const char teststring[] = "abcdef";

    static uint8_t hash_update_once[SHA384_DIGEST_LENGTH];
    static uint8_t hash_update_twice[SHA384_DIGEST_LENGTH];
    sha384_context_t sha384;

    sha384_init(&sha384);
    sha384_update(&sha384, (uint8_t*)teststring, sizeof(teststring));
    sha384_final(&sha384, hash_update_once);

    sha384_init(&sha384);
    sha384_update(&sha384, (uint8_t*)teststring, 3);
    sha384_update(&sha384, (uint8_t*)&teststring[3], sizeof(teststring)-3);
    sha384_final(&sha384, hash_update_twice);

    TEST_ASSERT(memcmp(hash_update_once, hash_update_twice, SHA384_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha384_hash_clear_ctx(void)
{
    static uint8_t hash[SHA384_DIGEST_LENGTH];
    sha384_context_t sha384;

    sha384_init(&sha384);
    sha384_update(&sha384, s_empty, strlen(s_empty));
    sha384_final(&sha384, hash);

    TEST_ASSERT(memcmp(h_empty, hash, SHA384_DIGEST_LENGTH) == 0);
    for (size_t i=0; i<sizeof(sha384_context_t); i++) {
        if (((uint8_t*)&sha384)[i] != 0)
            TEST_FAIL("sha384_context_t not completely cleared.");
    }
}

Test *tests_hashes_sha384_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha384_hash_sequence_empty),
        new_TestFixture(test_hashes_sha384_hash_sequence_abc),
        new_TestFixture(test_hashes_sha384_hash_sequence_abc_long),
        new_TestFixture(test_hashes_sha384_hash_long_sequence),
        new_TestFixture(test_hashes_sha384_hash_sequence_binary),
        new_TestFixture(test_hashes_sha384_hash_update_twice),
        new_TestFixture(test_hashes_sha384_hash_clear_ctx),
    };

    EMB_UNIT_TESTCALLER(hashes_sha384_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha384_tests;
}
