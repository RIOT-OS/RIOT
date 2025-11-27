/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <inttypes.h>

#include "bitfield.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/ft.h"

#include "_nib-internal.h"

#include "unittests-constants.h"

#include "tests-gnrc_ipv6_nib.h"

#define LINK_LOCAL_PREFIX   { 0xfe, 0x08, 0, 0, 0, 0, 0, 0 }
#define GLOBAL_PREFIX       { 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0 }
#define L2ADDR              { 0x90, 0xd5, 0x8e, 0x8c, 0x92, 0x43, 0x73, 0x5c }
#define GLOBAL_PREFIX_LEN   (30)
#define IFACE               (6)

static void set_up(void)
{
    evtimer_event_t *tmp;

    for (evtimer_event_t *ptr = _nib_evtimer.events;
         (ptr != NULL) && (tmp = (ptr->next), 1);
         ptr = tmp) {
        evtimer_del((evtimer_t *)(&_nib_evtimer), ptr);
    }
    _nib_init();
}

/*
 * Tries to get a route from an empty forwarding table.
 * Expected result: gnrc_ipv6_nib_ft_get() returns -ENETUNREACH
 */
static void test_nib_ft_get__ENETUNREACH_empty(void)
{
    gnrc_ipv6_nib_ft_t fte;
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                              { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(-ENETUNREACH, gnrc_ipv6_nib_ft_get(&dst, NULL, &fte));
}

/*
 * Adds a route (not the default route) to the forwarding table, then tries to
 * get another route.
 * Expected result: gnrc_ipv6_nib_ft_get() returns -ENETUNREACH
 */
static void test_nib_ft_get__ENETUNREACH_no_def_route(void)
{
    gnrc_ipv6_nib_ft_t fte;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    dst.u16[0].u16--;
    TEST_ASSERT_EQUAL_INT(-ENETUNREACH, gnrc_ipv6_nib_ft_get(&dst, NULL, &fte));
}

/*
 * Adds the default route to the forwarding table, then tries to get an
 * arbitrary route.
 * Expected result: gnrc_ipv6_nib_ft_get() returns the configured default route
 */
static void test_nib_ft_get__success1(void)
{
    gnrc_ipv6_nib_ft_t fte;
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                              { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0, &next_hop, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_get(&dst, NULL, &fte));
    TEST_ASSERT(ipv6_addr_is_unspecified(&fte.dst));
    TEST_ASSERT(ipv6_addr_equal(&next_hop, &fte.next_hop));
    TEST_ASSERT_EQUAL_INT(0, fte.dst_len);
    /* we can't make any sure assumption on fte.primary */
    TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
}

/*
 * Adds an arbitrary route to the forwarding table, then tries to get an address
 * with the same prefix.
 * Expected result: gnrc_ipv6_nib_ft_get() returns the configured route
 */
static void test_nib_ft_get__success2(void)
{
    gnrc_ipv6_nib_ft_t fte;
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                              { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_get(&dst, NULL, &fte));
    TEST_ASSERT(ipv6_addr_match_prefix(&dst, &fte.dst) >= GLOBAL_PREFIX_LEN);
    TEST_ASSERT(ipv6_addr_equal(&next_hop, &fte.next_hop));
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, fte.dst_len);
    /* we can't make any sure assumption on fte.primary */
    TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
}

/*
 * Adds two routes to the forwarding table that differ in their last byte
 * (prefixes have same length), then tries to get an address with the same
 * prefix as the first route.
 * Expected result: gnrc_ipv6_nib_ft_get() returns the first configured route
 */
static void test_nib_ft_get__success3(void)
{
    gnrc_ipv6_nib_ft_t fte;
    static const ipv6_addr_t dst1 = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t next_hop1 = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                  { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t next_hop2 = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                  { .u64 = TEST_UINT64 + 1 } } };
    ipv6_addr_t dst2 = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    bf_toggle(dst2.u8, GLOBAL_PREFIX_LEN);
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst1, GLOBAL_PREFIX_LEN,
                                                  &next_hop1, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst2, GLOBAL_PREFIX_LEN,
                                                  &next_hop2, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_get(&dst1, NULL, &fte));
    TEST_ASSERT(ipv6_addr_match_prefix(&dst1, &fte.dst) >= GLOBAL_PREFIX_LEN);
    TEST_ASSERT(ipv6_addr_equal(&next_hop1, &fte.next_hop));
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, fte.dst_len);
    /* we can't make any sure assumption on fte.primary */
    TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
}

/*
 * Adds two routes to the forwarding table that only differ in their prefix
 * length by one bit me length, then tries to get an address with the same
 * prefix as the route with the longer prefix.
 * Expected result: gnrc_ipv6_nib_ft_get() returns route with the longer prefix
 */
