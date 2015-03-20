/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "embUnit/embUnit.h"

#include "crypto/morph25519.h"
#include "crypto/c25519.h"
#include "crypto/f25519.h"
#include "crypto/ed25519.h"
#include "tests-crypto.h"
#include "tests-crypto-morph25519.h"

static int test_morph(const uint8_t *mx, const uint8_t *ex, const uint8_t *ey)
{
    const int parity = morph25519_eparity(ex);
    uint8_t mx_test[F25519_SIZE];
    uint8_t ex_test[F25519_SIZE];
    uint8_t ey_test[F25519_SIZE];

    morph25519_e2m(mx_test, ey);
    morph25519_m2e(ex_test, ey_test, mx, parity);

    if (!f25519_eq(mx_test, mx)) {
        return 0;
    }

    if (!f25519_eq(ex_test, ex)) {
        return 0;
    }

    if (!f25519_eq(ey_test, ey)) {
        return 0;
    }

    return 1;
}

static int test_sm(char *random)
{
    uint8_t e[C25519_EXPONENT_SIZE];
    uint8_t mx[F25519_SIZE];
    uint8_t ex[F25519_SIZE];
    uint8_t ey[F25519_SIZE];
    struct ed25519_pt p;
    unsigned int i;

    for (i = 0; i < sizeof(e); i++) {
        e[i] = random[i];
    }

    c25519_prepare(e);
    c25519_smult(mx, c25519_base_x, e);

    ed25519_smult(&p, &ed25519_base, e);
    ed25519_unproject(ex, ey, &p);

    return test_morph(mx, ex, ey);
}

static void test_crypto_morph25519_morph(void)
{
    TEST_ASSERT(test_morph(c25519_base_x, ed25519_base.x, ed25519_base.y));
}

static void test_crypto_morph25519_sm(void)
{
    TEST_ASSERT(test_sm(TV1_RANDOM));
    TEST_ASSERT(test_sm(TV2_RANDOM));
    TEST_ASSERT(test_sm(TV3_RANDOM));
}

Test *tests_crypto_morph25519_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_morph25519_morph),
        new_TestFixture(test_crypto_morph25519_sm),
    };

    EMB_UNIT_TESTCALLER(crypto_morph25519_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_morph25519_tests;
}
