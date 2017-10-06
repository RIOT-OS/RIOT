/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <inttypes.h>

#include "net/ipv6/addr.h"
#include "net/ndp.h"
#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/netif.h"

#include "_nib-internal.h"

#include "unittests-constants.h"

#include "tests-gnrc_ipv6_nib.h"

#define LINK_LOCAL_PREFIX   { 0xfe, 0x08, 0, 0, 0, 0, 0, 0 }
#define GLOBAL_PREFIX       { 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0 }
#define IFACE               (26)

static void set_up(void)
{
    _nib_init();
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF persistent entries with different IP addresses
 * and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_alloc__no_space_left_diff_addr(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node;

        TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, IFACE)));
        node->mode |= _DRL;
        addr.u64[1].u64++;
    }
    TEST_ASSERT_NULL(_nib_onl_alloc(&addr, IFACE));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF persistent entries with different interface
 * identifiers and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_alloc__no_space_left_diff_iface(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node;

        TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, iface)));
        node->mode |= _DAD;
        iface++;
    }
    TEST_ASSERT_NULL(_nib_onl_alloc(&addr, iface));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF persistent entries with different IP addresses
 * and interface identifiers and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_alloc__no_space_left_diff_addr_iface(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node;

        TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, iface)));
        node->mode |= _DC;
        addr.u64[1].u64++;
        iface++;
    }
    TEST_ASSERT_NULL(_nib_onl_alloc(&addr, iface));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF persistent entries with different IP addresses
 * and interface identifiers and then tries to add another that is equal to the
 * last.
 * Expected result: should return not NULL (the last)
 */
static void test_nib_alloc__success_duplicate(void)
{
    _nib_onl_entry_t *node;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        addr.u64[1].u64++;
        iface++;
        TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, iface)));
        node->mode |= _PL;
    }
    TEST_ASSERT(node == _nib_onl_alloc(&addr, iface));
}

/*
 * Creates an non-persistent entry.
 * Expected result: new entry should contain the given address and interface
 */
static void test_nib_alloc__success(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, IFACE)));
    TEST_ASSERT(ipv6_addr_equal(&addr, &node->ipv6));
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(node));
}

/*
 * Creates an persistent entry and tries to clear it.
 * Expected result: _nib_onl_clear returns false and entry should still be first
 */
static void test_nib_clear__persistent(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, IFACE)));
    node->mode |= _DRL;
    TEST_ASSERT(!_nib_onl_clear(node));
    TEST_ASSERT(node == _nib_onl_iter(NULL));
}

/*
 * Creates a non-persistent entry with all other flags set.
 * Expected result: _nib_onl_clear returns false and entry should still be first
 */
static void test_nib_clear__non_persistent_but_content(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, IFACE)));
    node->mode |= ~(_FT);
    TEST_ASSERT(!_nib_onl_clear(node));
    TEST_ASSERT(node == _nib_onl_iter(NULL));
}

/*
 * Creates a non-persistent entry.
 * Expected result: _nib_onl_clear returns true and entry is cleared
 */
static void test_nib_clear__empty(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, IFACE)));
    TEST_ASSERT(ipv6_addr_equal(&addr, &node->ipv6));
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(node));
    TEST_ASSERT(_nib_onl_clear(node));
    TEST_ASSERT_NULL(_nib_onl_iter(NULL));
    TEST_ASSERT(ipv6_addr_is_unspecified(&node->ipv6));
    TEST_ASSERT_EQUAL_INT(0, _nib_onl_get_if(node));

}

/*
 * Iterates over empty NIB
 * Expected result: _nib_onl_iter returns NULL
 */
static void test_nib_iter__empty(void)
{
    TEST_ASSERT_NULL(_nib_onl_iter(NULL));
}

/*
 * Iterates over NIB with one element
 * Expected result: _nib_onl_iter returns element with NULL, and with that element
 * NULL.
 */
static void test_nib_iter__one_elem(void)
{
    _nib_onl_entry_t *node, *res;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, IFACE)));
    node->mode = _FT;
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(NULL)));
    TEST_ASSERT(res == node);
    TEST_ASSERT_NULL(_nib_onl_iter(res));
}

/*
 * Iterates over NIB with two element
 * Expected result: _nib_onl_iter returns element with NULL, with that element
 * another, and with the last NULL.
 */
