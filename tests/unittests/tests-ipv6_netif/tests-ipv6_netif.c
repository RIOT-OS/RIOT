/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"

#include "byteorder.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/ipv6/netif.h"

#include "unittests-constants.h"
#include "tests-ipv6_netif.h"

/* default interface for testing */
#define DEFAULT_TEST_NETIF      (TEST_UINT16)
/* default IPv6 addr for testing */
#define DEFAULT_TEST_IPV6_ADDR  { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }
/* default IPv6 prefix length */
#define DEFAULT_TEST_PREFIX_LEN (18)

/* another interface for testing */
#define OTHER_TEST_NETIF        (TEST_UINT16 + TEST_UINT8)
/* another IPv6 addr for testing */
#define OTHER_TEST_IPV6_ADDR    { { \
            0x0f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }
/* matches DEFAULT_TEST_IPV6_ADDR with 18 byte */
#define DEFAULT_TEST_IPV6_PREFIX18  { { \
            0x00, 0x01, 0x22, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }
/* matches DEFAULT_TEST_IPV6_ADDR with 23 byte */
#define DEFAULT_TEST_IPV6_PREFIX23  { { \
            0x00, 0x01, 0x03, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }
/* matches DEFAULT_TEST_IPV6_ADDR with 64 byte */
#define DEFAULT_TEST_IPV6_PREFIX64  { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x88, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }

static void set_up(void)
{
    gnrc_netif_init();
    gnrc_ipv6_netif_init();
}

static void test_ipv6_netif_add__success(void)
{
    gnrc_ipv6_netif_t *entry;

    gnrc_ipv6_netif_add(DEFAULT_TEST_NETIF);

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_netif_get(DEFAULT_TEST_NETIF)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->pid);
}

static void test_netif_add__success_with_ipv6(void)
{
    kernel_pid_t pids[GNRC_NETIF_NUMOF];
    size_t pid_num;
    gnrc_ipv6_netif_t *entry;
    ipv6_addr_t exp_addr = IPV6_ADDR_ALL_NODES_LINK_LOCAL;

    gnrc_netif_add(DEFAULT_TEST_NETIF);

    TEST_ASSERT_EQUAL_INT(1, (pid_num = gnrc_netif_get(pids)));
    TEST_ASSERT_EQUAL_INT(1, pid_num);
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, pids[0]);
    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_netif_get(DEFAULT_TEST_NETIF)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->pid);
    TEST_ASSERT_EQUAL_STRING((char *)exp_addr.u8, (char *)entry->addrs[0].addr.u8);
}

static void test_ipv6_netif_add__despite_free_entry(void)
{
    /* Tests for possible duplicates as described in http://github.com/RIOT-OS/RIOT/issues/2965 */

    gnrc_ipv6_netif_add(OTHER_TEST_NETIF);
    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    /* create space by removing first entry */
    gnrc_ipv6_netif_remove(OTHER_TEST_NETIF);

    /* add DEFAULT_TEST_NETIF yet again and remove it */
    test_ipv6_netif_add__success();
    gnrc_ipv6_netif_remove(DEFAULT_TEST_NETIF);

    /* see if there's a duplicate left */
    TEST_ASSERT_NULL(gnrc_ipv6_netif_get(DEFAULT_TEST_NETIF));
}

static void test_ipv6_netif_remove__not_allocated(void)
{
    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */
    gnrc_ipv6_netif_remove(OTHER_TEST_NETIF);
    TEST_ASSERT_NULL(gnrc_ipv6_netif_get(OTHER_TEST_NETIF));
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_get(DEFAULT_TEST_NETIF));
}

static void test_ipv6_netif_remove__success(void)
{
    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */
    gnrc_ipv6_netif_remove(DEFAULT_TEST_NETIF);
    TEST_ASSERT_NULL(gnrc_ipv6_netif_get(DEFAULT_TEST_NETIF));
}

static void test_ipv6_netif_get__empty(void)
{
    for (kernel_pid_t pid = KERNEL_PID_FIRST; pid <= KERNEL_PID_LAST; pid++) {
        TEST_ASSERT_NULL(gnrc_ipv6_netif_get(pid));
    }
}

