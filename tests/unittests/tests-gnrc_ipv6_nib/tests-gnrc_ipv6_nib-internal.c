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

#include "_nib-internal.h"
#include "_nib-arsm.h"

#include "unittests-constants.h"

#include "tests-gnrc_ipv6_nib.h"

#define LINK_LOCAL_PREFIX   { 0xfe, 0x08, 0, 0, 0, 0, 0, 0 }
#define GLOBAL_PREFIX       { 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0 }
#define GLOBAL_PREFIX_LEN   (30)
#define IFACE               (6)

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
 * Creates a persistent on-link entry with no IPv6 address and then tries to
 * create another one with the same interface, but with an address
 * Expected result: entries should be identical
 */
static void test_nib_alloc__success_noaddr_override(void)
{
    _nib_onl_entry_t *node1, *node2;
    const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((node1 = _nib_onl_alloc(NULL, IFACE)));
    TEST_ASSERT_NOT_NULL((node2 = _nib_onl_alloc(&addr, IFACE)));
    TEST_ASSERT(node1 == node2);
    TEST_ASSERT(ipv6_addr_equal(&addr, &node1->ipv6));
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
    /* cppcheck-suppress redundantAssignment
     * (reason: we assigned _FT before so _nib_onl_alloc would recognize node2
     *          as used, now we want to clear it, so we need to set it to
     *          _EMPTY... we are testing internals of data structures here) */
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
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different IP address.
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
                                                 GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
        TEST_ASSERT(last != node);
        TEST_ASSERT(ipv6_addr_equal(&addr, &node->ipv6));
        TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(node));
        addr.u64[1].u64++;
        last = node;
    }
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different IP
 * addresses and a garbage-collectible AR state and then tries to add
 * 3 more after removing two.
 * Expected result: should not crash
 *
 * See https://github.com/RIOT-OS/RIOT/pull/10975
 */
