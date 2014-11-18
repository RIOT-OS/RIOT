/*
 * Copyright (C) 2014 Martine Lenders <mail@martine-lenders.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    tests-sixlowpan_iphc_cbuf.c
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "ipv6/addr.h"
#include "sixlowpan/iphc_cbuf.h"

#include "tests-sixlowpan_iphc_cbuf.h"

static void set_up(void)
{
    for (int cid = 0; cid < SIXLOWPAN_IPHC_CBUF_SIZE; cid++) {
        sixlowpan_iphc_cbuf_remove_by_cid(cid);
    }
}

static void test_sixlowpan_iphc_cbuf_lookup_addr_empty(void)
{
    TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_lookup_addr(TEST_IP1));
}

static void test_sixlowpan_iphc_cbuf_lookup_addr_no_fitting_addr(void)
{
    if (memcmp(TEST_IP1, TEST_IP2, sizeof(ipv6_addr_t)) == 0) {
        TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                             TEST_IP1_LEN, TEST_LIFETIME1));
        TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_lookup_addr(TEST_IP2));
    }
}

static void test_sixlowpan_iphc_cbuf_lookup_addr_success(void)
{
    sixlowpan_iphc_cbuf_t *c1, *c2;
    uint8_t mask = (uint8_t)(0xff << (8 - (TEST_IP1_LEN - ((TEST_IP1_LEN / 8) * 8))));
    ipv6_addr_t *prefix = TEST_IP1;

    c1 = sixlowpan_iphc_cbuf_update(TEST_CID1, prefix, TEST_IP1_LEN,
                                    TEST_LIFETIME1);
    TEST_ASSERT_NOT_NULL(c1);
    c2 = sixlowpan_iphc_cbuf_lookup_addr(prefix);
    TEST_ASSERT(c1 == c2);
    TEST_ASSERT_EQUAL_INT(TEST_CID1, c1->cid);
    TEST_ASSERT_EQUAL_INT(TEST_IP1_LEN, c1->prefix_len);
    TEST_ASSERT_EQUAL_INT(TEST_LIFETIME1, c1->lifetime);
    TEST_ASSERT_EQUAL_INT(0, memcmp(prefix, &(c1->prefix),  TEST_IP1_LEN / 8));
    TEST_ASSERT_EQUAL_INT((prefix->u8[TEST_IP1_LEN / 8] & mask),
                          (c1->prefix.u8[TEST_IP1_LEN / 8] & mask));
}

static void test_sixlowpan_iphc_cbuf_lookup_cid_empty(void)
{
    TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_lookup_cid(TEST_CID1));
}

static void test_sixlowpan_iphc_cbuf_lookup_cid_no_fitting_context(void)
{
    if (TEST_CID1 != TEST_CID2) {
        TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                             TEST_IP1_LEN, TEST_LIFETIME1));
        TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_lookup_cid(TEST_CID2));
    }
}

static void test_sixlowpan_iphc_cbuf_lookup_cid_success(void)
{
    sixlowpan_iphc_cbuf_t *c1, *c2;
    uint8_t mask = (uint8_t)(0xff << (8 - (TEST_IP1_LEN - ((TEST_IP1_LEN / 8) * 8))));
    ipv6_addr_t *prefix = TEST_IP1;

    c1 = sixlowpan_iphc_cbuf_update(TEST_CID1, prefix, TEST_IP1_LEN,
                                    TEST_LIFETIME1);
    TEST_ASSERT_NOT_NULL(c1);
    c2 = sixlowpan_iphc_cbuf_lookup_cid(TEST_CID1);
    TEST_ASSERT(c1 == c2);
    TEST_ASSERT_EQUAL_INT(TEST_CID1, c1->cid);
    TEST_ASSERT_EQUAL_INT(TEST_IP1_LEN, c1->prefix_len);
    TEST_ASSERT_EQUAL_INT(TEST_LIFETIME1, c1->lifetime);
    TEST_ASSERT_EQUAL_INT(0, memcmp(prefix, &(c1->prefix),  TEST_IP1_LEN / 8));
    TEST_ASSERT_EQUAL_INT((prefix->u8[TEST_IP1_LEN / 8] & mask),
                          (c1->prefix.u8[TEST_IP1_LEN / 8] & mask));
}

static void test_sixlowpan_iphc_cbuf_num_empty(void)
{
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_num_one_item(void)
{
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                         TEST_IP1_LEN, TEST_LIFETIME1));
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_num_two_items(void)
{
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                         TEST_IP1_LEN, TEST_LIFETIME1));
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID2, TEST_IP2,
                         TEST_IP2_LEN, TEST_LIFETIME2));

    if (TEST_CID1 == TEST_CID2) {
        TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
    }
    else {
        TEST_ASSERT_EQUAL_INT(2, sixlowpan_iphc_cbuf_num());
    }
}

static void test_sixlowpan_iphc_cbuf_remove_by_cid_empty(void)
{
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_cid(TEST_CID2);
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_remove_by_cid_no_fitting_context(void)
{
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID2, TEST_IP2,
                         TEST_IP1_LEN, TEST_LIFETIME1));
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_cid(TEST_CID1);
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_remove_by_cid_success(void)
{
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                         TEST_IP1_LEN, TEST_LIFETIME1));
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID2, TEST_IP2,
                         TEST_IP2_LEN, TEST_LIFETIME2));
    TEST_ASSERT_EQUAL_INT(2, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_cid(TEST_CID1);
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_cid(TEST_CID2);
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_remove_by_addr_empty(void)
{
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_addr(TEST_IP1);
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_remove_by_addr_no_fitting_addr(void)
{
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID2, TEST_IP2,
                         TEST_IP1_LEN, TEST_LIFETIME1));
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_addr(TEST_IP1);
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_remove_by_addr_success(void)
{
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                         TEST_IP1_LEN, TEST_LIFETIME1));
    TEST_ASSERT_NOT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID2, TEST_IP2,
                         TEST_IP2_LEN, TEST_LIFETIME2));
    TEST_ASSERT_EQUAL_INT(2, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_addr(TEST_IP2);
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
    sixlowpan_iphc_cbuf_remove_by_addr(TEST_IP1);
    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
}

static void test_sixlowpan_iphc_cbuf_update_illegal_cid(void)
{
    TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1 + 16, TEST_IP1,
                     TEST_IP1_LEN, TEST_LIFETIME1));
}

static void test_sixlowpan_iphc_cbuf_update_illegal_prefix_len(void)
{
    TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                     0, TEST_LIFETIME1));
}

static void test_sixlowpan_iphc_cbuf_update_illegal_lifetime(void)
{
    TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1, TEST_IP1,
                     TEST_IP1_LEN, 0));
}

static void test_sixlowpan_iphc_cbuf_update_just_wrong(void)
{
    TEST_ASSERT_NULL(sixlowpan_iphc_cbuf_update(TEST_CID1 + 16, TEST_IP1, 0, 0));
}

static void test_sixlowpan_iphc_cbuf_update_success(void)
{
    sixlowpan_iphc_cbuf_t *c1, *c2;
    uint8_t mask1 = (uint8_t)(0xff << (8 - (TEST_IP1_LEN - ((TEST_IP1_LEN / 8) * 8))));
    uint8_t mask2 = (uint8_t)(0xff << (8 - (TEST_IP2_LEN - ((TEST_IP2_LEN / 8) * 8))));
    ipv6_addr_t *prefix1 = TEST_IP1, *prefix2 = TEST_IP2;

    TEST_ASSERT_EQUAL_INT(0, sixlowpan_iphc_cbuf_num());
    c1 = sixlowpan_iphc_cbuf_update(TEST_CID1, prefix1, TEST_IP1_LEN,
                                    TEST_LIFETIME1);
    TEST_ASSERT_NOT_NULL(c1);
    TEST_ASSERT_EQUAL_INT(1, sixlowpan_iphc_cbuf_num());
    TEST_ASSERT_EQUAL_INT(TEST_CID1, c1->cid);
    TEST_ASSERT_EQUAL_INT(TEST_IP1_LEN, c1->prefix_len);
    TEST_ASSERT_EQUAL_INT(TEST_LIFETIME1, c1->lifetime);
    TEST_ASSERT_EQUAL_INT(0, memcmp(prefix1, &(c1->prefix),  TEST_IP1_LEN / 8));
    TEST_ASSERT_EQUAL_INT((prefix1->u8[TEST_IP1_LEN / 8] & mask1),
                          (c1->prefix.u8[TEST_IP1_LEN / 8] & mask1));

    c2 = sixlowpan_iphc_cbuf_update(TEST_CID2, prefix2, TEST_IP2_LEN,
                                    TEST_LIFETIME2);
    TEST_ASSERT_NOT_NULL(c2);
    TEST_ASSERT_EQUAL_INT(2, sixlowpan_iphc_cbuf_num());

    if (TEST_CID1 == TEST_CID2) {
        TEST_ASSERT(c1 == c2);
    }
    else {
        TEST_ASSERT(c1 != c2);
    }

    if (TEST_CID1 != TEST_CID2) {
        TEST_ASSERT_EQUAL_INT(TEST_CID1, c1->cid);
        TEST_ASSERT_EQUAL_INT(TEST_IP1_LEN, c1->prefix_len);
        TEST_ASSERT_EQUAL_INT(TEST_LIFETIME1, c1->lifetime);
        TEST_ASSERT_EQUAL_INT(0, memcmp(prefix1, &(c1->prefix),  TEST_IP1_LEN / 8));
        TEST_ASSERT_EQUAL_INT((prefix1->u8[TEST_IP1_LEN / 8] & mask1),
                              (c1->prefix.u8[TEST_IP1_LEN / 8] & mask1));
    }

    TEST_ASSERT_EQUAL_INT(TEST_CID2, c2->cid);
    TEST_ASSERT_EQUAL_INT(TEST_IP2_LEN, c2->prefix_len);
    TEST_ASSERT_EQUAL_INT(TEST_LIFETIME2, c2->lifetime);
    TEST_ASSERT_EQUAL_INT(0, memcmp(prefix2, &(c2->prefix),  TEST_IP2_LEN / 8));
    TEST_ASSERT_EQUAL_INT((prefix2->u8[TEST_IP2_LEN / 8] & mask2),
                          (c2->prefix.u8[TEST_IP2_LEN / 8] & mask2));
}

Test *tests_sixlowpan_iphc_cbuf_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sixlowpan_iphc_cbuf_lookup_addr_empty),
        new_TestFixture(test_sixlowpan_iphc_cbuf_lookup_addr_no_fitting_addr),
        new_TestFixture(test_sixlowpan_iphc_cbuf_lookup_addr_success),
        new_TestFixture(test_sixlowpan_iphc_cbuf_lookup_cid_empty),
        new_TestFixture(test_sixlowpan_iphc_cbuf_lookup_cid_no_fitting_context),
        new_TestFixture(test_sixlowpan_iphc_cbuf_lookup_cid_success),
        new_TestFixture(test_sixlowpan_iphc_cbuf_num_empty),
        new_TestFixture(test_sixlowpan_iphc_cbuf_num_one_item),
        new_TestFixture(test_sixlowpan_iphc_cbuf_num_two_items),
        new_TestFixture(test_sixlowpan_iphc_cbuf_remove_by_cid_empty),
        new_TestFixture(test_sixlowpan_iphc_cbuf_remove_by_cid_no_fitting_context),
        new_TestFixture(test_sixlowpan_iphc_cbuf_remove_by_cid_success),
        new_TestFixture(test_sixlowpan_iphc_cbuf_remove_by_addr_empty),
        new_TestFixture(test_sixlowpan_iphc_cbuf_remove_by_addr_no_fitting_addr),
        new_TestFixture(test_sixlowpan_iphc_cbuf_remove_by_addr_success),
        new_TestFixture(test_sixlowpan_iphc_cbuf_update_illegal_cid),
        new_TestFixture(test_sixlowpan_iphc_cbuf_update_illegal_prefix_len),
        new_TestFixture(test_sixlowpan_iphc_cbuf_update_illegal_lifetime),
        new_TestFixture(test_sixlowpan_iphc_cbuf_update_just_wrong),
        new_TestFixture(test_sixlowpan_iphc_cbuf_update_success),
    };

    EMB_UNIT_TESTCALLER(sixlowpan_iphc_cbuf_tests, set_up, NULL, fixtures);

    return (Test *)&sixlowpan_iphc_cbuf_tests;
}

void tests_sixlowpan_iphc_cbuf(void)
{
    TESTS_RUN(tests_sixlowpan_iphc_cbuf_tests());
}
/** @} */
