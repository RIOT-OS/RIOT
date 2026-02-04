/*
 * SPDX-FileCopyrightText: 2023 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       testcases for the sha512 implementation
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

#include "hashes/sha512.h"

#include "tests-hashes.h"

/**
 * @brief expected hash for test 01
 * i.e. 0f3e840daba378160173034567d6fa7373056634834c7127399871f6175ff9f5c09cb0d1af35679de6b5893ab7c800a46f121821414f4cf11c27e67630e10e99
 *
 * obtained using:
 * h=$(echo -n '<test string>' | sha512sum - | cut -d ' ' -f1); s=$(echo $h | sed -e 's/../0x&, /g' | sed 's/, $//'); echo $h; echo {$s};
 *
 * where <test string> is the message to hash, i.e. 1234567890_1 for this test
 */
static const unsigned char h01[] = {0x0f, 0x3e, 0x84, 0x0d, 0xab, 0xa3, 0x78, 0x16,
                                    0x01, 0x73, 0x03, 0x45, 0x67, 0xd6, 0xfa, 0x73,
                                    0x73, 0x05, 0x66, 0x34, 0x83, 0x4c, 0x71, 0x27,
                                    0x39, 0x98, 0x71, 0xf6, 0x17, 0x5f, 0xf9, 0xf5,
                                    0xc0, 0x9c, 0xb0, 0xd1, 0xaf, 0x35, 0x67, 0x9d,
                                    0xe6, 0xb5, 0x89, 0x3a, 0xb7, 0xc8, 0x00, 0xa4,
                                    0x6f, 0x12, 0x18, 0x21, 0x41, 0x4f, 0x4c, 0xf1,
                                    0x1c, 0x27, 0xe6, 0x76, 0x30, 0xe1, 0x0e, 0x99};
static const char *s01 = "1234567890_1";

/**
 * @brief expected hash for test 02
 * i.e. 448e45902091177d01a402e1d31d0852899c48eaa2b331868b91afbec39a2a3413145f565336004055bbc05cfdc862732bf002bf90bc3f941ed7f6bcbc19bdc9
 */
static const unsigned char h02[] = {0x44, 0x8e, 0x45, 0x90, 0x20, 0x91, 0x17, 0x7d,
                                    0x01, 0xa4, 0x02, 0xe1, 0xd3, 0x1d, 0x08, 0x52,
                                    0x89, 0x9c, 0x48, 0xea, 0xa2, 0xb3, 0x31, 0x86,
                                    0x8b, 0x91, 0xaf, 0xbe, 0xc3, 0x9a, 0x2a, 0x34,
                                    0x13, 0x14, 0x5f, 0x56, 0x53, 0x36, 0x00, 0x40,
                                    0x55, 0xbb, 0xc0, 0x5c, 0xfd, 0xc8, 0x62, 0x73,
                                    0x2b, 0xf0, 0x02, 0xbf, 0x90, 0xbc, 0x3f, 0x94,
                                    0x1e, 0xd7, 0xf6, 0xbc, 0xbc, 0x19, 0xbd, 0xc9};
static const char *s02 = "1234567890_2";

/**
 * @brief expected hash for test 03
 * i.e. bc92bfae4d2b2e40371e543e0b70033ca1d308e01452fcc5678ac7b20b254b09159290166e5c1f4012d5295e2057dc202a4c42bf7d4cb7229f28c5bcf18e655f
 */
static const unsigned char h03[] = {0xbc, 0x92, 0xbf, 0xae, 0x4d, 0x2b, 0x2e, 0x40,
                                    0x37, 0x1e, 0x54, 0x3e, 0x0b, 0x70, 0x03, 0x3c,
                                    0xa1, 0xd3, 0x08, 0xe0, 0x14, 0x52, 0xfc, 0xc5,
                                    0x67, 0x8a, 0xc7, 0xb2, 0x0b, 0x25, 0x4b, 0x09,
                                    0x15, 0x92, 0x90, 0x16, 0x6e, 0x5c, 0x1f, 0x40,
                                    0x12, 0xd5, 0x29, 0x5e, 0x20, 0x57, 0xdc, 0x20,
                                    0x2a, 0x4c, 0x42, 0xbf, 0x7d, 0x4c, 0xb7, 0x22,
                                    0x9f, 0x28, 0xc5, 0xbc, 0xf1, 0x8e, 0x65, 0x5f};