static void test_nib_nc_add__cache_out_crash(void)
{
    _nib_onl_entry_t *node1, *node2;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < GNRC_IPV6_NIB_NUMOF - 2; i++) {
        TEST_ASSERT_NOT_NULL(_nib_nc_add(&addr, IFACE,
                                         GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE));
        addr.u64[1].u64++;
    }
    TEST_ASSERT_NOT_NULL((node1 = _nib_nc_add(&addr, IFACE,
                                             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((node2 = _nib_nc_add(&addr, IFACE,
                                             GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE)));
    addr.u64[1].u64++;
    _nib_nc_remove(node1);
    _nib_nc_remove(node2);
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_NOT_NULL(_nib_nc_add(&addr, IFACE,
                                         GNRC_IPV6_NIB_NC_INFO_NUD_STATE_STALE));
        addr.u64[1].u64++;
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

#if GNRC_IPV6_NIB_NUMOF < GNRC_IPV6_NIB_OFFL_NUMOF
#define MAX_NUMOF   (GNRC_IPV6_NIB_NUMOF)
#else /* GNRC_IPV6_NIB_NUMOF < GNRC_IPV6_NIB_OFFL_NUMOF */
#define MAX_NUMOF   (GNRC_IPV6_NIB_OFFL_NUMOF)
#endif

/*
 * Creates MAX_NUMOF off-link entries with different next-hop addresses and
 * then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                    { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                    GLOBAL_PREFIX_LEN)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        next_hop.u64[1].u64++;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, IFACE, &pfx, GLOBAL_PREFIX_LEN));
}

/*
 * Creates MAX_NUMOF off-link entries with different interfaces and then tries
 * to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_iface(void)
{
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;
        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    GLOBAL_PREFIX_LEN)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _PL;
        iface++;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, GLOBAL_PREFIX_LEN));
}

/*
 * Creates MAX_NUMOF off-link entries with different next-hop addresses and
 * interfaces and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop_iface(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    GLOBAL_PREFIX_LEN)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _DC;
        next_hop.u64[1].u64++;
        iface++;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, GLOBAL_PREFIX_LEN));
}

/*
 * Creates GNRC_IPV6_NIB_OFFL_NUMOF off-link entries with different prefixes
 * of the same length and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_pfx(void)
{
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    for (int i = 0; i < GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                    GLOBAL_PREFIX_LEN)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        pfx.u16[0].u16++;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, IFACE, &pfx, GLOBAL_PREFIX_LEN));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes of the same
 * length and different next-hop addresses and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop_pfx(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                    { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                    GLOBAL_PREFIX_LEN)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        next_hop.u64[1].u64++;
        pfx.u16[0].u16++;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, IFACE, &pfx, GLOBAL_PREFIX_LEN));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes of the same
 * length and different interfaces and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_iface_pfx(void)
{
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    GLOBAL_PREFIX_LEN)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        iface++;
        pfx.u16[0].u16++;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, GLOBAL_PREFIX_LEN));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes of the same
 * length, different interfaces, and different next hop addresses and then
 * tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop_iface_pfx(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    GLOBAL_PREFIX_LEN)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        next_hop.u64[1].u64++;
        iface++;
        pfx.u16[0].u16++;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, GLOBAL_PREFIX_LEN));
}

/*
 * Creates GNRC_IPV6_NIB_OFFL_NUMOF off-link entries with different prefix
 * lengths and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_pfx_len(void)
{
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN;

    for (int i = 0; i < GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        _nib_offl_entry_t *dst;
        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _PL;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, IFACE, &pfx, pfx_len));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes and then tries to
 * add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop_pfx_len(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;
        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _PL;
        next_hop.u64[1].u64++;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, IFACE, &pfx, pfx_len));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefix lengths and
 * interfaces and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_iface_pfx_len(void)
{
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;
        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _PL;
        iface++;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, pfx_len));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefix lengths,
 * interfaces, and next hop addresses and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop_iface_pfx_len(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;
        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _PL;
        next_hop.u64[1].u64++;
        iface++;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, pfx_len));
}

/*
 * Creates GNRC_IPV6_NIB_OFFL_NUMOF off-link entries with different prefixes
 * and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_pfx_pfx_len(void)
{
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN;

    for (int i = 0; i < GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        pfx.u16[0].u16++;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, IFACE, &pfx, pfx_len));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes and different
 * next-hop addresses and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop_pfx_pfx_len(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                    { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        next_hop.u64[1].u64++;
        pfx.u16[0].u16++;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, IFACE, &pfx, pfx_len));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes and different
 * interfaces and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_iface_pfx_pfx_len(void)
{
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        iface++;
        pfx.u16[0].u16++;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, pfx_len));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes, different
 * interfaces, and different next hop addresses and then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_offl_alloc__no_space_left_diff_next_hop_iface_pfx_pfx_len(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
        next_hop.u64[1].u64++;
        iface++;
        pfx.u16[0].u16++;
        pfx_len--;
    }
    TEST_ASSERT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, pfx_len));
}

/*
 * Creates MAX_NUMOF off-link entries with different prefixes, different
 * interfaces, and different next hop addresses and then tries to add another
 * equal to the last.
 * Expected result: should return not NULL (the last)
 */
static void test_nib_offl_alloc__success_duplicate(void)
{
    ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                      { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (int i = 0; i < MAX_NUMOF; i++) {
        _nib_offl_entry_t *dst;

        next_hop.u64[1].u64++;
        iface++;
        pfx.u16[0].u16++;
        pfx_len--;
        TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, iface, &pfx,
                                                    pfx_len)));
        TEST_ASSERT_NOT_NULL(dst->next_hop);
        dst->mode |= _FT;
    }
    TEST_ASSERT_NOT_NULL(_nib_offl_alloc(&next_hop, iface, &pfx, pfx_len));
}

/*
 * Creates an off-link entry with no next hop address and then adds another
 * with equal prefix and interface to the last, but with a next hop address
 * Expected result: the first entry should be equal to the second and both
 * have the same next hop address
 */
static void test_nib_offl_alloc__success_overwrite_unspecified(void)
{
    _nib_offl_entry_t *dst1, *dst2;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst1 = _nib_offl_alloc(NULL, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst1->mode |= _PL;
    TEST_ASSERT_NOT_NULL((dst2 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    TEST_ASSERT_NOT_NULL(dst1->next_hop);
    TEST_ASSERT_EQUAL_INT(_PL, dst1->mode);
    TEST_ASSERT(dst1 == dst2);
    TEST_ASSERT(ipv6_addr_equal(&next_hop, &dst1->next_hop->ipv6));
}

/*
 * Creates an off-link entry.
 * Expected result: new entry should contain the given prefix, address and
 *                  interface
 */
static void test_nib_offl_alloc__success(void)
{
    _nib_offl_entry_t *dst;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                GLOBAL_PREFIX_LEN)));
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, dst->pfx_len);
    TEST_ASSERT(GLOBAL_PREFIX_LEN <= ipv6_addr_match_prefix(&pfx, &dst->pfx));
    TEST_ASSERT_NOT_NULL(dst->next_hop);
    TEST_ASSERT_EQUAL_INT(_DST, dst->next_hop->mode);
    TEST_ASSERT(ipv6_addr_equal(&next_hop, &dst->next_hop->ipv6));
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(dst->next_hop));
}