static void test_nib_ft_get__success4(void)
{
    gnrc_ipv6_nib_ft_t fte;
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                              { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t next_hop1 = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                  { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t next_hop2 = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                  { .u64 = TEST_UINT64 + 1 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                  &next_hop1, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN - 1,
                                                  &next_hop2, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_get(&dst, NULL, &fte));
    TEST_ASSERT(ipv6_addr_match_prefix(&dst, &fte.dst) >= GLOBAL_PREFIX_LEN);
    TEST_ASSERT(ipv6_addr_equal(&next_hop1, &fte.next_hop));
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, fte.dst_len);
    /* we can't make any sure assumption on fte.primary */
    TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
}

/*
 * Tries to create a forwarding table entry for the default route (::) with
 * NULL as next hop.
 * Expected result: gnrc_ipv6_nib_ft_add() returns -EINVAL
 */
static void test_nib_ft_add__EINVAL_def_route_next_hop_NULL(void)
{
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                              { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_ft_add(&ipv6_addr_unspecified,
                                                        GLOBAL_PREFIX_LEN,
                                                        NULL, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_ft_add(NULL, GLOBAL_PREFIX_LEN,
                                                        NULL, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_ft_add(&dst, 0, NULL, IFACE, 0));
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_ft_add(NULL, 0, NULL, IFACE, 0));
}

/*
 * Tries to create a route via interface 0.
 * Expected result: gnrc_ipv6_nib_ft_add() returns -EINVAL
 */
static void test_nib_ft_add__EINVAL_iface0(void)
{
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                              { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                        &next_hop, 0, 0));
}

#if CONFIG_GNRC_IPV6_NIB_NUMOF < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF
#define MAX_NUMOF   (CONFIG_GNRC_IPV6_NIB_NUMOF)
#else /* CONFIG_GNRC_IPV6_NIB_NUMOF < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF */
#define MAX_NUMOF   (CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF)
#endif

/*
 * Creates CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF default route entries and then
 * tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_def_router(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                    { .u64 = TEST_UINT64 } } };

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0, &next_hop,
                                                      IFACE, 0));
        next_hop.u64[1].u64++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(NULL, GLOBAL_PREFIX_LEN,
                                                        &next_hop, IFACE, 0));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF routes with different destinations of same
 * prefix lengths to the same next hop and then tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                      NULL, IFACE, 0));
        dst.u16[0].u16--;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                        NULL, IFACE, 0));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF routes with destinations of different
 * prefix lengths to the same next hop and then tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_len(void)
{
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN;

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                      NULL, IFACE, 0));
        dst_len--;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                        NULL, IFACE, 0));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF routes with different destination of
 * different prefix lengths to the same next hop and then tries to create
 * another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_dst_len(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN;

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                      NULL, IFACE, 0));
        dst.u16[0].u16--;
        dst_len--;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                        NULL, IFACE, 0));
}

/*
 * Creates MAX_NUMOF routes with the same destination different next hops and
 * then tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_next_hop(void)
{
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                      &next_hop, IFACE, 0));
        next_hop.u64[1].u64++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                        &next_hop, IFACE, 0));
}

/*
 * Creates MAX_NUMOF routes with different destinations of same prefix lengths
 * to the different next hops and then tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_next_hop(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                      &next_hop, IFACE, 0));
        dst.u16[0].u16--;
        next_hop.u64[1].u64++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                        &next_hop, IFACE, 0));
}

/*
 * Creates MAX_NUMOF routes with different destinations of same prefix lengths
 * to different next hops and then tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_dst_len_next_hop(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len, &next_hop,
                                                      IFACE, 0));
        dst.u16[0].u16--;
        dst_len--;
        next_hop.u64[1].u64++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                        &next_hop, IFACE, 0));
}

/*
 * Creates MAX_NUMOF routes with different destinations of same prefix lengths
 * to the same next hop but on different interfaces and then tries to create
 * another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_iface(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                      NULL, iface, 0));
        dst.u16[0].u16--;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                        NULL, iface, 0));
}

/*
 * Creates MAX_NUMOF routes with destinations of different prefix lengths to the
 * same next hop but on different interfaces and then tries to create another
 * one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_len_iface(void)
{
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN;
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                      NULL, iface, 0));
        dst_len--;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                        NULL, iface, 0));
}

/*
 * Creates MAX_NUMOF routes with different destination of different prefix
 * lengths to the same next hop but on different interfaces and then tries to
 * create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_dst_len_iface(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN;
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                      NULL, iface, 0));
        dst.u16[0].u16--;
        dst_len--;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                        NULL, iface, 0));
}

/*
 * Creates MAX_NUMOF routes with the same destination to different next hops and
 * interfaces and then tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_next_hop_iface(void)
{
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                      &next_hop, iface, 0));
        next_hop.u64[1].u64++;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                        &next_hop, iface, 0));
}

/*
 * Creates MAX_NUMOF routes with different destinations of same prefix lengths
 * to different next hops and interfaces and then tries to create another one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_next_hop_iface(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                      &next_hop, iface, 0));
        dst.u16[0].u16--;
        next_hop.u64[1].u64++;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                        &next_hop, iface, 0));
}

/*
 * Creates MAX_NUMOF routes with different destinations of same prefix lengths
 * to the different next hops and interfaces and then tries to create another
 * one
 * Expected result: gnrc_ipv6_nib_ft_add() returns -ENOMEM
 */
static void test_nib_ft_add__ENOMEM_diff_dst_dst_len_next_hop_iface(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN;
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len, &next_hop,
                                                      iface, 0));
        dst.u16[0].u16--;
        dst_len--;
        next_hop.u64[1].u64++;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                        &next_hop, iface, 0));
}

