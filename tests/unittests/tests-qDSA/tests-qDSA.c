/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       qDSA crypto library tests
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "embUnit.h"
#include "tests-qDSA.h"

#include "random.h"
#include "sign.h"

static const unsigned char m[] = "0123456789abcdef";

#define SMLEN (sizeof(m) + 64)

static unsigned char sm[SMLEN];
static unsigned char m_result[sizeof(m)];
static unsigned char sk[64];
static unsigned char pk[32];

static void setUp(void)
{
    /* Initialize */
    random_init(0);
}

static void tearDown(void)
{
    /* Finalize */
}

static void test_qDSA_sign_verify(void)
{
    unsigned long long smlen;

    random_bytes(sk, 32);
    keypair(pk, sk);

    sign(sm, &smlen, m, sizeof(m), pk, sk);

    TEST_ASSERT_EQUAL_INT(SMLEN, smlen);
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", (const char *)(sm + 64));

    TEST_ASSERT_EQUAL_INT(0, verify(m_result, 0, sm, smlen, pk));
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", (const char *)m_result);

    sm[70] = 'x';

    TEST_ASSERT_EQUAL_INT(1, verify(m_result, 0, sm, smlen, pk));
}

Test *tests_qDSA_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_qDSA_sign_verify),
    };

    EMB_UNIT_TESTCALLER(qDSA_tests, setUp, tearDown, fixtures);
    return (Test*)&qDSA_tests;
}

void tests_qDSA(void)
{
    TESTS_RUN(tests_qDSA_all());
}