static void test_nib_iter__two_elem(void)
{
    _nib_onl_entry_t *node1, *node2, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_onl_alloc(&addr, IFACE)));
    node1->mode = _DC;
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node2 = _nib_onl_alloc(&addr, IFACE)));
    node2->mode = _PL;
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(NULL)));
    TEST_ASSERT(res == node1);
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(res)));
    TEST_ASSERT(res == node2);
    TEST_ASSERT_NULL(_nib_onl_iter(res));
}

/*
 * Iterates over NIB with three element
 * Expected result: _nib_onl_iter returns element with NULL, with that element
 * another, with that element yet another and with the last NULL.
 */
static void test_nib_iter__three_elem(void)
{
    _nib_onl_entry_t *node1, *node2, *node3, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_onl_alloc(&addr, IFACE)));
    node1->mode = _DAD;
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node2 = _nib_onl_alloc(&addr, IFACE)));
    node2->mode = _DRL;
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node3 = _nib_onl_alloc(&addr, IFACE)));
    node3->mode = _FT;
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(NULL)));
    TEST_ASSERT(res == node1);
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(res)));
    TEST_ASSERT(res == node2);
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(res)));
    TEST_ASSERT(res == node3);
    TEST_ASSERT_NULL(_nib_onl_iter(res));
}

/*
 * Iterates over NIB with three element
 * Expected result: _nib_onl_iter returns element with NULL, with that element
 * another, with that element yet another and with the last NULL.
 */
static void test_nib_iter__three_elem_middle_removed(void)
{
    _nib_onl_entry_t *node1, *node2, *node3, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_onl_alloc(&addr, IFACE)));
    node1->mode = _PL;
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node2 = _nib_onl_alloc(&addr, IFACE)));
    node2->mode = _FT;
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node3 = _nib_onl_alloc(&addr, IFACE)));
    node3->mode = _DRL;
    node2->mode = _EMPTY;
    TEST_ASSERT(_nib_onl_clear(node2));
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(NULL)));
    TEST_ASSERT(res == node1);
    TEST_ASSERT_NOT_NULL((res = _nib_onl_iter(res)));
    TEST_ASSERT(res == node3);
    TEST_ASSERT_NULL(_nib_onl_iter(res));
}

/*
 * Tries to get a NIB entry from an empty NIB.
 * Expected result: _nib_onl_get() returns NULL
 */
static void test_nib_get__not_in_nib(void)
{
    _nib_onl_entry_t *nib_alloced;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((nib_alloced = _nib_onl_alloc(&addr, IFACE)));
    nib_alloced->mode = _FT;
    addr.u64[1].u64++;
    TEST_ASSERT_NULL(_nib_onl_get(&addr, IFACE));
}

/*
 * Tries to get a NIB entry from an empty NIB.
 * Expected result: _nib_onl_get() returns NULL
 */
static void test_nib_get__success(void)
{
    _nib_onl_entry_t *nib_alloced, *nib_got;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((nib_alloced = _nib_onl_alloc(&addr, IFACE)));
    nib_alloced->mode = _NC;
    TEST_ASSERT_NOT_NULL((nib_got = _nib_onl_get(&addr, IFACE)));
    TEST_ASSERT(nib_alloced == nib_got);
}

/*
 * Tries to get a NIB entry that is not in the NIB.
 * Expected result: _nib_onl_get() returns NULL
 */
static void test_nib_get__empty(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NULL(_nib_onl_get(&addr, IFACE));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different IP
 * addresses and a non-garbage-collectible AR state and then tries to add
 * another.
 * Expected result: should return NULL
 */
static void test_nib_nc_add__no_space_left_diff_addr(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node;

        TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, IFACE,
                                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
        node->info |= GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED;
        addr.u64[1].u64++;
    }
    TEST_ASSERT_NULL(_nib_nc_add(&addr, IFACE,
                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different interface
 * identifiers and a non-garbage-collectible AR state and then tries to add
 * another.
 * Expected result: should return NULL
 */
static void test_nib_nc_add__no_space_left_diff_iface(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node;

        TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, iface,
                                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
        node->info |= GNRC_IPV6_NIB_NC_INFO_AR_STATE_TENTATIVE;
        iface++;
    }
    TEST_ASSERT_NULL(_nib_nc_add(&addr, iface,
                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different IP
 * addresses and interface identifiers and a non-garbage-collectible AR state
 * and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_nc_add__no_space_left_diff_addr_iface(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node;

        TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, iface,
                                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED)));
        node->info |= GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED;
        addr.u64[1].u64++;
        iface++;
    }
    TEST_ASSERT_NULL(_nib_nc_add(&addr, iface,
                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different IP
 * addresses and interface identifiers and a non-garbage-collectible AR state
 * and then tries to add another that is equal to the last.
 * Expected result: should return not NULL (the last)
 */
static void test_nib_nc_add__success_duplicate(void)
{
    _nib_onl_entry_t *node;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 0;

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        addr.u64[1].u64++;
        iface++;
        TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, iface,
                                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE)));
        node->info |= GNRC_IPV6_NIB_NC_INFO_AR_STATE_REGISTERED;
    }
    TEST_ASSERT(node == _nib_nc_add(&addr, iface,
                                    GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE));
}

