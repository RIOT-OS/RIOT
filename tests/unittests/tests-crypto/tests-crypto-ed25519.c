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

#include "crypto/ed25519.h"
#include "tests-crypto.h"
#include "tests-crypto-ed25519.h"

static int check_valid(const uint8_t *x, const uint8_t *y)
{
    uint8_t xsq[F25519_SIZE];
    uint8_t ysq[F25519_SIZE];
    uint8_t m[F25519_SIZE];
    uint8_t dxsqysq[F25519_SIZE];
    uint8_t lhs[F25519_SIZE];
    uint8_t rhs[F25519_SIZE];

    f25519_mul__distinct(xsq, x, x);
    f25519_mul__distinct(ysq, y, y);
    f25519_mul__distinct(m, xsq, ysq);
    f25519_mul__distinct(dxsqysq, m, curve_d);

    f25519_sub(lhs, ysq, xsq);
    f25519_load(rhs, 1);
    f25519_add(rhs, rhs, dxsqysq);

    f25519_normalize(lhs);
    f25519_normalize(rhs);

    if (memcmp(lhs, rhs, sizeof(rhs)) == 0) {
        return 1;
    }

    return 0;
}

static int test_pack(void)
{
    uint8_t x1[F25519_SIZE];
    uint8_t y1[F25519_SIZE];
    uint8_t pack[F25519_SIZE];
    uint8_t x2[F25519_SIZE];
    uint8_t y2[F25519_SIZE];
    struct ed25519_pt p;
    uint8_t s;

    ed25519_smult(&p, &ed25519_base, (uint8_t *)TV1_E);
    ed25519_unproject(x1, y1, &p);
    f25519_normalize(x1);
    f25519_normalize(y1);

    check_valid(x1, y1);

    ed25519_pack(pack, x1, y1);
    s = ed25519_try_unpack(x2, y2, pack);

    if (s == 0) {
        return 0;
    }

    if (f25519_eq(x1, x2) != 1) {
        return 0;
    }

    if (f25519_eq(y1, y2) != 1) {
        return 0;
    }

    return 1;
}


static int test_dh(void)
{
    uint8_t p1x[F25519_SIZE];
    uint8_t p1y[F25519_SIZE];
    uint8_t p2x[F25519_SIZE];
    uint8_t p2y[F25519_SIZE];
    uint8_t s1x[F25519_SIZE];
    uint8_t s1y[F25519_SIZE];
    uint8_t s2x[F25519_SIZE];
    uint8_t s2y[F25519_SIZE];
    struct ed25519_pt p;

    /* Generate public key 1 */
    ed25519_smult(&p, &ed25519_base, (uint8_t *)TV1_E);
    ed25519_unproject(p1x, p1y, &p);

    /* Generate public key 2 */
    ed25519_smult(&p, &ed25519_base, (uint8_t *)TV2_E);
    ed25519_unproject(p2x, p2y, &p);

    /* Generate shared secret 1 */
    ed25519_project(&p, p1x, p1y);
    ed25519_smult(&p, &p, (uint8_t *)TV2_E);
    ed25519_unproject(s1x, s1y, &p);

    /* Generate shared secret 2 */
    ed25519_project(&p, p2x, p2y);
    ed25519_smult(&p, &p, (uint8_t *)TV1_E);
    ed25519_unproject(s2x, s2y, &p);

    /* Should be the same point */
    f25519_normalize(s1x);
    f25519_normalize(s1y);
    f25519_normalize(s2x);
    f25519_normalize(s2y);

    if (f25519_eq(s1x, s2x) != 1) {
        return 0;
    }

    if (f25519_eq(s1y, s2y) != 1) {
        return 0;
    }

    return 1;
}

static int test_add(void)
{
    struct ed25519_pt p;
    uint8_t ax[F25519_SIZE];
    uint8_t ay[F25519_SIZE];
    uint8_t bx[F25519_SIZE];
    uint8_t by[F25519_SIZE];

    ed25519_double(&p, &ed25519_base);
    ed25519_unproject(ax, ay, &p);
    check_valid(ax, ay);

    ed25519_add(&p, &ed25519_neutral, &ed25519_base);
    ed25519_add(&p, &p, &ed25519_base);
    ed25519_unproject(bx, by, &p);
    check_valid(bx, by);

    f25519_normalize(ax);
    f25519_normalize(ay);
    f25519_normalize(bx);
    f25519_normalize(by);

    if (f25519_eq(ax, bx) != 1) {
        return 0;
    }

    if (f25519_eq(ay, by) != 1) {
        return 0;
    }

    return 1;
}

static int test_order(void)
{
    static const uint8_t zero[ED25519_EXPONENT_SIZE] = {0};
    uint8_t x[F25519_SIZE];
    uint8_t y[F25519_SIZE];
    uint8_t b1[F25519_SIZE];
    uint8_t b2[F25519_SIZE];
    struct ed25519_pt p;

    ed25519_smult(&p, &ed25519_base, ed25519_order);
    ed25519_unproject(x, y, &p);
    ed25519_pack(b1, x, y);

    ed25519_smult(&p, &ed25519_base, zero);
    ed25519_unproject(x, y, &p);
    ed25519_pack(b2, x, y);

    if (f25519_eq(b1, b2) != 1) {
        return 0;
    }

    return 1;
}

static void test_crypto_ed25519_checks(void)
{
    TEST_ASSERT(check_valid(ed25519_neutral.x, ed25519_neutral.y));
    TEST_ASSERT(check_valid(ed25519_base.x, ed25519_base.y));
}

static void test_crypto_ed25519_adds(void)
{
    TEST_ASSERT(test_add());
}

static void test_crypto_ed25519_orders(void)
{
    TEST_ASSERT(test_order());
}

static void test_crypto_ed25519_packs(void)
{
    TEST_ASSERT(test_pack());
}

static void test_crypto_ed25519_dhs(void)
{
    TEST_ASSERT(test_dh());
}

Test *tests_crypto_ed25519_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_ed25519_checks),
        new_TestFixture(test_crypto_ed25519_adds),
        new_TestFixture(test_crypto_ed25519_orders),
        new_TestFixture(test_crypto_ed25519_packs),
        new_TestFixture(test_crypto_ed25519_dhs),
    };

    EMB_UNIT_TESTCALLER(crypto_ed25519_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_ed25519_tests;
}