/*
 * Creates an off-link entry, sets a neighbor cache flag, and tries to remove
 * it.
 * Expected result: The off-link entry is removed, but the on-link entry should
 * still exist
 */
static void test_nib_offl_clear__uncleared(void)
{
    _nib_offl_entry_t *dst;
    _nib_onl_entry_t *node;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                GLOBAL_PREFIX_LEN)));
    node = dst->next_hop;
    node->mode |= _NC;
    _nib_offl_clear(dst);
    TEST_ASSERT_NULL(_nib_offl_iter(NULL));
    TEST_ASSERT(node == _nib_onl_iter(NULL));
}

/*
 * Creates two off-link entry off-link entries and tries to remove one of them.
 * Expected result: The NIB should only contain the one removed, the on-link
 * entry should still exist
 */
static void test_nib_offl_clear__same_next_hop(void)
{
    _nib_offl_entry_t *dst1, *dst2, *res;
    _nib_onl_entry_t *node;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst1 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst1->mode |= _FT;
    pfx.u16[0].u16++;
    TEST_ASSERT_NOT_NULL((dst2 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                GLOBAL_PREFIX_LEN)));
    TEST_ASSERT(dst1->next_hop == dst2->next_hop);
    node = dst2->next_hop;
    TEST_ASSERT_NOT_NULL(node);
    _nib_offl_clear(dst2);
    TEST_ASSERT_NOT_NULL((res = _nib_offl_iter(NULL)));
    TEST_ASSERT(dst1 == res);
    TEST_ASSERT_NULL(_nib_offl_iter(res));
    TEST_ASSERT(node == _nib_onl_iter(NULL));
}

/*
 * Creates an off-link entry and tries to remove it.
 * Expected result: The NIB should be empty
 */
static void test_nib_offl_clear__cleared(void)
{
    _nib_offl_entry_t *dst;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                GLOBAL_PREFIX_LEN)));
    _nib_offl_clear(dst);
    TEST_ASSERT_NULL(_nib_offl_iter(NULL));
    TEST_ASSERT_NULL(_nib_onl_iter(NULL));
}

/*
 * Iterates over empty off-link entries
 * Expected result: _nib_drl_iter returns NULL
 */
static void test_nib_offl_iter__empty(void)
{
    TEST_ASSERT_NULL(_nib_offl_iter(NULL));
}

/*
 * Iterates over off-link entries with one element
 * Expected result: _nib_offl_iter returns element with NULL, and with that
 * element NULL.
 */