/*
 * Creates an neighbor cache entry.
 * Expected result: new entry should contain the given address and interface
 */
static void test_nib_nc_add__success(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, IFACE,
                                             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
    TEST_ASSERT(node->mode & _NC);
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE,
                          (node->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK));
    TEST_ASSERT(ipv6_addr_equal(&addr, &node->ipv6));
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(node));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with differnt IP address.
 * Expected result: new entries should still be able to be created and further
 * should be different than the previous created ones
 */
static void test_nib_nc_add__success_full_but_garbage_collectible(void)
{
    _nib_onl_entry_t *last = NULL, *node;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < (3 * GNRC_IPV6_NIB_NUMOF); i++) {
        TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, IFACE,
                                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE)));
        TEST_ASSERT(last != node);
        TEST_ASSERT(ipv6_addr_equal(&addr, &node->ipv6));
        TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(node));
        addr.u64[1].u64++;
        last = node;
    }
}

/*
 * Creates a neighbor cache entry and sets it reachable
 * Expected result: node->info flags set to NUD_STATE_REACHABLE and NIB's event
 * timer contains a GNRC_IPV6_NIB_MSG_NUD_SET_STALE event
 */
static void test_nib_nc_set_reachable__success(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, IFACE,
                                             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
    _nib_nc_set_reachable(node);
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE,
                          (node->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK));
    /* check existence of event in event timer first */
    /* TODO: check NIB's event timer */
}

/*
 * Creates a neighbor cache entry, sets another flag, and tries to remove it.
 * Expected result: The entry should still exist
 */
static void test_nib_nc_remove__uncleared(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, IFACE,
                                             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE)));
    node->mode |= _DC;
    _nib_nc_remove(node);
    TEST_ASSERT(node == _nib_onl_iter(NULL));
}

/*
 * Creates a neighbor cache entry and tries to remove it.
 * Expected result: The NIB should be empty
 */
static void test_nib_nc_remove__cleared(void)
{
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_nc_add(&addr, IFACE,
                                             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
    _nib_nc_remove(node);
    TEST_ASSERT_NULL(_nib_onl_iter(NULL));
}

/*
 * Creates GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF default router list entries with
 * different IP addresses and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_drl_add__no_space_left_diff_addr(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        TEST_ASSERT_NOT_NULL(_nib_drl_add(&addr, IFACE));
        addr.u64[1].u64++;
    }
    TEST_ASSERT_NULL(_nib_drl_add(&addr, IFACE));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF persistent enties entries with
 * different IP addresses and then tries to add a default router list entry with
 * yet another address.
 * Expected result: should return NULL
 */
static void test_nib_drl_add__no_space_left_nib_full(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        _nib_onl_entry_t *node;

        TEST_ASSERT_NOT_NULL((node = _nib_onl_alloc(&addr, IFACE)));
        node->mode |= _PL;
        addr.u64[1].u64++;
    }
    TEST_ASSERT_NULL(_nib_drl_add(&addr, IFACE));
}

/*
 * Creates GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF default router list entries with
 * different interface identifiers and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_drl_add__no_space_left_diff_iface(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        TEST_ASSERT_NOT_NULL(_nib_drl_add(&addr, iface));
        iface++;
    }
    TEST_ASSERT_NULL(_nib_drl_add(&addr, iface));
}

/*
 * Creates GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF default router list entries with
 * different IP addresses and interface identifiers and then tries to add
 * another.
 * Expected result: should return NULL
 */
static void test_nib_drl_add__no_space_left_diff_addr_iface(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        TEST_ASSERT_NOT_NULL(_nib_drl_add(&addr, iface));
        addr.u64[1].u64++;
        iface++;
    }
    TEST_ASSERT_NULL(_nib_drl_add(&addr, iface));
}