static void test_ipv6_netif_add_addr__no_iface1(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr, DEFAULT_TEST_PREFIX_LEN, 0));
}

static void test_ipv6_netif_add_addr__no_iface2(void)
{
    TEST_ASSERT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, NULL, DEFAULT_TEST_PREFIX_LEN, 0));
}

static void test_ipv6_netif_add_addr__addr_NULL(void)
{
    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    TEST_ASSERT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, NULL, DEFAULT_TEST_PREFIX_LEN, 0));
}

static void test_ipv6_netif_add_addr__addr_unspecified(void)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    TEST_ASSERT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr, DEFAULT_TEST_PREFIX_LEN, 0));
}

static void test_ipv6_netif_add_addr__full(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR, *res = &addr;
    int i;

    /* make link local to avoid automatic link local adding */
    ipv6_addr_set_link_local_prefix(&addr);

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    for (i = 0; res != NULL; i++, addr.u8[15]++) {
        res =  gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr, DEFAULT_TEST_PREFIX_LEN, 0);
    }
}

static void test_ipv6_netif_add_addr__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr,
                                                  DEFAULT_TEST_PREFIX_LEN, 0));
}

static void test_ipv6_netif_add_addr__despite_free_entry(void)
{
    /* Tests for possible duplicates as described in http://github.com/RIOT-OS/RIOT/issues/2965 */
    ipv6_addr_t *entry_1;
    ipv6_addr_t *entry_2;

    ipv6_addr_t default_addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t sol_addr;

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    /* add addresses to the interface */
    TEST_ASSERT_NOT_NULL((entry_1 = gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &default_addr,
                                                             DEFAULT_TEST_PREFIX_LEN, 0)));

    /* remove default_addr, but not the corresponding solicited-node addr that
     * came with it */
    gnrc_ipv6_netif_remove_addr(DEFAULT_TEST_NETIF, &default_addr);

    /* create and re-add corresponding solicited node address and check that it
     * hasn't taken the old place of default_addr*/
    ipv6_addr_set_solicited_nodes(&sol_addr, &default_addr);
    TEST_ASSERT_NOT_NULL((entry_2 = gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &sol_addr,
                                                             DEFAULT_TEST_PREFIX_LEN, 0)));

    TEST_ASSERT(entry_1 != entry_2);
}

static void test_ipv6_netif_remove_addr__not_allocated(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t other_addr = OTHER_TEST_IPV6_ADDR;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    gnrc_ipv6_netif_remove_addr(DEFAULT_TEST_NETIF, &other_addr);

    TEST_ASSERT_NULL(gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &other_addr));
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_netif_remove_addr__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    gnrc_ipv6_netif_remove_addr(DEFAULT_TEST_NETIF, &addr);

    TEST_ASSERT_NULL(gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_netif_reset_addr__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    gnrc_ipv6_netif_reset_addr(DEFAULT_TEST_NETIF);

    TEST_ASSERT_NULL(gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_netif_find_by_addr__empty(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t *out = NULL;

    TEST_ASSERT_EQUAL_INT(KERNEL_PID_UNDEF, gnrc_ipv6_netif_find_by_addr(&out, &addr));
    TEST_ASSERT_NULL(out);
}

static void test_ipv6_netif_find_by_addr__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, gnrc_ipv6_netif_find_by_addr(&out, &addr));
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(out, &addr));
}

static void test_ipv6_netif_find_addr__no_iface(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_NULL(gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_netif_find_addr__wrong_iface(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_NULL(gnrc_ipv6_netif_find_addr(OTHER_TEST_NETIF, &addr));
}

static void test_ipv6_netif_find_addr__wrong_addr(void)
{
    ipv6_addr_t addr = OTHER_TEST_IPV6_ADDR;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_NULL(gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_netif_find_addr__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(out, &addr));

    /* also test for link local address */
    ipv6_addr_set_solicited_nodes(&addr, &addr);
    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(out, &addr));
}

static void test_ipv6_netif_find_by_prefix__success1(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_PREFIX23;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, gnrc_ipv6_netif_find_by_prefix(&out, &addr));
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(23, ipv6_addr_match_prefix(out, &addr));
}