static const char *s03 = "1234567890_3";

/**
 * @brief expected hash for test 04
 * i.e. e9a43cf81972a22e85f1dca0b3be0b71f07ef30778ddf0eb5ae40e6d9ff1927db1d53c717f0cf43f1d99cfe360170a0a5885d2a85ac498be4f12405da4a8c79d
 */
static const unsigned char h04[] = {0xe9, 0xa4, 0x3c, 0xf8, 0x19, 0x72, 0xa2, 0x2e,
                                    0x85, 0xf1, 0xdc, 0xa0, 0xb3, 0xbe, 0x0b, 0x71,
                                    0xf0, 0x7e, 0xf3, 0x07, 0x78, 0xdd, 0xf0, 0xeb,
                                    0x5a, 0xe4, 0x0e, 0x6d, 0x9f, 0xf1, 0x92, 0x7d,
                                    0xb1, 0xd5, 0x3c, 0x71, 0x7f, 0x0c, 0xf4, 0x3f,
                                    0x1d, 0x99, 0xcf, 0xe3, 0x60, 0x17, 0x0a, 0x0a,
                                    0x58, 0x85, 0xd2, 0xa8, 0x5a, 0xc4, 0x98, 0xbe,
                                    0x4f, 0x12, 0x40, 0x5d, 0xa4, 0xa8, 0xc7, 0x9d};
static const char *s04 = "1234567890_4";

/**
 * @brief expected hash for test digits_letters
 * i.e. f380afeb63f7d64018d89836c766a18f3cde99047a7fe183326d101ca9d9d4f9a5f03d77b6d542c66bcc9f46c766fe59a6a7dab300237b031a38600f463bb329
 */
static const unsigned char hdigits_letters[] =
                                   {0xf3, 0x80, 0xaf, 0xeb, 0x63, 0xf7, 0xd6, 0x40,
                                    0x18, 0xd8, 0x98, 0x36, 0xc7, 0x66, 0xa1, 0x8f,
                                    0x3c, 0xde, 0x99, 0x04, 0x7a, 0x7f, 0xe1, 0x83,
                                    0x32, 0x6d, 0x10, 0x1c, 0xa9, 0xd9, 0xd4, 0xf9,
                                    0xa5, 0xf0, 0x3d, 0x77, 0xb6, 0xd5, 0x42, 0xc6,
                                    0x6b, 0xcc, 0x9f, 0x46, 0xc7, 0x66, 0xfe, 0x59,
                                    0xa6, 0xa7, 0xda, 0xb3, 0x00, 0x23, 0x7b, 0x03,
                                    0x1a, 0x38, 0x60, 0x0f, 0x46, 0x3b, 0xb3, 0x29};
static const char *sdigits_letters =
    "0123456789abcde-0123456789abcde-0123456789abcde-0123456789abcde-";

/**
 * @brief expected hash for test pangramm
 * i.e. af9ed2de700433b803240a552b41b5a472a6ef3fe1431a722b2063c75e9f07451f67a28e37d09cde769424c96aea6f8971389db9e1993d6c565c3c71b855723c
 */