/*
 * Creates GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF default router list entries with
 * different IP addresses and interface identifiers and then tries to add
 * another that is equal to the last.
 * Expected result: should return not NULL (the last)
 */
static void test_nib_drl_add__success_duplicate(void)
{
    _nib_dr_entry_t *nib_dr;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    unsigned iface = 1;

    for (int i = 0; i < GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF; i++) {
        addr.u64[1].u64++;
        iface++;
        TEST_ASSERT_NOT_NULL((nib_dr = _nib_drl_add(&addr, iface)));
    }
    TEST_ASSERT(nib_dr == _nib_drl_add(&addr, iface));
}

/*
 * Creates an default router list entry.
 * Expected result: new entry should contain the given address and interface
 */
static void test_nib_drl_add__success(void)
{
    _nib_dr_entry_t *nib_dr;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((nib_dr = _nib_drl_add(&addr, IFACE)));
    TEST_ASSERT_NOT_NULL(nib_dr->next_hop);
    TEST_ASSERT(nib_dr->next_hop == _nib_onl_iter(NULL));
    TEST_ASSERT(nib_dr->next_hop->mode & (_DRL));
    TEST_ASSERT(ipv6_addr_equal(&addr, &nib_dr->next_hop->ipv6));
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(nib_dr->next_hop));
}

/*
 * Creates a default router list entry, sets another flag, and tries to remove
 * it.
 * Expected result: The entry default router list entry is removed, but the
 * NIB entry should still exist
 */
static void test_nib_drl_remove__uncleared(void)
{
    _nib_dr_entry_t *nib_dr;
    _nib_onl_entry_t *node;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((nib_dr = _nib_drl_add(&addr, IFACE)));
    node = nib_dr->next_hop;
    node->mode |= _NC;
    _nib_drl_remove(nib_dr);
    TEST_ASSERT_NULL(_nib_drl_iter(NULL));
    TEST_ASSERT(node == _nib_onl_iter(NULL));
}

/*
 * Creates a default router list entry and tries to remove it.
 * Expected result: The NIB should be empty
 */
static void test_nib_drl_remove__cleared(void)
{
    _nib_dr_entry_t *nib_dr;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((nib_dr = _nib_drl_add(&addr, IFACE)));
    _nib_drl_remove(nib_dr);
    TEST_ASSERT_NULL(_nib_drl_iter(NULL));
    TEST_ASSERT_NULL(_nib_onl_iter(NULL));
}

/*
 * Iterates over empty default router list
 * Expected result: _nib_drl_iter returns NULL
 */
static void test_nib_drl_iter__empty(void)
{
    TEST_ASSERT_NULL(_nib_drl_iter(NULL));
}

/*
 * Iterates over default router list with one element
 * Expected result: _nib_drl_iter returns element with NULL, and with that
 * element NULL.
 */
static void test_nib_drl_iter__one_elem(void)
{
    _nib_dr_entry_t *node, *res;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                               { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_drl_add(&addr, IFACE)));
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(NULL)));
    TEST_ASSERT(res == node);
    TEST_ASSERT_NULL(_nib_drl_iter(res));
}

/*
 * Iterates over default router list with two element
 * Expected result: _nib_drl_iter returns element with NULL, with that element
 * another, and with the last NULL.
 */
static void test_nib_drl_iter__two_elem(void)
{
    _nib_dr_entry_t *node1, *node2, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(NULL)));
    TEST_ASSERT(res == node1);
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(res)));
    TEST_ASSERT(res == node2);
    TEST_ASSERT_NULL(_nib_drl_iter(res));
}

/*
 * Iterates over default router list with three element
 * Expected result: _nib_drl_iter returns element with NULL, with that element
 * another, with that element yet another and with the last NULL.
 */
static void test_nib_drl_iter__three_elem(void)
{
    _nib_dr_entry_t *node1, *node2, *node3, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node3 = _nib_drl_add(&addr, IFACE)));
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(NULL)));
    TEST_ASSERT(res == node1);
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(res)));
    TEST_ASSERT(res == node2);
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(res)));
    TEST_ASSERT(res == node3);
    TEST_ASSERT_NULL(_nib_drl_iter(res));
}

/*
 * Iterates over default router list with three element
 * Expected result: _nib_drl_iter returns element with NULL, with that element
 * another, with that element yet another and with the last NULL.
 */