static void test_ipv6_netif_find_by_prefix__success2(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_PREFIX18;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, gnrc_ipv6_netif_find_by_prefix(&out, &addr));
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(18, ipv6_addr_match_prefix(out, &addr));
}

static void test_ipv6_netif_find_by_prefix__success3(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_PREFIX64;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, gnrc_ipv6_netif_find_by_prefix(&out, &addr));
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(64, ipv6_addr_match_prefix(out, &addr));
}

static void test_ipv6_netif_match_prefix__success1(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_PREFIX23;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_match_prefix(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(23, ipv6_addr_match_prefix(out, &addr));
}

static void test_ipv6_netif_match_prefix__success2(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_PREFIX18;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_match_prefix(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(18, ipv6_addr_match_prefix(out, &addr));
}

static void test_ipv6_netif_match_prefix__success3(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_PREFIX64;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_match_prefix(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT(out != &addr);
    TEST_ASSERT_EQUAL_INT(64, ipv6_addr_match_prefix(out, &addr));
}

static void test_ipv6_netif_find_best_src_addr__no_unicast(void)
{
    ipv6_addr_t ll_dst_addr = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t mc_addr1 = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ipv6_addr_t mc_addr2 = IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL;

    ll_dst_addr.u8[15] = 1;
    ipv6_addr_set_link_local_prefix(&ll_dst_addr);

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &mc_addr1,
                                                  DEFAULT_TEST_PREFIX_LEN, 0));
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &mc_addr2,
                                                  DEFAULT_TEST_PREFIX_LEN, 0));

    TEST_ASSERT_NULL(gnrc_ipv6_netif_find_best_src_addr(DEFAULT_TEST_NETIF, &ll_dst_addr, false));
}

static void test_ipv6_netif_find_best_src_addr__success(void)
{
    ipv6_addr_t ll_addr1 = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t ll_addr2 = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t mc_addr = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ipv6_addr_t *out = NULL;

    ll_addr1.u8[15] = 1;
    ipv6_addr_set_link_local_prefix(&ll_addr1);
    ll_addr2.u8[15] = 2;
    ipv6_addr_set_link_local_prefix(&ll_addr2);

    TEST_ASSERT_EQUAL_INT(126, ipv6_addr_match_prefix(&ll_addr2, &ll_addr1));

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &mc_addr,
                                                  DEFAULT_TEST_PREFIX_LEN, 0));
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &ll_addr1,
                                                  DEFAULT_TEST_PREFIX_LEN, 0));

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_best_src_addr(DEFAULT_TEST_NETIF, &ll_addr2, false)));
    TEST_ASSERT(out != &ll_addr1);
    TEST_ASSERT(out != &ll_addr2);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(out, &ll_addr1));
}

static void test_ipv6_netif_find_best_src_addr__multicast_input(void)
{
    ipv6_addr_t mc_addr = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
    ipv6_addr_t *out = NULL;

    /* Adds DEFAULT_TEST_NETIF as interface and to it fe80::1, fe80::2 and ff02::1 */
    test_ipv6_netif_find_best_src_addr__success();

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_best_src_addr(DEFAULT_TEST_NETIF, &mc_addr, false)));
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_equal(&mc_addr, out));
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_unspecified(out));
}

static void test_ipv6_netif_find_best_src_addr__other_subnet(void)
{
    ipv6_addr_t addr1 = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t addr2 = OTHER_TEST_IPV6_ADDR;

    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr1, DEFAULT_TEST_PREFIX_LEN,
                         0));

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_best_src_addr(DEFAULT_TEST_NETIF, &addr2, false)));
    TEST_ASSERT(out != &addr1);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(out, &addr1));
}