static const unsigned char hpangramm[] =
                                   {0xaf, 0x9e, 0xd2, 0xde, 0x70, 0x04, 0x33, 0xb8,
                                    0x03, 0x24, 0x0a, 0x55, 0x2b, 0x41, 0xb5, 0xa4,
                                    0x72, 0xa6, 0xef, 0x3f, 0xe1, 0x43, 0x1a, 0x72,
                                    0x2b, 0x20, 0x63, 0xc7, 0x5e, 0x9f, 0x07, 0x45,
                                    0x1f, 0x67, 0xa2, 0x8e, 0x37, 0xd0, 0x9c, 0xde,
                                    0x76, 0x94, 0x24, 0xc9, 0x6a, 0xea, 0x6f, 0x89,
                                    0x71, 0x38, 0x9d, 0xb9, 0xe1, 0x99, 0x3d, 0x6c,
                                    0x56, 0x5c, 0x3c, 0x71, 0xb8, 0x55, 0x72, 0x3c};
static const char *spangramm = "Franz jagt im komplett verwahrlosten Taxi quer durch Bayern";

/**
 * @brief expected hash for test pangramm_no_more
 * i.e. 90b30ef9902ae4c4c691d2d78c2f8fa0aa785afbc5545286b310f68e91dd2299c84a2484f0419fc5eaa7de598940799e1091c4948926ae1c9488dddae180bb80
 */
static const unsigned char hpangramm_no_more[] =
                                   {0x90, 0xb3, 0x0e, 0xf9, 0x90, 0x2a, 0xe4, 0xc4,
                                    0xc6, 0x91, 0xd2, 0xd7, 0x8c, 0x2f, 0x8f, 0xa0,
                                    0xaa, 0x78, 0x5a, 0xfb, 0xc5, 0x54, 0x52, 0x86,
                                    0xb3, 0x10, 0xf6, 0x8e, 0x91, 0xdd, 0x22, 0x99,
                                    0xc8, 0x4a, 0x24, 0x84, 0xf0, 0x41, 0x9f, 0xc5,
                                    0xea, 0xa7, 0xde, 0x59, 0x89, 0x40, 0x79, 0x9e,
                                    0x10, 0x91, 0xc4, 0x94, 0x89, 0x26, 0xae, 0x1c,
                                    0x94, 0x88, 0xdd, 0xda, 0xe1, 0x80, 0xbb, 0x80};
/* exchanged `z` with `k` of the first word `Fran[z|k]` */
static const char *spangramm_no_more = "Frank jagt im komplett verwahrlosten Taxi quer durch Bayern";

/**
 * @brief expected hash for test empty
 * i.e. cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e
 */
static const unsigned char hempty[] =
                                   {0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
                                    0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
                                    0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
                                    0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
                                    0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
                                    0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
                                    0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
                                    0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e};
static const char *sempty = "";

/**
 * @brief wrong hash for test failing_compare (changed last byte from a4 to a5)
 * i.e. 2dfe3483a0ca0bfb78c0928832bd6854eb0b18963ba24d7d2c9dcd8ec0403bc4d4201f3006d9d235189f657af978b613ea3324bfb7a9d748933980ded86c93a5
 */
static const unsigned char hfailing_compare[] =
                                   {0x2d, 0xfe, 0x34, 0x83, 0xa0, 0xca, 0x0b, 0xfb,
                                    0x78, 0xc0, 0x92, 0x88, 0x32, 0xbd, 0x68, 0x54,
                                    0xeb, 0x0b, 0x18, 0x96, 0x3b, 0xa2, 0x4d, 0x7d,
                                    0x2c, 0x9d, 0xcd, 0x8e, 0xc0, 0x40, 0x3b, 0xc4,
                                    0xd4, 0x20, 0x1f, 0x30, 0x06, 0xd9, 0xd2, 0x35,
                                    0x18, 0x9f, 0x65, 0x7a, 0xf9, 0x78, 0xb6, 0x13,
                                    0xea, 0x33, 0x24, 0xbf, 0xb7, 0xa9, 0xd7, 0x48,
                                    0x93, 0x39, 0x80, 0xde, 0xd8, 0x6c, 0x93, 0xa5};
static const char *sfailing_compare = "This test fails!";

