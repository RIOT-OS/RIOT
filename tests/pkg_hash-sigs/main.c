/*
 * Copyright (C) 2021 Koen Zandberg
 *               2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests hash-sigs package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "hash_sig_api.h"
#include "embUnit.h"

#define MESSAGE_SIZE 17

static void test_key_gen(void)
{
    unsigned char sk[PRIVATE_KEY_LEN] = { 0 };
    unsigned char pk[HSS_MAX_PUBLIC_KEY_LEN] = { 0 };

    int return_key_gen = keygen(sk, pk);

    TEST_ASSERT_EQUAL_INT(return_key_gen, 1);
}

static void test_hash_sigs(void)
{
    unsigned char sk[PRIVATE_KEY_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
                                          0x0, 0x53, 0x53, 0xff, 0xff, 0xff,
                                          0xff, 0xff, 0xff, 0x59, 0x68,
                                          0x3a, 0xca, 0xc8, 0xfe, 0x75, 0x5a,
                                          0x30, 0x7e, 0xbf, 0x2d, 0xb4,
                                          0xb9, 0xee, 0xf0, 0xd9, 0xb1, 0xa9,
                                          0x2c, 0xa6, 0x5f, 0xb2, 0x6b,
                                          0xf0, 0x4f, 0xed, 0x80, 0x44, 0x36,
                                          0x1, 0x4a, 0x83, 0xbf, 0xbd,
                                          0x5e, 0x7f, 0xaa, 0x6f, 0xbd, 0xdc,
                                          0x6f, 0xfb, 0xe6, 0xc, 0x38,
                                          0x17, 0x9d };

    unsigned char pk[HSS_MAX_PUBLIC_KEY_LEN] = { 0x0, 0x0, 0x0, 0x2, 0x0, 0x0,
                                                 0x0, 0x5, 0x0, 0x0, 0x0, 0x3,
                                                 0x83, 0xbf, 0xbd, 0x5e, 0x7f,
                                                 0xaa,
                                                 0x6f, 0xbd, 0xdc, 0x6f, 0xfb,
                                                 0xe6, 0xc, 0x38, 0x17, 0x9d,
                                                 0x8f,
                                                 0x90, 0x1d, 0x46, 0x6f, 0x92,
                                                 0xcd, 0xe8, 0x21, 0x14, 0x91,
                                                 0xe3,
                                                 0xe3, 0xcc, 0x3c, 0x92, 0xf5,
                                                 0x7b, 0xc6, 0xbe, 0xb9, 0x42,
                                                 0xb5,
                                                 0x65, 0x5a, 0xcd, 0x8d, 0xf1,
                                                 0xc7, 0xc3, 0x7, 0x65 };

    unsigned char message[MESSAGE_SIZE] = "This should work!";
    unsigned char signature[CRYPTO_BYTES + MESSAGE_SIZE];

    unsigned long long length = 0;

    int return_sign = sign(signature, &length, message, MESSAGE_SIZE, sk);

    TEST_ASSERT_EQUAL_INT(return_sign, 1);
    TEST_ASSERT_EQUAL_INT(length, CRYPTO_BYTES);

    int return_verify = verify(pk, signature, length, message, MESSAGE_SIZE);

    TEST_ASSERT_EQUAL_INT(return_verify, 1);
}

Test *tests_hash_sigs(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_key_gen),
        new_TestFixture(test_hash_sigs)
    };

    EMB_UNIT_TESTCALLER(hash_sigs_tests, NULL, NULL, fixtures);
    return (Test *)&hash_sigs_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_hash_sigs());
    TESTS_END();

    return 0;
}
