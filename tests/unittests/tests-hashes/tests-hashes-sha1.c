/*
 * Copyright (C) 2016 Oliver Hahm <oliver.hahm@inria.fr>
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
 * @brief       Test cases for the SHA-1 hash implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "hashes/sha1.h"

#define TEST_CASES_NUM      (4)
/*
 *  *  Define patterns for testing
 *   */
#define TEST1   "abc"
#define TEST2a  "abcdbcdecdefdefgefghfghighijhi"
#define TEST2b  "jkijkljklmklmnlmnomnopnopq"
#define TEST2   TEST2a TEST2b
#define TEST3   "a"
#define TEST4a  "01234567012345670123456701234567"
#define TEST4b  "01234567012345670123456701234567"
/* an exact multiple of 512 bits */
#define TEST4   TEST4a TEST4b

static char *_testarray[TEST_CASES_NUM] =
{
    TEST1,
    TEST2,
    TEST3,
    TEST4
};

static long int _repeatcount[TEST_CASES_NUM] = { 1, 1, 1000000, 10 };

static char *_resultarray[TEST_CASES_NUM + 2] =
{
    "A9 99 3E 36 47 06 81 6A BA 3E 25 71 78 50 C2 6C 9C D0 D8 9D",
    "84 98 3E 44 1C 3B D2 6E BA AE 4A A1 F9 51 29 E5 E5 46 70 F1",
    /* for repeatcount == 1000000 */
    "34 AA 97 3C D4 C4 DA A4 F6 1E EB 2B DB AD 27 31 65 34 01 6F",
    /* for repeatcount == 10*/
    "DE A3 56 A2 CD DD 90 C7 A7 EC ED C5 EB B5 63 93 4F 46 04 52",
    /* for single runs */
    "86 F7 E4 37 FA A5 A7 FC E1 5D 1D DC B9 EA EA EA 37 76 67 B8",
    "E0 C0 94 E8 67 EF 46 C3 50 EF 54 A7 F5 9D D6 0B ED 92 AE 83"
};

#define TEST_CASES_HMAC_NUM      (5)

#define TEST1_HMAC	"Hi There"
#define TEST2_HMAC	"what do ya want for nothing?"
#define TEST3_HMAC  "Test With Truncation"
#define TEST4_HMAC  "Test Using Larger Than Block-Size Key - Hash Key First"
#define TEST5_HMAC  "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data"

static uint8_t _hmac_key1[]={
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};
static uint8_t _hmac_key2[]= "Jefe";
static uint8_t _hmac_key3[]={
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c
};
static uint8_t _hmac_key4[]={
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
};
static uint8_t _hmac_key5[]={
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
};

static char *_resultarray_hmac[TEST_CASES_HMAC_NUM] =
{
    "B6 17 31 86 55 05 72 64 E2 8B C0 B6 FB 37 8C 8E F1 46 BE 00",
    "EF FC DF 6A E5 EB 2F A2 D2 74 16 D5 F1 84 DF 9C 25 9A 7C 79",
    "4C 1A 03 42 4B 55 E0 7F E7 F2 7B E1 D5 8B B9 32 4A 9A 5A 04",
    "AA 4A E5 E1 52 72 D0 0E 95 70 56 37 CE 8A 3B 55 ED 40 21 12",
    "E8 E9 9D 0F 45 23 7D 78 6D 6B BA A7 96 5C 78 08 BB FF 1A 91 "
};

static int calc_and_compare_hash(const char *str, const char *expected,
                                 long int repeatcount)
{
    sha1_context ctx;

    uint8_t *hash;
    char tmp[(3 * SHA1_DIGEST_LENGTH) + 1];

    /* calculate hash */
    sha1_init(&ctx);
    for (long int i = 0; i < repeatcount; ++i) {
        sha1_update(&ctx, (unsigned char*) str, strlen(str));
    }
    hash = sha1_final(&ctx);
    /* copy hash to string */
    for (size_t i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        sprintf(&(tmp[i * 3]), "%02X ", (unsigned) hash[i]);
    }
    tmp[SHA1_DIGEST_LENGTH* 2] = '\0';
    /* compare with result string */
    return strncmp(tmp, expected, strlen((char*) tmp));
}

static int calc_and_compare_hash2(const char *str, const char *expected)
{
    char tmp[(3 * SHA1_DIGEST_LENGTH) + 1];
    uint8_t hash[SHA1_DIGEST_LENGTH];
    sha1((unsigned char*) hash, (unsigned char*) str, strlen(str));

    /* copy hash to string */
    for (size_t i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        sprintf(&(tmp[i * 3]), "%02X ", (unsigned) hash[i]);
    }
    tmp[SHA1_DIGEST_LENGTH* 2] = '\0';
    /* compare with result string */
    return strncmp(tmp, expected, strlen((char*) tmp));
}

static int calc_and_compare_hash_hmac(const char *str, const char *expected,
                                      const uint8_t *key, size_t key_len)
{
    sha1_context ctx;

    uint8_t *hash;
    char tmp[(3 * SHA1_DIGEST_LENGTH) + 1];

    /* calculate hash */
    sha1_init_hmac(&ctx, key, key_len);
    sha1_update(&ctx, (unsigned char*) str, strlen(str));

    hash = sha1_final_hmac(&ctx);
    /* copy hash to string */
    for (size_t i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        sprintf(&(tmp[i * 3]), "%02X ", (unsigned) hash[i]);
    }
    tmp[SHA1_DIGEST_LENGTH* 2] = '\0';
    /* compare with result string */
    return strncmp(tmp, expected, strlen((char*) tmp));
}
/* test cases copied from section 7.3 of RFC 3174
 * https://tools.ietf.org/html/rfc3174#section-7.3
 */
static void test_hashes_sha1(void)
{
    TEST_ASSERT(calc_and_compare_hash(_testarray[0], _resultarray[0], _repeatcount[0]) == 0);
    TEST_ASSERT(calc_and_compare_hash(_testarray[1], _resultarray[1], _repeatcount[1]) == 0);
    TEST_ASSERT(calc_and_compare_hash(_testarray[2], _resultarray[2], _repeatcount[2]) == 0);
    TEST_ASSERT(calc_and_compare_hash(_testarray[3], _resultarray[3], _repeatcount[3]) == 0);
    TEST_ASSERT(calc_and_compare_hash2(_testarray[0], _resultarray[0]) == 0);
    TEST_ASSERT(calc_and_compare_hash2(_testarray[1], _resultarray[1]) == 0);
    TEST_ASSERT(calc_and_compare_hash2(_testarray[2], _resultarray[4]) == 0);
    TEST_ASSERT(calc_and_compare_hash2(_testarray[3], _resultarray[5]) == 0);

    TEST_ASSERT(calc_and_compare_hash_hmac(TEST1_HMAC, _resultarray_hmac[0], _hmac_key1, sizeof(_hmac_key1)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST2_HMAC, _resultarray_hmac[1], _hmac_key2, sizeof(_hmac_key2)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST3_HMAC, _resultarray_hmac[2], _hmac_key3, sizeof(_hmac_key3)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST4_HMAC, _resultarray_hmac[3], _hmac_key4, sizeof(_hmac_key4)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST5_HMAC, _resultarray_hmac[4], _hmac_key5, sizeof(_hmac_key5)) == 0);
}

Test *tests_hashes_sha1_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha1),
    };

    EMB_UNIT_TESTCALLER(test_hashes_sha1, NULL, NULL, fixtures);

    return (Test *)&test_hashes_sha1;
}
