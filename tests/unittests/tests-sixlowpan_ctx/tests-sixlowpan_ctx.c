/*
 * SPDX-FileCopyrightText: 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include "embUnit.h"

#include "net/ipv6/addr.h"
#include "net/gnrc/sixlowpan/ctx.h"

#include "unittests-constants.h"
#include "tests-sixlowpan_ctx.h"

#define DEFAULT_TEST_ID         (5)
#define DEFAULT_TEST_PREFIX { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }
#define DEFAULT_TEST_PREFIX_LEN (63)

#define OTHER_TEST_ID         (12)
#define OTHER_TEST_PREFIX   { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x06, \
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f \
        } \
    }
#define WRONG_TEST_PREFIX   { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x05, \
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f \
        } \
    }

static void tear_down(void)
{
    gnrc_sixlowpan_ctx_reset();
}

static void test_sixlowpan_ctx_update__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;

    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_update(DEFAULT_TEST_ID, &addr,
                                                   DEFAULT_TEST_PREFIX_LEN,
                                                   TEST_UINT16, true));
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
}

static void test_sixlowpan_ctx_update__ltime0(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;

    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_update(DEFAULT_TEST_ID, &addr,
                                                   DEFAULT_TEST_PREFIX_LEN,
                                                   0, true));
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_ID, gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID)->flags_id);
}

static void test_sixlowpan_ctx_update__wrong_id1(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;

    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    /* GNRC_SIXLOWPAN_CTX_SIZE out of bound so neither context update nor lookup
     * should not be possible */
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_update(GNRC_SIXLOWPAN_CTX_SIZE, &addr,
                                               DEFAULT_TEST_PREFIX_LEN, TEST_UINT16, true));
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_id(GNRC_SIXLOWPAN_CTX_SIZE));

    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_update(DEFAULT_TEST_ID, &addr, DEFAULT_TEST_PREFIX_LEN,
                                                   TEST_UINT16, true));
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
}

static void test_sixlowpan_ctx_update__wrong_id2(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;

    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    /* UINT8_MAX out of bound so neither context update nor lookup should not
     * be possible */
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_update(UINT8_MAX, &addr, DEFAULT_TEST_PREFIX_LEN,
                                               TEST_UINT16, true));
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_id(UINT8_MAX));

    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_update(DEFAULT_TEST_ID, &addr, DEFAULT_TEST_PREFIX_LEN,
                                                   TEST_UINT16, true));
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
}

static void test_sixlowpan_ctx_update__wrong_prefix_len(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;

    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_update(DEFAULT_TEST_ID, &addr, 0, TEST_UINT16, true));
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
}

static void test_sixlowpan_ctx_lookup_addr__empty(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;

    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_addr(&addr));
}

static void test_sixlowpan_ctx_lookup_addr__same_addr(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;
    gnrc_sixlowpan_ctx_t *ctx;

    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    TEST_ASSERT_NOT_NULL((ctx = gnrc_sixlowpan_ctx_lookup_addr(&addr)));
    TEST_ASSERT_EQUAL_INT(GNRC_SIXLOWPAN_CTX_FLAGS_COMP | DEFAULT_TEST_ID, ctx->flags_id);
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_PREFIX_LEN, ctx->prefix_len);
    TEST_ASSERT(TEST_UINT16 >= ctx->ltime);
    TEST_ASSERT(DEFAULT_TEST_PREFIX_LEN <= ipv6_addr_match_prefix(&addr, &ctx->prefix));
}

static void test_sixlowpan_ctx_lookup_addr__other_addr_same_prefix(void)
{
    ipv6_addr_t addr1 = DEFAULT_TEST_PREFIX;
    ipv6_addr_t addr2 = OTHER_TEST_PREFIX;
    gnrc_sixlowpan_ctx_t *ctx;

    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    TEST_ASSERT_NOT_NULL((ctx = gnrc_sixlowpan_ctx_lookup_addr(&addr2)));
    TEST_ASSERT_EQUAL_INT(GNRC_SIXLOWPAN_CTX_FLAGS_COMP | DEFAULT_TEST_ID, ctx->flags_id);
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_PREFIX_LEN, ctx->prefix_len);
    TEST_ASSERT(TEST_UINT16 >= ctx->ltime);
    TEST_ASSERT(DEFAULT_TEST_PREFIX_LEN <= ipv6_addr_match_prefix(&addr1, &ctx->prefix));
}

static void test_sixlowpan_ctx_lookup_addr__other_addr_other_prefix(void)
{
    ipv6_addr_t addr = WRONG_TEST_PREFIX;

    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_addr(&addr));
}

static void test_sixlowpan_ctx_lookup_id__empty(void)
{
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
}

static void test_sixlowpan_ctx_lookup_id__wrong_id(void)
{
    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_id(OTHER_TEST_ID));
}

static void test_sixlowpan_ctx_lookup_id__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;
    gnrc_sixlowpan_ctx_t *ctx;

    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    TEST_ASSERT_NOT_NULL((ctx = gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID)));
    TEST_ASSERT_EQUAL_INT(GNRC_SIXLOWPAN_CTX_FLAGS_COMP | DEFAULT_TEST_ID, ctx->flags_id);
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_PREFIX_LEN, ctx->prefix_len);
    TEST_ASSERT(TEST_UINT16 >= ctx->ltime);
    TEST_ASSERT(DEFAULT_TEST_PREFIX_LEN <= ipv6_addr_match_prefix(&addr, &ctx->prefix));
}

static void test_sixlowpan_ctx_remove(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_PREFIX;

    /* add context DEFAULT_TEST_PREFIX to DEFAULT_TEST_ID */
    test_sixlowpan_ctx_update__success();
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
    TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_ctx_lookup_addr(&addr));
    gnrc_sixlowpan_ctx_remove(DEFAULT_TEST_ID);
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_id(DEFAULT_TEST_ID));
    TEST_ASSERT_NULL(gnrc_sixlowpan_ctx_lookup_addr(&addr));
}

Test *tests_sixlowpan_ctx_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sixlowpan_ctx_update__wrong_id1),
        new_TestFixture(test_sixlowpan_ctx_update__wrong_id2),
        new_TestFixture(test_sixlowpan_ctx_update__wrong_prefix_len),
        new_TestFixture(test_sixlowpan_ctx_update__success),
        new_TestFixture(test_sixlowpan_ctx_update__ltime0),
        new_TestFixture(test_sixlowpan_ctx_lookup_addr__empty),
        new_TestFixture(test_sixlowpan_ctx_lookup_addr__same_addr),
        new_TestFixture(test_sixlowpan_ctx_lookup_addr__other_addr_same_prefix),
        new_TestFixture(test_sixlowpan_ctx_lookup_addr__other_addr_other_prefix),
        new_TestFixture(test_sixlowpan_ctx_lookup_id__empty),
        new_TestFixture(test_sixlowpan_ctx_lookup_id__wrong_id),
        new_TestFixture(test_sixlowpan_ctx_lookup_id__success),
        new_TestFixture(test_sixlowpan_ctx_remove),
    };

    EMB_UNIT_TESTCALLER(sixlowpan_ctx_tests, NULL, tear_down, fixtures);

    return (Test *)&sixlowpan_ctx_tests;
}

void tests_sixlowpan_ctx(void)
{
    TESTS_RUN(tests_sixlowpan_ctx_tests());
}
/** @} */