static void test_ipv6_netif_addr_is_non_unicast__unicast(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add_addr__success(); /* adds DEFAULT_TEST_IPV6_ADDR to
                                          * DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(false, gnrc_ipv6_netif_addr_is_non_unicast(out));
}

static void test_ipv6_netif_addr_is_non_unicast__anycast(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr, DEFAULT_TEST_PREFIX_LEN,
                         GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST));

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(true, gnrc_ipv6_netif_addr_is_non_unicast(out));
}

static void test_ipv6_netif_addr_is_non_unicast__multicast1(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr,
                                                  DEFAULT_TEST_PREFIX_LEN, 0));

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(true, gnrc_ipv6_netif_addr_is_non_unicast(out));
}

static void test_ipv6_netif_addr_is_non_unicast__multicast2(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
    ipv6_addr_t *out = NULL;

    test_ipv6_netif_add__success(); /* adds DEFAULT_TEST_NETIF as interface */

    TEST_ASSERT_NOT_NULL(gnrc_ipv6_netif_add_addr(DEFAULT_TEST_NETIF, &addr, DEFAULT_TEST_PREFIX_LEN,
                                                  GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST));

    TEST_ASSERT_NOT_NULL((out = gnrc_ipv6_netif_find_addr(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(true, gnrc_ipv6_netif_addr_is_non_unicast(out));
}

Test *tests_ipv6_netif_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_netif_add__success),
        new_TestFixture(test_netif_add__success_with_ipv6),
        new_TestFixture(test_ipv6_netif_add__despite_free_entry),
        new_TestFixture(test_ipv6_netif_remove__not_allocated),
        new_TestFixture(test_ipv6_netif_remove__success),
        new_TestFixture(test_ipv6_netif_get__empty),
        new_TestFixture(test_ipv6_netif_add_addr__no_iface1),
        new_TestFixture(test_ipv6_netif_add_addr__no_iface2),
        new_TestFixture(test_ipv6_netif_add_addr__addr_NULL),
        new_TestFixture(test_ipv6_netif_add_addr__addr_unspecified),
        new_TestFixture(test_ipv6_netif_add_addr__full),
        new_TestFixture(test_ipv6_netif_add_addr__success),
        new_TestFixture(test_ipv6_netif_add_addr__despite_free_entry),
        new_TestFixture(test_ipv6_netif_remove_addr__not_allocated),
        new_TestFixture(test_ipv6_netif_remove_addr__success),
        new_TestFixture(test_ipv6_netif_reset_addr__success),
        new_TestFixture(test_ipv6_netif_find_by_addr__empty),
        new_TestFixture(test_ipv6_netif_find_by_addr__success),
        new_TestFixture(test_ipv6_netif_find_addr__no_iface),
        new_TestFixture(test_ipv6_netif_find_addr__wrong_iface),
        new_TestFixture(test_ipv6_netif_find_addr__wrong_addr),
        new_TestFixture(test_ipv6_netif_find_addr__success),
        new_TestFixture(test_ipv6_netif_find_by_prefix__success1),
        new_TestFixture(test_ipv6_netif_find_by_prefix__success2),
        new_TestFixture(test_ipv6_netif_find_by_prefix__success3),
        new_TestFixture(test_ipv6_netif_match_prefix__success1),
        new_TestFixture(test_ipv6_netif_match_prefix__success2),
        new_TestFixture(test_ipv6_netif_match_prefix__success3),
        new_TestFixture(test_ipv6_netif_find_best_src_addr__no_unicast),
        new_TestFixture(test_ipv6_netif_find_best_src_addr__success),
        new_TestFixture(test_ipv6_netif_find_best_src_addr__multicast_input),
        new_TestFixture(test_ipv6_netif_find_best_src_addr__other_subnet),
        new_TestFixture(test_ipv6_netif_addr_is_non_unicast__unicast),
        new_TestFixture(test_ipv6_netif_addr_is_non_unicast__anycast),
        new_TestFixture(test_ipv6_netif_addr_is_non_unicast__multicast1),
        new_TestFixture(test_ipv6_netif_addr_is_non_unicast__multicast2),
    };

    EMB_UNIT_TESTCALLER(ipv6_netif_tests, set_up, NULL, fixtures);

    return (Test *)&ipv6_netif_tests;
}

void tests_ipv6_netif(void)
{
    TESTS_RUN(tests_ipv6_netif_tests());
}
/** @} */