static void test_nib_offl_iter__one_elem(void)
{
    _nib_offl_entry_t *dst, *res;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                GLOBAL_PREFIX_LEN)));
    dst->mode |= _FT;
    TEST_ASSERT_NOT_NULL((res = _nib_offl_iter(NULL)));
    TEST_ASSERT(res == dst);
    TEST_ASSERT_NULL(_nib_offl_iter(res));
}

/*
 * Iterates over off-link entries with three element
 * Expected result: _nib_offl_iter returns element with NULL, with that element
 * another, with that element yet another and with the last NULL.
 */
static void test_nib_offl_iter__three_elem(void)
{
    _nib_offl_entry_t *dst1, *dst2, *dst3, *res;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst1 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst1->mode |= _FT;
    pfx.u16[0].u16++;
    TEST_ASSERT_NOT_NULL((dst2 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst2->mode |= _FT;
    pfx.u16[0].u16++;
    TEST_ASSERT_NOT_NULL((dst3 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst3->mode |= _FT;
    TEST_ASSERT_NOT_NULL((res = _nib_offl_iter(NULL)));
    TEST_ASSERT(res == dst1);
    TEST_ASSERT_NOT_NULL((res = _nib_offl_iter(res)));
    TEST_ASSERT(res == dst2);
    TEST_ASSERT_NOT_NULL((res = _nib_offl_iter(res)));
    TEST_ASSERT(res == dst3);
    TEST_ASSERT_NULL(_nib_offl_iter(res));
}

/*
 * Iterates over off-link entries with two elements, where there is a whole in
 * the internal array
 * Expected result: _nib_offl_iter returns element with NULL, with that element
 * another, and with the last NULL.
 */
static void test_nib_offl_iter__three_elem_middle_removed(void)
{
    _nib_offl_entry_t *dst1, *dst2, *dst3, *res;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst1 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst1->mode |= _FT;
    pfx.u16[0].u16++;
    TEST_ASSERT_NOT_NULL((dst2 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst2->mode |= _FT;
    pfx.u16[0].u16++;
    TEST_ASSERT_NOT_NULL((dst3 = _nib_offl_alloc(&next_hop, IFACE, &pfx,
                                                 GLOBAL_PREFIX_LEN)));
    dst3->mode |= _FT;
    dst2->mode = _EMPTY;
    _nib_offl_clear(dst2);
    TEST_ASSERT_NOT_NULL((res = _nib_offl_iter(NULL)));
    TEST_ASSERT(res == dst1);
    TEST_ASSERT_NOT_NULL((res = _nib_offl_iter(res)));
    TEST_ASSERT(res == dst3);
    TEST_ASSERT_NULL(_nib_offl_iter(res));
}

#if GNRC_IPV6_NIB_CONF_DC
/*
 * Creates a destination cache entry.
 * Expected result: new entry should contain the given address and interface
 */
static void test_nib_dc_add__success(void)
{
    _nib_offl_entry_t *dst;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t dst_addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_dc_add(&next_hop, IFACE, &dst_addr)));
    TEST_ASSERT(dst->mode & _DC);
    TEST_ASSERT_EQUAL_INT(IPV6_ADDR_BIT_LEN, dst->pfx_len);
    TEST_ASSERT(ipv6_addr_equal(&dst_addr, &dst->pfx));
    TEST_ASSERT_NOT_NULL(dst->next_hop);
    TEST_ASSERT_EQUAL_INT(_DST, dst->next_hop->mode);
    TEST_ASSERT(ipv6_addr_equal(&next_hop, &dst->next_hop->ipv6));
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(dst->next_hop));
}

/*
 * Creates a destination cache entry and removes it.
 * Expected result: The destination cache should be empty
 */
static void test_nib_dc_remove(void)
{

    _nib_offl_entry_t *dst;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t dst_addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_dc_add(&next_hop, IFACE, &dst_addr)));
    _nib_dc_remove(dst);
    TEST_ASSERT_NULL(_nib_offl_iter(NULL));
}
#endif

/*
 * Creates a prefix list entry.
 * Expected result: new entry should contain the given address and interface
 */
static void test_nib_pl_add__success(void)
{
    _nib_offl_entry_t *dst;
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_pl_add(IFACE, &pfx, GLOBAL_PREFIX_LEN,
                                            UINT32_MAX, UINT32_MAX)));
    TEST_ASSERT(dst->mode & _PL);
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, dst->pfx_len);
    TEST_ASSERT(GLOBAL_PREFIX_LEN <= ipv6_addr_match_prefix(&pfx, &dst->pfx));
    TEST_ASSERT_NOT_NULL(dst->next_hop);
    TEST_ASSERT_EQUAL_INT(_DST, dst->next_hop->mode);
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(dst->next_hop));
    TEST_ASSERT_EQUAL_INT(UINT32_MAX, dst->valid_until);
    TEST_ASSERT_EQUAL_INT(UINT32_MAX, dst->pref_until);
}