static void test_nib_drl_iter__three_elem_middle_removed(void)
{
    _nib_dr_entry_t *node1, *node2, *node3, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node3 = _nib_drl_add(&addr, IFACE)));
    node2->next_hop->mode = _EMPTY;
    _nib_drl_remove(node2);
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(NULL)));
    TEST_ASSERT(res == node1);
    TEST_ASSERT_NOT_NULL((res = _nib_drl_iter(res)));
    TEST_ASSERT(res == node3);
    TEST_ASSERT_NULL(_nib_drl_iter(res));
}

/*
 * Tries to get a default router list entry from an empty NIB.
 * Expected result: _nib_drl_get() returns NULL
 */
static void test_nib_drl_get__not_in_nib(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL(_nib_drl_add(&addr, IFACE));
    addr.u64[1].u64++;
    TEST_ASSERT_NULL(_nib_drl_get(&addr, IFACE));
}

/*
 * Tries to get a default router list entry
 * Expected result: _nib_drl_get() returns
 */
static void test_nib_drl_get__success(void)
{
    _nib_dr_entry_t *nib_alloced, *nib_got;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((nib_alloced = _nib_drl_add(&addr, IFACE)));
    TEST_ASSERT_NOT_NULL((nib_got = _nib_drl_get(&addr, IFACE)));
    TEST_ASSERT(nib_alloced == nib_got);
}

/*
 * Tries to get the default router from an empty default router list
 * Expected result: _nib_drl_get_dr() returns NULL
 */
static void test_nib_drl_get_dr__empty(void)
{
    TEST_ASSERT_NULL(_nib_drl_get_dr());
}

/*
 * Tries to get the default router from a list of one unreachable routers
 * Expected result: _nib_drl_get_dr() returns always the one unreachable router
 */
static void test_nib_drl_get_dr__round_robin1(void)
{
    _nib_dr_entry_t *node, *nib_res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node = _nib_drl_add(&addr, IFACE)));
    node->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE;
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node);
}

/*
 * Tries to get the default router from a list of two unreachable routers
 * Expected result: _nib_drl_get_dr() returns one router first, then the other,
 * then the first again, etc.
 */
static void test_nib_drl_get_dr__round_robin2(void)
{
    _nib_dr_entry_t *node1, *node2, *nib_res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    node1->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    node2->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE;
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node1);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node2);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node1);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node2);
}

/*
 * Tries to get the default router from a list of two routers
 * the first reachable, the second unreachable
 * Expected result: _nib_drl_get_dr() always returns the first router
 */
static void test_nib_drl_get_dr__success1(void)
{
    _nib_dr_entry_t *node1, *node2, *nib_res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    node1->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    node2->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE;
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node1);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node1);
}

/*
 * Tries to get the default router from a list of three routers
 * the first two unreachable, the third reachable
 * Expected result: _nib_drl_get_dr() always returns the third router
 */
static void test_nib_drl_get_dr__success2(void)
{
    _nib_dr_entry_t *node1, *node2, *node3, *nib_res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    node1->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    node2->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE;
    TEST_ASSERT_NOT_NULL((node3 = _nib_drl_add(&addr, IFACE)));
    node3->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_DELAY;
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node3);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node3);
}

/*
 * Tries to get the default router from a list of three routers
 * the first and the third unreachable, the second reachable. Afterwards, the
 * second becomes unreachable
 * Expected result: _nib_drl_get_dr() at first always returns the second router
 * but continues to round robin as soon as the second router becomes
 * unreachable.
 */
static void test_nib_drl_get_dr__success3(void)
{
    _nib_dr_entry_t *node1, *node2, *node3, *nib_res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    node1->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    node2->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
    TEST_ASSERT_NOT_NULL((node3 = _nib_drl_add(&addr, IFACE)));
    node3->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE;
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node2);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node2);
    node2->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE;
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res != node2);
}

/*
 * Tries to get the default router from a list of three routers
 * the first and the third unreachable, the second reachable. Afterwards, the
 * second is deleted
 * Expected result: _nib_drl_get_dr() at first always returns the second router
 * but continues to round robin as soon as the second router is deleted.
 */
static void test_nib_drl_get_dr__success4(void)
{
    _nib_dr_entry_t *node1, *node2, *node3, *nib_res;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_drl_add(&addr, IFACE)));
    addr.u64[1].u64++;
    node1->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE;
    TEST_ASSERT_NOT_NULL((node2 = _nib_drl_add(&addr, IFACE)));
    node2->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE;
    TEST_ASSERT_NOT_NULL((node3 = _nib_drl_add(&addr, IFACE)));
    node3->next_hop->info = GNRC_IPV6_NIB_NC_INFO_NUD_STATE_INCOMPLETE;
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node2);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res == node2);
    _nib_drl_remove(node2);
    TEST_ASSERT_NOT_NULL((nib_res = _nib_drl_get_dr()));
    TEST_ASSERT(nib_res != node2);
}