/**
 * @brief expected hash for test long_sequence
 * i.e. 21185dd187d8b7d32d36419260bc65e03294f393aa24c4caf528d664f1cd07a1cc77d63baaa351e156b3f40ca1899121e6dcb0ced8672e1b36c45ec252002a8f
 */
static const unsigned char hlong_sequence[] =
                                   {0x21, 0x18, 0x5d, 0xd1, 0x87, 0xd8, 0xb7, 0xd3,
                                    0x2d, 0x36, 0x41, 0x92, 0x60, 0xbc, 0x65, 0xe0,
                                    0x32, 0x94, 0xf3, 0x93, 0xaa, 0x24, 0xc4, 0xca,
                                    0xf5, 0x28, 0xd6, 0x64, 0xf1, 0xcd, 0x07, 0xa1,
                                    0xcc, 0x77, 0xd6, 0x3b, 0xaa, 0xa3, 0x51, 0xe1,
                                    0x56, 0xb3, 0xf4, 0x0c, 0xa1, 0x89, 0x91, 0x21,
                                    0xe6, 0xdc, 0xb0, 0xce, 0xd8, 0x67, 0x2e, 0x1b,
                                    0x36, 0xc4, 0x5e, 0xc2, 0x52, 0x00, 0x2a, 0x8f};
static const char *slong_sequence =
    {"RIOT is an open-source microkernel-based operating system, designed"
    " to match the requirements of Internet of Things (IoT) devices and"
    " other embedded devices. These requirements include a very low memory"
    " footprint (on the order of a few kilobytes), high energy efficiency"
    ", real-time capabilities, communication stacks for both wireless and"
    " wired networks, and support for a wide range of low-power hardware."};

/**
 * @brief expected hash for "abc"
 *        (from FIPS 180-2 Appendix C.1)
 */
static const unsigned char h_fips_oneblock[] =
                                   {0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba,
                                    0xcc, 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31,
                                    0x12, 0xe6, 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2,
                                    0x0a, 0x9e, 0xee, 0xe6, 0x4b, 0x55, 0xd3, 0x9a,
                                    0x21, 0x92, 0x99, 0x2a, 0x27, 0x4f, 0xc1, 0xa8,
                                    0x36, 0xba, 0x3c, 0x23, 0xa3, 0xfe, 0xeb, 0xbd,
                                    0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c, 0xe8, 0x0e,
                                    0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4, 0x9f};
static const char *s_fips_oneblock = "abc";

/**
 * @brief expected hash for "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
 *        (from FIPS 180-2 Appendix C.2)
 */
static const unsigned char h_fips_multiblock[] =
                                    {0x8e, 0x95, 0x9b, 0x75, 0xda, 0xe3, 0x13, 0xda,
                                     0x8c, 0xf4, 0xf7, 0x28, 0x14, 0xfc, 0x14, 0x3f,
                                     0x8f, 0x77, 0x79, 0xc6, 0xeb, 0x9f, 0x7f, 0xa1,
                                     0x72, 0x99, 0xae, 0xad, 0xb6, 0x88, 0x90, 0x18,
                                     0x50, 0x1d, 0x28, 0x9e, 0x49, 0x00, 0xf7, 0xe4,
                                     0x33, 0x1b, 0x99, 0xde, 0xc4, 0xb5, 0x43, 0x3a,
                                     0xc7, 0xd3, 0x29, 0xee, 0xb6, 0xdd, 0x26, 0x54,
                                     0x5e, 0x96, 0xe5, 0x5b, 0x87, 0x4b, 0xe9, 0x09};
static const char *s_fips_multiblock = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklm"
        "ghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

/**
 * @brief expected hash for { 0xC0, 0xFF, 0xEE }
 * i.e. d6f3d166b443b394f2505c48a5c6904c682d5a6fbe360d6c337a98f7ea6675f195157b33f599b600e39783c72024f91b4718651b4cfd08afcf6c06b9cdb6508c
 *
 * obtained using:
 * h=$(printf '\xc0\xff\xee' | sha512sum - | cut -d ' ' -f1); s=$(echo $h | sed -e 's/../0x&, /g' | sed 's/, $//'); echo $h; echo {$s};
 */