/*
 * Creates a prefix list entry and removes it.
 * Expected result: The prefix list should be empty
 */
static void test_nib_pl_remove(void)
{

    _nib_offl_entry_t *dst;
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_pl_add(IFACE, &pfx, GLOBAL_PREFIX_LEN,
                                            UINT32_MAX, UINT32_MAX)));
    _nib_pl_remove(dst);
    TEST_ASSERT_NULL(_nib_offl_iter(NULL));
}

/*
 * Creates a forwarding table entry.
 * Expected result: new entry should contain the given address and interface
 */
static void test_nib_ft_add__success(void)
{
    _nib_offl_entry_t *dst;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_ft_add(&next_hop, IFACE, &pfx,
                                            GLOBAL_PREFIX_LEN)));
    TEST_ASSERT(dst->mode & _FT);
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, dst->pfx_len);
    TEST_ASSERT(GLOBAL_PREFIX_LEN <= ipv6_addr_match_prefix(&pfx, &dst->pfx));
    TEST_ASSERT_NOT_NULL(dst->next_hop);
    TEST_ASSERT_EQUAL_INT(_DST, dst->next_hop->mode);
    TEST_ASSERT(ipv6_addr_equal(&next_hop, &dst->next_hop->ipv6));
    TEST_ASSERT_EQUAL_INT(IFACE, _nib_onl_get_if(dst->next_hop));
}

/*
 * Creates a forwarding table entry and removes it.
 * Expected result: The forwarding table should be empty
 */
static void test_nib_ft_remove(void)
{

    _nib_offl_entry_t *dst;
    static const ipv6_addr_t next_hop = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                                 { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };

    TEST_ASSERT_NOT_NULL((dst = _nib_ft_add(&next_hop, IFACE, &pfx,
                                            GLOBAL_PREFIX_LEN)));
    _nib_ft_remove(dst);
    TEST_ASSERT_NULL(_nib_offl_iter(NULL));
}

#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
/*
 * Creates GNRC_IPV6_NIB_ABR_NUMOF ABR entries with different addresses and
 * then tries to add another.
 * Expected result: should return NULL
 */
static void test_nib_abr_add__no_space_left(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < GNRC_IPV6_NIB_ABR_NUMOF; i++) {
        TEST_ASSERT_NOT_NULL(_nib_abr_add(&addr));
        addr.u64[1].u64++;
    }
    TEST_ASSERT_NULL(_nib_abr_add(&addr));
}

/*
 * Creates GNRC_IPV6_NIB_ABR_NUMOF ABR entries with different addresses and then
 * tries to add another that is equal to the last.
 * Expected result: should return not NULL (the last)
 */
static void test_nib_abr_add__success_duplicate(void)
{
    _nib_abr_entry_t *abr;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };

    for (int i = 0; i < GNRC_IPV6_NIB_ABR_NUMOF; i++) {
        addr.u64[1].u64++;
        TEST_ASSERT_NOT_NULL((abr = _nib_abr_add(&addr)));
    }
    TEST_ASSERT(abr == _nib_abr_add(&addr));
}