/*
 * Creates MAX_NUMOF routes with different destinations to different next hops
 * and interfaces and then tries add another equal to the last.
 * Expected result: should return not NULL (the last)
 */
static void test_nib_ft_add__success_duplicate(void)
{
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN;
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        dst.u16[0].u16--;
        dst_len--;
        next_hop.u64[1].u64++;
        iface++;
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len, &next_hop,
                                                      iface, 0));
    }
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len,
                                                  &next_hop, iface, 0));
}

/*
 * Creates a route with no next hop address then adds another with equal prefix
 * and interface to the last, but with a next hop address
 * Expected result: there should only be one route (with the configuration of
 * the second)
 */
static void test_nib_ft_add__success_overwrite_unspecified(void)
{
    gnrc_ipv6_nib_ft_t fte;
    void *iter_state = NULL;
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN, NULL,
                                                  IFACE, 0));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    TEST_ASSERT(gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte));
    TEST_ASSERT(ipv6_addr_equal(&fte.next_hop, &next_hop));
    TEST_ASSERT(!gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte));
}

/*
 * Creates a route
 * Expected result: a new entry should exist and contain the given prefix,
 * interface, and lifetimes
 */
static void test_nib_ft_add__success(void)
{
    gnrc_ipv6_nib_ft_t fte;
    void *iter_state = NULL;
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    TEST_ASSERT(gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte));
    TEST_ASSERT(ipv6_addr_match_prefix(&fte.dst, &dst) >= GLOBAL_PREFIX_LEN);
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, fte.dst_len);
    TEST_ASSERT(ipv6_addr_equal(&fte.next_hop, &next_hop));
    TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
    TEST_ASSERT_EQUAL_INT(0, fte.primary);
    TEST_ASSERT(!gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte));
}

/*
 * Creates a default route
 * Expected result: a new entry should exist and contain the given prefix,
 * interface, and lifetimes and it should be the primary default route
 */
static void test_nib_ft_add__success_dr(void)
{
    gnrc_ipv6_nib_ft_t fte;
    void *iter_state = NULL;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0, &next_hop, IFACE, 0));
    TEST_ASSERT(gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte));
    TEST_ASSERT(ipv6_addr_is_unspecified(&fte.dst));
    TEST_ASSERT_EQUAL_INT(0, fte.dst_len);
    TEST_ASSERT(ipv6_addr_equal(&fte.next_hop, &next_hop));
    TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
    TEST_ASSERT_EQUAL_INT(1, fte.primary);
    TEST_ASSERT(!gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte));
}

/*
 * Creates MAX_NUMOF routes with different destinations of to the different
 * next hops and interfaces and then tries to delete one with yet another
 * destination, next hop and interface.
 * Expected result: There should be still MAX_NUMOF entries in the forwarding
 * table
 */
static void test_nib_ft_del__unknown(void)
{
    gnrc_ipv6_nib_ft_t fte;
    void *iter_state = NULL;
    ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    unsigned dst_len = GLOBAL_PREFIX_LEN, iface = IFACE, count = 0;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, dst_len, &next_hop,
                                                      iface, 0));
        dst.u16[0].u16--;
        dst_len--;
        next_hop.u64[1].u64++;
        iface++;
    }
    gnrc_ipv6_nib_ft_del(&dst, dst_len);
    while (gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte)) {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(MAX_NUMOF, count);
}

/*
 * Creates a route and removes it.
 * Expected result: forwarding table should be empty
 */
static void test_nib_ft_del__success(void)
{
    void *iter_state = NULL;
    static const ipv6_addr_t dst = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_ft_t fte;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&dst, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    gnrc_ipv6_nib_ft_del(&dst, GLOBAL_PREFIX_LEN);
    TEST_ASSERT(!gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte));
}