/*
 * Creates GNRC_NETIF_NUMOF interfaces and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_iface_get__no_space_left(void)
{
    unsigned iface = 1;

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        TEST_ASSERT_NOT_NULL(_nib_iface_get(iface++));
    }
    TEST_ASSERT_NULL(_nib_iface_get(iface));
}

/*
 * Creates an interface and then gets the same interface.
 * Expected result: interface pointers should equal
 */
static void test_nib_iface_get__success(void)
{
    _nib_iface_t *ni1, *ni2;

    TEST_ASSERT_NOT_NULL((ni1 = _nib_iface_get(IFACE)));
    TEST_ASSERT_NOT_NULL((ni2 = _nib_iface_get(IFACE)));
    TEST_ASSERT(ni1 == ni2);
}

Test *tests_gnrc_ipv6_nib_internal_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nib_alloc__no_space_left_diff_addr),
        new_TestFixture(test_nib_alloc__no_space_left_diff_iface),
        new_TestFixture(test_nib_alloc__no_space_left_diff_addr_iface),
        new_TestFixture(test_nib_alloc__success_duplicate),
        new_TestFixture(test_nib_alloc__success),
        new_TestFixture(test_nib_clear__persistent),
        new_TestFixture(test_nib_clear__non_persistent_but_content),
        new_TestFixture(test_nib_clear__empty),
        new_TestFixture(test_nib_iter__empty),
        new_TestFixture(test_nib_iter__one_elem),
        new_TestFixture(test_nib_iter__two_elem),
        new_TestFixture(test_nib_iter__three_elem),
        new_TestFixture(test_nib_iter__three_elem_middle_removed),
        new_TestFixture(test_nib_get__empty),
        new_TestFixture(test_nib_get__not_in_nib),
        new_TestFixture(test_nib_get__success),
        new_TestFixture(test_nib_nc_add__no_space_left_diff_addr),
        new_TestFixture(test_nib_nc_add__no_space_left_diff_iface),
        new_TestFixture(test_nib_nc_add__no_space_left_diff_addr_iface),
        new_TestFixture(test_nib_nc_add__success_duplicate),
        new_TestFixture(test_nib_nc_add__success),
        new_TestFixture(test_nib_nc_add__success_full_but_garbage_collectible),
        new_TestFixture(test_nib_nc_remove__uncleared),
        new_TestFixture(test_nib_nc_remove__cleared),
        new_TestFixture(test_nib_nc_set_reachable__success),
        new_TestFixture(test_nib_drl_add__no_space_left_diff_addr),
        new_TestFixture(test_nib_drl_add__no_space_left_diff_iface),
        new_TestFixture(test_nib_drl_add__no_space_left_diff_addr_iface),
        new_TestFixture(test_nib_drl_add__no_space_left_nib_full),
        new_TestFixture(test_nib_drl_add__success_duplicate),
        new_TestFixture(test_nib_drl_add__success),
        new_TestFixture(test_nib_drl_remove__uncleared),
        new_TestFixture(test_nib_drl_remove__cleared),
        new_TestFixture(test_nib_drl_iter__empty),
        new_TestFixture(test_nib_drl_iter__one_elem),
        new_TestFixture(test_nib_drl_iter__two_elem),
        new_TestFixture(test_nib_drl_iter__three_elem),
        new_TestFixture(test_nib_drl_iter__three_elem_middle_removed),
        new_TestFixture(test_nib_drl_get__not_in_nib),
        new_TestFixture(test_nib_drl_get__success),
        new_TestFixture(test_nib_drl_get_dr__empty),
        new_TestFixture(test_nib_drl_get_dr__round_robin1),
        new_TestFixture(test_nib_drl_get_dr__round_robin2),
        new_TestFixture(test_nib_drl_get_dr__success1),
        new_TestFixture(test_nib_drl_get_dr__success2),
        new_TestFixture(test_nib_drl_get_dr__success3),
        new_TestFixture(test_nib_drl_get_dr__success4),
        new_TestFixture(test_nib_iface_get__no_space_left),
        new_TestFixture(test_nib_iface_get__success),
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL,
                        fixtures);

    return (Test *)&tests;
}