/*
 * Creates an ABR entry.
 * Expected result: new entry should contain the given address
 */
static void test_nib_abr_add__success(void)
{
    _nib_abr_entry_t *abr;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((abr = _nib_abr_add(&addr)));
    TEST_ASSERT(ipv6_addr_equal(&addr, &abr->addr));
}

/*
 * Creates an ABR entry and then removes the entry.
 * Expected result: the ABR list should be empty
 */
static void test_nib_abr_remove__success(void)
{
    _nib_abr_entry_t *abr = NULL;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL(_nib_abr_add(&addr));
    _nib_abr_remove(&addr);
    TEST_ASSERT_NULL(_nib_abr_iter(abr));
}

/*
 * Creates an ABR entry and tries to add a prefix, that is not in the NIB.
 * Expected result: the ABR's prefix list should be unchanged.
 */
static void test_nib_abr_add_pfx__pfx_not_in_nib(void)
{
    _nib_abr_entry_t *abr;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    _nib_offl_entry_t offl;
    offl.mode = _PL;
    TEST_ASSERT_NOT_NULL((abr = _nib_abr_add(&addr)));
    TEST_ASSERT_NULL(_nib_abr_iter_pfx(abr, NULL));
    _nib_abr_add_pfx(abr, &offl);
    TEST_ASSERT_NULL(_nib_abr_iter_pfx(abr, NULL));
}

/*
 * Creates an ABR entry and a prefix and tries to add that prefix.
 * Expected result: the ABR's prefix list should be changed.
 */
static void test_nib_abr_add_pfx__pfx_in_nib(void)
{
    _nib_abr_entry_t *abr;
    _nib_offl_entry_t *dst;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };


    TEST_ASSERT_NOT_NULL((abr = _nib_abr_add(&addr)));
    TEST_ASSERT_NOT_NULL((dst = _nib_pl_add(IFACE, &pfx, GLOBAL_PREFIX_LEN,
                                            UINT32_MAX, UINT32_MAX)));
    TEST_ASSERT_NULL(_nib_abr_iter_pfx(abr, NULL));
    _nib_abr_add_pfx(abr, dst);
    TEST_ASSERT_NOT_NULL(_nib_abr_iter_pfx(abr, NULL));
}

/*
 * Iterates over prefixes of ABR with no prefix entries
 * Expected result: _nib_abr_pfx_iter returns NULL
 */
static void test_nib_abr_iter_pfx__empty(void)
{
    _nib_abr_entry_t *abr;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((abr = _nib_abr_add(&addr)));
    TEST_ASSERT_NULL(_nib_abr_iter_pfx(abr, NULL));
}

/*
 * Iterates over empty ABR entries
 * Expected result: _nib_abr_iter returns NULL
 */
static void test_nib_abr_iter__empty(void)
{
    TEST_ASSERT_NULL(_nib_abr_iter(NULL));
}

/*
 * Iterates over ABR entries with one element
 * Expected result: _nib_abr_iter returns element with NULL, and with that
 * element NULL.
 */
static void test_nib_abr_iter__one_elem(void)
{
    _nib_abr_entry_t *abr, *res;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((abr = _nib_abr_add(&addr)));
    TEST_ASSERT_NOT_NULL((res = _nib_abr_iter(NULL)));
    TEST_ASSERT(res == abr);
    TEST_ASSERT_NULL(_nib_abr_iter(res));
}

/*
 * Iterates over ABR entries with three element
 * Expected result: _nib_abr_iter returns element with NULL, with that element
 * another, with that element yet another and with the last NULL.
 */