/**
 * Creates three default routes and removes the first one.
 * The prefix list is then iterated.
 * Expected result: there should be two default routes returned, the last
 * two added.
 */
static void test_nib_ft_iter__empty_def_route_at_beginning(void)
{
    gnrc_ipv6_nib_ft_t fte;
    void *iter_state = NULL;
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                    { .u64 = TEST_UINT64 } } };
    int count = 0;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0,
                                                  &next_hop, IFACE, 0));
    next_hop.u16[0].u16++;
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0,
                                                  &next_hop, IFACE, 0));
    next_hop.u16[0].u16++;
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0,
                                                  &next_hop, IFACE, 0));
    /* make first added route the primary default route again */
    next_hop.u16[0].u16 -= 2;
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(NULL, 0,
                                                  &next_hop, IFACE, 0));
    /* remove primary default route */
    gnrc_ipv6_nib_ft_del(NULL, 0);
    next_hop.u16[0].u16++;
    while (gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte)) {
        TEST_ASSERT(ipv6_addr_is_unspecified(&fte.dst));
        TEST_ASSERT(ipv6_addr_equal(&fte.next_hop, &next_hop));
        TEST_ASSERT_EQUAL_INT(0, fte.dst_len);
        TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
        TEST_ASSERT(!fte.primary);
        count++;
        next_hop.u16[0].u16++;
    }
    TEST_ASSERT_EQUAL_INT(2, count);
}

/**
 * Creates three prefix based routes and removes the second one.
 * The prefix list is then iterated.
 * Expected result: there should be two prefix based routes returned, the first
 * and the third one.
 */
static void test_nib_ft_iter__empty_pref_route_in_the_middle(void)
{
    gnrc_ipv6_nib_ft_t fte;
    void *iter_state = NULL;
    ipv6_addr_t route = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                 { .u64 = TEST_UINT64 } } };
    const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                            { .u64 = TEST_UINT64 } } };
    int count = 0;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&route, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    route.u16[0].u16++;
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&route, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    route.u16[0].u16++;
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_ft_add(&route, GLOBAL_PREFIX_LEN,
                                                  &next_hop, IFACE, 0));
    route.u16[0].u16--;
    gnrc_ipv6_nib_ft_del(&route, GLOBAL_PREFIX_LEN);
    route.u16[0].u16--;
    while (gnrc_ipv6_nib_ft_iter(NULL, 0, &iter_state, &fte)) {
        TEST_ASSERT(ipv6_addr_match_prefix(&fte.dst, &route) >= GLOBAL_PREFIX_LEN);
        TEST_ASSERT(ipv6_addr_equal(&fte.next_hop, &next_hop));
        TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, fte.dst_len);
        TEST_ASSERT_EQUAL_INT(IFACE, fte.iface);
        count++;
        route.u16[0].u16 += 2;  /* we skip the second address */
    }
    TEST_ASSERT_EQUAL_INT(2, count);
}

Test *tests_gnrc_ipv6_nib_ft_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nib_ft_get__ENETUNREACH_empty),
        new_TestFixture(test_nib_ft_get__ENETUNREACH_no_def_route),
        new_TestFixture(test_nib_ft_get__success1),
        new_TestFixture(test_nib_ft_get__success2),
        new_TestFixture(test_nib_ft_get__success3),
        new_TestFixture(test_nib_ft_get__success4),
        new_TestFixture(test_nib_ft_add__EINVAL_def_route_next_hop_NULL),
        new_TestFixture(test_nib_ft_add__EINVAL_iface0),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_def_router),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_len),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_dst_len),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_next_hop),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_next_hop),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_dst_len_next_hop),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_iface),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_len_iface),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_dst_len_iface),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_next_hop_iface),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_next_hop_iface),
        new_TestFixture(test_nib_ft_add__ENOMEM_diff_dst_dst_len_next_hop_iface),
        new_TestFixture(test_nib_ft_add__success_duplicate),
        new_TestFixture(test_nib_ft_add__success_overwrite_unspecified),
        new_TestFixture(test_nib_ft_add__success),
        new_TestFixture(test_nib_ft_add__success_dr),
        new_TestFixture(test_nib_ft_del__unknown),
        new_TestFixture(test_nib_ft_del__success),
        /* most of gnrc_ipv6_nib_ft_iter() is tested during all the tests above */
        new_TestFixture(test_nib_ft_iter__empty_def_route_at_beginning),
        new_TestFixture(test_nib_ft_iter__empty_pref_route_in_the_middle),
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL,
                        fixtures);

    return (Test *)&tests;
}