static const unsigned char h_coffee[] =
                                    {0xd6, 0xf3, 0xd1, 0x66, 0xb4, 0x43, 0xb3, 0x94,
                                     0xf2, 0x50, 0x5c, 0x48, 0xa5, 0xc6, 0x90, 0x4c,
                                     0x68, 0x2d, 0x5a, 0x6f, 0xbe, 0x36, 0x0d, 0x6c,
                                     0x33, 0x7a, 0x98, 0xf7, 0xea, 0x66, 0x75, 0xf1,
                                     0x95, 0x15, 0x7b, 0x33, 0xf5, 0x99, 0xb6, 0x00,
                                     0xe3, 0x97, 0x83, 0xc7, 0x20, 0x24, 0xf9, 0x1b,
                                     0x47, 0x18, 0x65, 0x1b, 0x4c, 0xfd, 0x08, 0xaf,
                                     0xcf, 0x6c, 0x06, 0xb9, 0xcd, 0xb6, 0x50, 0x8c};
static const uint8_t s_coffe[] = { 0xC0, 0xFF, 0xEE };

static int calc_and_compare_hash(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA512_DIGEST_LENGTH];
    sha512_context_t sha512;

    sha512_init(&sha512);
    sha512_update(&sha512, (uint8_t*)str, strlen(str));
    sha512_final(&sha512, hash);

    return (memcmp(expected, hash, SHA512_DIGEST_LENGTH) == 0);
}

static int calc_and_compare_hash_wrapper(const char *str, const unsigned char *expected)
{
    static unsigned char hash[SHA512_DIGEST_LENGTH];

    sha512((uint8_t*)str, strlen(str), hash);

    return (memcmp(expected, hash, SHA512_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_hash_sequence_01(void)
{
    TEST_ASSERT(calc_and_compare_hash(s01, h01));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s01, h01));
}

static void test_hashes_sha512_hash_sequence_02(void)
{
    TEST_ASSERT(calc_and_compare_hash(s02, h02));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s02, h02));
}

static void test_hashes_sha512_hash_sequence_03(void)
{
    TEST_ASSERT(calc_and_compare_hash(s03, h03));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s03, h03));
}

static void test_hashes_sha512_hash_sequence_04(void)
{
    TEST_ASSERT(calc_and_compare_hash(s04, h04));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s04, h04));
}

static void test_hashes_sha512_hash_sequence_digits_letters(void)
{
    TEST_ASSERT(calc_and_compare_hash(sdigits_letters, hdigits_letters));
    TEST_ASSERT(calc_and_compare_hash_wrapper(sdigits_letters, hdigits_letters));
}

static void test_hashes_sha512_hash_sequence_pangramm(void)
{
    TEST_ASSERT(calc_and_compare_hash(spangramm, hpangramm));
    TEST_ASSERT(calc_and_compare_hash_wrapper(spangramm, hpangramm));
}

static void test_hashes_sha512_hash_sequence_pangramm_no_more(void)
{
    TEST_ASSERT(calc_and_compare_hash(spangramm_no_more, hpangramm_no_more));
    TEST_ASSERT(calc_and_compare_hash_wrapper(spangramm_no_more, hpangramm_no_more));
}

static void test_hashes_sha512_hash_sequence_empty(void)
{
    TEST_ASSERT(calc_and_compare_hash(sempty, hempty));
    TEST_ASSERT(calc_and_compare_hash_wrapper(sempty, hempty));
}

static void test_hashes_sha512_hash_sequence_failing_compare(void)
{
    /* failing compare (switched last byte of expected hash from `a4` to `a5`) */
    TEST_ASSERT(!calc_and_compare_hash(sfailing_compare, hfailing_compare));
    TEST_ASSERT(!calc_and_compare_hash_wrapper(sfailing_compare, hfailing_compare));
}