static void test_nib_abr_iter__three_elem(void)
{
    _nib_abr_entry_t *abr1, *abr2, *abr3, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((abr1 = _nib_abr_add(&addr)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((abr2 = _nib_abr_add(&addr)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((abr3 = _nib_abr_add(&addr)));
    TEST_ASSERT_NOT_NULL((res = _nib_abr_iter(NULL)));
    TEST_ASSERT(res == abr1);
    TEST_ASSERT_NOT_NULL((res = _nib_abr_iter(res)));
    TEST_ASSERT(res == abr2);
    TEST_ASSERT_NOT_NULL((res = _nib_abr_iter(res)));
    TEST_ASSERT(res == abr3);
    TEST_ASSERT_NULL(_nib_abr_iter(res));
}

/*
 * Iterates over ABR entries with two elements, where there is a hole in the
 * internal array
 * Expected result: _nib_abr_iter returns element with NULL, with that element
 * another, and with the last NULL.
 */
static void test_nib_abr_iter__three_elem_middle_removed(void)
{
    _nib_abr_entry_t *abr1, *abr2, *res;
    ipv6_addr_t addr = { .u64 = { { .u8 = LINK_LOCAL_PREFIX },
                                { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_NOT_NULL((abr1 = _nib_abr_add(&addr)));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL(_nib_abr_add(&addr));
    addr.u64[1].u64++;
    TEST_ASSERT_NOT_NULL((abr2 = _nib_abr_add(&addr)));
    addr.u64[1].u64--;
    _nib_abr_remove(&addr);
    TEST_ASSERT_NOT_NULL((res = _nib_abr_iter(NULL)));
    TEST_ASSERT(res == abr1);
    TEST_ASSERT_NOT_NULL((res = _nib_abr_iter(res)));
    TEST_ASSERT(res == abr2);
    TEST_ASSERT_NULL(_nib_abr_iter(res));
}
#endif

static void test_retrans_exp_backoff(void)
{
    TEST_ASSERT_EQUAL_INT(0,
            _exp_backoff_retrans_timer_factor(0, 0, NDP_MIN_RANDOM_FACTOR));
    /* factor 1000 means multiplied by 1 */
    TEST_ASSERT_EQUAL_INT(NDP_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(0, NDP_RETRANS_TIMER_MS, 1000));
    TEST_ASSERT_EQUAL_INT(2 * NDP_RETRANS_TIMER_MS,     /* 2^1 = 2 */
            _exp_backoff_retrans_timer_factor(1, NDP_RETRANS_TIMER_MS, 1000));
    TEST_ASSERT_EQUAL_INT(4 * NDP_RETRANS_TIMER_MS,     /* 2^2 = 4 */
            _exp_backoff_retrans_timer_factor(2, NDP_RETRANS_TIMER_MS, 1000));
    TEST_ASSERT_EQUAL_INT(8 * NDP_RETRANS_TIMER_MS,     /* 2^3 = 8 */
            _exp_backoff_retrans_timer_factor(3, NDP_RETRANS_TIMER_MS, 1000));
    TEST_ASSERT_EQUAL_INT(16 * NDP_RETRANS_TIMER_MS,    /* 2^4 = 16 */
            _exp_backoff_retrans_timer_factor(4, NDP_RETRANS_TIMER_MS, 1000));
    TEST_ASSERT_EQUAL_INT(32 * NDP_RETRANS_TIMER_MS,    /* 2^5 = 32 */
            _exp_backoff_retrans_timer_factor(5, NDP_RETRANS_TIMER_MS, 1000));
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(6, NDP_RETRANS_TIMER_MS, 1000));
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(0, UINT32_MAX,
                                              NDP_MIN_RANDOM_FACTOR));
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(0, UINT32_MAX,
                                              NDP_MAX_RANDOM_FACTOR - 1));
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(NDP_MAX_NS_NUMOF, UINT32_MAX,
                                              NDP_MAX_RANDOM_FACTOR - 1));
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(NDP_MAX_NS_NUMOF,
                                              NDP_RETRANS_TIMER_MS,
                                              NDP_MAX_RANDOM_FACTOR - 1));
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(NDP_MAX_NS_NUMOF,
                                              NDP_RETRANS_TIMER_MS,
                                              NDP_MIN_RANDOM_FACTOR));
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(NDP_MAX_NS_NUMOF,
                                              NDP_MAX_RETRANS_TIMER_MS,
                                              NDP_MAX_RANDOM_FACTOR - 1));
    TEST_ASSERT_EQUAL_INT(32768,
            _exp_backoff_retrans_timer_factor(NDP_MAX_NS_NUMOF - 1, 1,
                                              NDP_MIN_RANDOM_FACTOR));
    TEST_ASSERT_EQUAL_INT(47653,
            _exp_backoff_retrans_timer_factor(5U, 1118U, 1332));
    TEST_ASSERT_EQUAL_INT(47653,
            _exp_backoff_retrans_timer_factor(5U, 1118U, 1332));
    /* test 64-bit overfrow */
    TEST_ASSERT_EQUAL_INT(NDP_MAX_RETRANS_TIMER_MS,
            _exp_backoff_retrans_timer_factor(NDP_MAX_NS_NUMOF, 32768, 1024));
}

