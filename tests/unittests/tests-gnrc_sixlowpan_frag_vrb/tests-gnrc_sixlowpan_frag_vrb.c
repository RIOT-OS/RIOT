/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @file
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "net/gnrc/sixlowpan/frag/fb.h"
#include "net/gnrc/sixlowpan/frag/vrb.h"
#include "xtimer.h"

#include "tests-gnrc_sixlowpan_frag_vrb.h"

#define TEST_TAG_INITIAL    (6U)
#define TEST_SRC            { 0x68, 0xDF, 0x2A, 0x5B, 0x21, 0x7D, 0xEE, 0xE5 }
#define TEST_DST            { 0xB1, 0x5B, 0x0F, 0x2F, 0x63, 0x76, 0x28, 0x39 }
#define TEST_OUT_DST        { 0x13, 0xF8, 0xEB, 0x75, 0x46, 0x60, 0x7C, 0x60 }
#define TEST_SRC_LEN        (8U)
#define TEST_DST_LEN        (8U)
#define TEST_TAG            (26U)

/* The interface is not used for anything by the VRB (it just is kept as a
 * reference for forwarding) so an uninitialized one is enough */
static gnrc_netif_t _dummy_netif;

static const gnrc_sixlowpan_frag_rb_int_t _interval = {
    .next = NULL,
    .start = 0,
    .end = 116U,
};
static const gnrc_sixlowpan_frag_rb_base_t _base = {
    .ints = (gnrc_sixlowpan_frag_rb_int_t *)&_interval,
    .src = TEST_SRC,
    .dst = TEST_DST,
    .src_len = TEST_SRC_LEN,
    .dst_len = TEST_DST_LEN,
    .tag = TEST_TAG,
    .datagram_size = 1156U,
    .current_size = 116U,
    .arrival = 1742197326U,
};
static uint8_t _out_dst[] = TEST_OUT_DST;

static void set_up(void)
{
    gnrc_sixlowpan_frag_vrb_reset();
    gnrc_sixlowpan_frag_fb_reset();
}

static void test_vrb_add__success(void)
{
    gnrc_sixlowpan_frag_vrb_t *res;

    TEST_ASSERT_NOT_NULL((res = gnrc_sixlowpan_frag_vrb_add(&_base,
                                                            &_dummy_netif,
                                                            _out_dst,
                                                            sizeof(_out_dst))));
    TEST_ASSERT_NOT_NULL(res->super.ints);
    TEST_ASSERT_NULL(res->super.ints->next);
    /* make sure _base and res->super are distinct*/
    TEST_ASSERT((&_base) != (&res->super));
    /* but that the values are the same */
    TEST_ASSERT_EQUAL_INT(_interval.start, res->super.ints->start);
    TEST_ASSERT_EQUAL_INT(_interval.end, res->super.ints->end);
    TEST_ASSERT_EQUAL_INT(_base.src_len, res->super.src_len);
    TEST_ASSERT_MESSAGE(memcmp(_base.src, res->super.src, TEST_SRC_LEN) == 0,
                        "TEST_SRC != res->super.src");
    TEST_ASSERT_EQUAL_INT(_base.tag, res->super.tag);
    TEST_ASSERT_EQUAL_INT(_base.datagram_size, res->super.datagram_size);
    TEST_ASSERT_EQUAL_INT(_base.current_size, res->super.current_size);
    TEST_ASSERT_EQUAL_INT(_base.arrival, res->super.arrival);
    TEST_ASSERT((&_dummy_netif) == res->out_netif);
    TEST_ASSERT_EQUAL_INT(sizeof(_out_dst), res->super.dst_len);
    TEST_ASSERT_MESSAGE(memcmp(_out_dst, res->super.dst, sizeof(_out_dst)) == 0,
                        "TEST_DST != res->super.dst");
}

static void test_vrb_add__duplicate(void)
{
    gnrc_sixlowpan_frag_vrb_t *res1, *res2;

    TEST_ASSERT_NOT_NULL((res1 = gnrc_sixlowpan_frag_vrb_add(&_base,
                                                             &_dummy_netif,
                                                             _out_dst,
                                                             sizeof(_out_dst))));
    TEST_ASSERT_NOT_NULL((res2 = gnrc_sixlowpan_frag_vrb_add(&_base,
                                                             &_dummy_netif,
                                                             _out_dst,
                                                             sizeof(_out_dst))));
    TEST_ASSERT(res1 == res2);
}

static void test_vrb_add__full(void)
{
    gnrc_sixlowpan_frag_rb_base_t base = _base;

    /* fill up VRB */
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        TEST_ASSERT_NOT_NULL(gnrc_sixlowpan_frag_vrb_add(&base,
                                                         &_dummy_netif,
                                                         _out_dst,
                                                         sizeof(_out_dst)));
        base.tag++;
    }
    /* another entry will not fit */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_add(&base, &_dummy_netif,
                                                 _out_dst, sizeof(_out_dst)));
    /* check if it really isn't in the VRB */
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(base.src, base.src_len,
                                                 base.tag));
}

static void test_vrb_get__empty(void)
{
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(_base.src, _base.src_len,
                                                 _base.tag));
}

static void test_vrb_get__after_add(void)
{
    gnrc_sixlowpan_frag_vrb_t *res1, *res2;

    TEST_ASSERT_NOT_NULL((res1 = gnrc_sixlowpan_frag_vrb_add(&_base,
                                                             &_dummy_netif,
                                                             _out_dst,
                                                             sizeof(_out_dst))));
    TEST_ASSERT_NOT_NULL((res2 = gnrc_sixlowpan_frag_vrb_get(_base.src,
                                                             _base.src_len,
                                                             _base.tag)));
    TEST_ASSERT(res1 == res2);
}

static void test_vrb_rm(void)
{
    gnrc_sixlowpan_frag_vrb_t *res;

    TEST_ASSERT_NOT_NULL((res = gnrc_sixlowpan_frag_vrb_add(&_base,
                                                            &_dummy_netif,
                                                            _out_dst,
                                                            sizeof(_out_dst))));
    gnrc_sixlowpan_frag_vrb_rm(res);
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(_base.src, _base.src_len,
                                                 _base.tag));
}

static void test_vrb_gc(void)
{
    gnrc_sixlowpan_frag_rb_base_t base = _base;
    gnrc_sixlowpan_frag_vrb_t *res;

    base.arrival = xtimer_now_usec() - CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_TIMEOUT_US - 1000;
    TEST_ASSERT_NOT_NULL((res = gnrc_sixlowpan_frag_vrb_add(&base,
                                                            &_dummy_netif,
                                                            _out_dst,
                                                            sizeof(_out_dst))));
    gnrc_sixlowpan_frag_vrb_gc();
    TEST_ASSERT_NULL(gnrc_sixlowpan_frag_vrb_get(base.src, base.src_len,
                                                 base.tag));
}

static Test *tests_gnrc_sixlowpan_frag_vrb_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_vrb_add__success),
        new_TestFixture(test_vrb_add__duplicate),
        new_TestFixture(test_vrb_add__full),
        new_TestFixture(test_vrb_get__empty),
        new_TestFixture(test_vrb_get__after_add),
        new_TestFixture(test_vrb_rm),
        new_TestFixture(test_vrb_gc),
    };

    EMB_UNIT_TESTCALLER(vrb_tests, set_up, NULL, fixtures);

    return (Test *)&vrb_tests;
}

void tests_gnrc_sixlowpan_frag_vrb(void)
{
    xtimer_init();
    TESTS_RUN(tests_gnrc_sixlowpan_frag_vrb_tests());
}
/** @} */