static void test_hashes_sha512_hash_long_sequence(void)
{
    TEST_ASSERT(calc_and_compare_hash(slong_sequence, hlong_sequence));
    TEST_ASSERT(calc_and_compare_hash_wrapper(slong_sequence, hlong_sequence));
}

static void test_hashes_sha512_hash_sequence_abc(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_fips_oneblock, h_fips_oneblock));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_fips_oneblock, h_fips_oneblock));
}

static void test_hashes_sha512_hash_sequence_abc_long(void)
{
    TEST_ASSERT(calc_and_compare_hash(s_fips_multiblock, h_fips_multiblock));
    TEST_ASSERT(calc_and_compare_hash_wrapper(s_fips_multiblock, h_fips_multiblock));
}

static void test_hashes_sha512_hash_sequence_binary(void)
{
    static uint8_t hash[SHA512_DIGEST_LENGTH];
    sha512(s_coffe, sizeof(s_coffe), hash);
    TEST_ASSERT(memcmp(h_coffee, hash, SHA512_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_hash_update_twice(void)
{
    static const char teststring[] = "abcdef";

    static uint8_t hash_update_once[SHA512_DIGEST_LENGTH];
    static uint8_t hash_update_twice[SHA512_DIGEST_LENGTH];
    sha512_context_t sha512;

    sha512_init(&sha512);
    sha512_update(&sha512, (uint8_t*)teststring, sizeof(teststring));
    sha512_final(&sha512, hash_update_once);

    sha512_init(&sha512);
    sha512_update(&sha512, (uint8_t*)teststring, 3);
    sha512_update(&sha512, (uint8_t*)&teststring[3], sizeof(teststring)-3);
    sha512_final(&sha512, hash_update_twice);

    TEST_ASSERT(memcmp(hash_update_once, hash_update_twice, SHA512_DIGEST_LENGTH) == 0);
}

static void test_hashes_sha512_hash_clear_ctx(void)
{
    static uint8_t hash[SHA512_DIGEST_LENGTH];
    sha512_context_t sha512;

    sha512_init(&sha512);
    sha512_update(&sha512, sempty, strlen(sempty));
    sha512_final(&sha512, hash);

    TEST_ASSERT(memcmp(hempty, hash, SHA512_DIGEST_LENGTH) == 0);
    for (size_t i=0; i<sizeof(sha512_context_t); i++) {
        if (((uint8_t*)&sha512)[i] != 0)
            TEST_FAIL("sha512_context_t not completely cleared.");
    }
}

Test *tests_hashes_sha512_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha512_hash_sequence_01),
        new_TestFixture(test_hashes_sha512_hash_sequence_02),
        new_TestFixture(test_hashes_sha512_hash_sequence_03),
        new_TestFixture(test_hashes_sha512_hash_sequence_04),

        new_TestFixture(test_hashes_sha512_hash_sequence_digits_letters),
        new_TestFixture(test_hashes_sha512_hash_sequence_pangramm),
        new_TestFixture(test_hashes_sha512_hash_sequence_pangramm_no_more),
        new_TestFixture(test_hashes_sha512_hash_sequence_empty),
        new_TestFixture(test_hashes_sha512_hash_sequence_failing_compare),

        new_TestFixture(test_hashes_sha512_hash_long_sequence),

        new_TestFixture(test_hashes_sha512_hash_sequence_abc),
        new_TestFixture(test_hashes_sha512_hash_sequence_abc_long),

        new_TestFixture(test_hashes_sha512_hash_sequence_binary),
        new_TestFixture(test_hashes_sha512_hash_update_twice),
        new_TestFixture(test_hashes_sha512_hash_clear_ctx),
    };

    EMB_UNIT_TESTCALLER(hashes_sha512_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha512_tests;
}
