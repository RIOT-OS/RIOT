/*
 * SPDX-FileCopyrightText: 2016 Oliver Hahm <oliver.hahm@inria.fr>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Test cases for the HMAC-SHA-1 implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "embUnit/embUnit.h"

#include "hashes/sha1.h"

#include "tests-hashes.h"

#define TEST_CASES_HMAC_NUM      (5)

#define TEST1_HMAC  "Hi There"
#define TEST2_HMAC  "what do ya want for nothing?"
#define TEST3_HMAC  "Test With Truncation"
#define TEST4_HMAC  "Test Using Larger Than Block-Size Key - Hash Key First"
#define TEST5_HMAC  "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data"

static const uint8_t _hmac_key1[]={
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};
static const uint8_t _hmac_key2[]= "Jefe";
static const uint8_t _hmac_key3[]={
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c
};
static const uint8_t _hmac_key4[]={
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
};
static const uint8_t _hmac_key5[]={
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
};

static const char *_resultarray_hmac[TEST_CASES_HMAC_NUM] =
{
    "B6 17 31 86 55 05 72 64 E2 8B C0 B6 FB 37 8C 8E F1 46 BE 00",
    "EF FC DF 6A E5 EB 2F A2 D2 74 16 D5 F1 84 DF 9C 25 9A 7C 79",
    "4C 1A 03 42 4B 55 E0 7F E7 F2 7B E1 D5 8B B9 32 4A 9A 5A 04",
    "AA 4A E5 E1 52 72 D0 0E 95 70 56 37 CE 8A 3B 55 ED 40 21 12",
    "E8 E9 9D 0F 45 23 7D 78 6D 6B BA A7 96 5C 78 08 BB FF 1A 91 "
};

static int calc_and_compare_hash_hmac(const char *str, const char *expected,
                                      const uint8_t *key, size_t key_len)
{
    sha1_context ctx;

    uint8_t hash[SHA1_DIGEST_LENGTH];
    char tmp[(3 * SHA1_DIGEST_LENGTH) + 1];

    /* calculate hash */
    sha1_init_hmac(&ctx, key, key_len);
    sha1_update(&ctx, (unsigned char*) str, strlen(str));

    sha1_final_hmac(&ctx, hash);
    /* copy hash to string */
    for (size_t i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        sprintf(&(tmp[i * 3]), "%02X ", (unsigned) hash[i]);
    }
    tmp[SHA1_DIGEST_LENGTH* 2] = '\0';

    /* compare with result string */
    return strncmp(tmp, expected, strlen((char*) tmp));
}

/* test cases from RFC 2202
 * https://tools.ietf.org/html/rfc2202
 */
static void test_hashes_sha1_hmac(void)
{
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST1_HMAC, _resultarray_hmac[0],
                                           _hmac_key1, sizeof(_hmac_key1)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST2_HMAC, _resultarray_hmac[1],
                                           _hmac_key2, sizeof(_hmac_key2)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST3_HMAC, _resultarray_hmac[2],
                                           _hmac_key3, sizeof(_hmac_key3)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST4_HMAC, _resultarray_hmac[3],
                                           _hmac_key4, sizeof(_hmac_key4)) == 0);
    TEST_ASSERT(calc_and_compare_hash_hmac(TEST5_HMAC, _resultarray_hmac[4],
                                           _hmac_key5, sizeof(_hmac_key5)) == 0);
}

static void test_hashes_sha1_hmac_wipe(void)
{
    sha1_context ctx;
    uint8_t digest[SHA1_DIGEST_LENGTH];
    static const uint8_t zeros_key[SHA1_BLOCK_LENGTH];
    static const uint8_t zeros_hash[SHA1_DIGEST_LENGTH];

    sha1_init_hmac(&ctx, _hmac_key1, sizeof(_hmac_key1));
    sha1_update(&ctx, (unsigned char *)TEST1_HMAC, strlen(TEST1_HMAC));
    sha1_final_hmac(&ctx, digest);

    TEST_ASSERT(memcmp(ctx.key_buffer, zeros_key, sizeof(ctx.key_buffer)) == 0);
    TEST_ASSERT(memcmp(ctx.inner_hash, zeros_hash, sizeof(ctx.inner_hash)) == 0);
}

Test *tests_hashes_sha1_hmac_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha1_hmac),
        new_TestFixture(test_hashes_sha1_hmac_wipe),
    };

    EMB_UNIT_TESTCALLER(test_hashes_sha1_hmac, NULL, NULL, fixtures);

    return (Test *)&test_hashes_sha1_hmac;
}