Test *tests_gnrc_ipv6_nib_internal_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nib_alloc__no_space_left_diff_addr),
        new_TestFixture(test_nib_alloc__no_space_left_diff_iface),
        new_TestFixture(test_nib_alloc__no_space_left_diff_addr_iface),
        new_TestFixture(test_nib_alloc__success_duplicate),
        new_TestFixture(test_nib_alloc__success_noaddr_override),
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
        new_TestFixture(test_nib_nc_add__cache_out_crash),
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
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_iface),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop_iface),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_pfx),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop_pfx),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_iface_pfx),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop_iface_pfx),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_pfx_len),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop_pfx_len),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_iface_pfx_len),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop_iface_pfx_len),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_pfx_pfx_len),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop_pfx_pfx_len),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_iface_pfx_pfx_len),
        new_TestFixture(test_nib_offl_alloc__no_space_left_diff_next_hop_iface_pfx_pfx_len),
        new_TestFixture(test_nib_offl_alloc__success_duplicate),
        new_TestFixture(test_nib_offl_alloc__success_overwrite_unspecified),
        new_TestFixture(test_nib_offl_alloc__success),
        new_TestFixture(test_nib_offl_clear__uncleared),
        new_TestFixture(test_nib_offl_clear__same_next_hop),
        new_TestFixture(test_nib_offl_clear__cleared),
        new_TestFixture(test_nib_offl_iter__empty),
        new_TestFixture(test_nib_offl_iter__one_elem),
        new_TestFixture(test_nib_offl_iter__three_elem),
        new_TestFixture(test_nib_offl_iter__three_elem_middle_removed),
#if GNRC_IPV6_NIB_CONF_DC
        new_TestFixture(test_nib_dc_add__success),
        new_TestFixture(test_nib_dc_remove),
#endif
        new_TestFixture(test_nib_pl_add__success),
        new_TestFixture(test_nib_pl_remove),
        new_TestFixture(test_nib_ft_add__success),
        new_TestFixture(test_nib_ft_remove),
#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
        new_TestFixture(test_nib_abr_add__no_space_left),
        new_TestFixture(test_nib_abr_add__success_duplicate),
        new_TestFixture(test_nib_abr_add__success),
        new_TestFixture(test_nib_abr_remove__success),
        new_TestFixture(test_nib_abr_add_pfx__pfx_not_in_nib),
        new_TestFixture(test_nib_abr_add_pfx__pfx_in_nib),
        new_TestFixture(test_nib_abr_iter_pfx__empty),
        /* rest of _nib_abr_iter_pfx() tested through _nib_abr_add_pfx() tests */
        new_TestFixture(test_nib_abr_iter__empty),
        new_TestFixture(test_nib_abr_iter__one_elem),
        new_TestFixture(test_nib_abr_iter__three_elem),
        new_TestFixture(test_nib_abr_iter__three_elem_middle_removed),
#endif
        new_TestFixture(test_retrans_exp_backoff),
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL,
                        fixtures);

    return (Test *)&tests;
}
