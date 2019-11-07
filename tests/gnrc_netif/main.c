/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests default configuration of GNRC's Network Information Base
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "common.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "net/ethernet.h"
#include "net/ipv6.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc/netif/internal.h"
#include "net/netdev_test.h"
#include "net/netif.h"
#include "utlist.h"
#include "xtimer.h"

#define ETHERNET_STACKSIZE          (THREAD_STACKSIZE_MAIN)
#define IEEE802154_STACKSIZE        (THREAD_STACKSIZE_MAIN)

static gnrc_netif_t *ethernet_netif = NULL;
static gnrc_netif_t *ieee802154_netif = NULL;
static gnrc_netif_t *netifs[DEFAULT_DEVS_NUMOF];
static char ethernet_netif_stack[ETHERNET_STACKSIZE];
static char ieee802154_netif_stack[ETHERNET_STACKSIZE];
static char netifs_stack[DEFAULT_DEVS_NUMOF][THREAD_STACKSIZE_DEFAULT];
static bool init_called = false;

static inline void _test_init(gnrc_netif_t *netif);
static inline int _mock_netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static inline gnrc_pktsnip_t *_mock_netif_recv(gnrc_netif_t * netif);
static int _get_netdev_address(netdev_t *dev, void *value, size_t max_len);
static int _set_netdev_address(netdev_t *dev, const void *value,
                               size_t value_len);
static int _get_netdev_address_long(netdev_t *dev, void *value, size_t max_len);
static int _set_netdev_address_long(netdev_t *dev, const void *value,
                                    size_t value_len);
static int _get_netdev_src_len(netdev_t *dev, void *value, size_t max_len);
static int _set_netdev_src_len(netdev_t *dev, const void *value,
                               size_t value_len);

static const gnrc_netif_ops_t default_ops = {
    .init = _test_init,
    .send = _mock_netif_send,
    .recv = _mock_netif_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
    .msg_handler = NULL,
};

static void _set_up(void)
{
    msg_t msg;

    if (ethernet_netif != NULL) {
        memset(ethernet_netif->ipv6.addrs_flags, 0,
               sizeof(ethernet_netif->ipv6.addrs_flags));
        memset(ethernet_netif->ipv6.addrs, 0,
               sizeof(ethernet_netif->ipv6.addrs));
        memset(ethernet_netif->ipv6.groups, 0,
               sizeof(ethernet_netif->ipv6.groups));
    }
    if (ieee802154_netif != NULL) {
        memset(ieee802154_netif->ipv6.addrs_flags, 0,
               sizeof(ieee802154_netif->ipv6.addrs_flags));
        memset(ieee802154_netif->ipv6.addrs, 0,
               sizeof(ieee802154_netif->ipv6.addrs));
        memset(ieee802154_netif->ipv6.groups, 0,
               sizeof(ieee802154_netif->ipv6.groups));
    }
    for (unsigned i = 0; i < DEFAULT_DEVS_NUMOF; i++) {
        if (netifs[i] != NULL) {
            memset(netifs[i]->ipv6.addrs_flags, 0,
                   sizeof(netifs[i]->ipv6.addrs_flags));
            memset(netifs[i]->ipv6.addrs, 0, sizeof(netifs[i]->ipv6.addrs));
            memset(netifs[i]->ipv6.groups, 0, sizeof(netifs[i]->ipv6.groups));
        }
    }
    /* empty message queue */
    while (msg_try_receive(&msg) > 0) {}
}

static inline void _test_init(gnrc_netif_t *netif)
{
    (void)netif;
    gnrc_netif_default_init(netif);
    init_called = true;
}

static void test_creation(void)
{
    gnrc_netif_t *ptr = NULL;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_numof());
    TEST_ASSERT_NULL(gnrc_netif_iter(ptr));
    TEST_ASSERT_NOT_NULL((ethernet_netif = gnrc_netif_ethernet_create(
            ethernet_netif_stack, ETHERNET_STACKSIZE, GNRC_NETIF_PRIO,
            "eth", ethernet_dev
        )));
    TEST_ASSERT_EQUAL_INT(1, gnrc_netif_numof());
    TEST_ASSERT_NOT_NULL((ptr = gnrc_netif_iter(ptr)));
    TEST_ASSERT_NULL((ptr = gnrc_netif_iter(ptr)));
    TEST_ASSERT_NOT_NULL(ethernet_netif->ops);
    TEST_ASSERT_NOT_NULL(ethernet_netif->dev);
    TEST_ASSERT_EQUAL_INT(ETHERNET_DATA_LEN, ethernet_netif->ipv6.mtu);
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_DEFAULT_HL, ethernet_netif->cur_hl);
    TEST_ASSERT_EQUAL_INT(NETDEV_TYPE_ETHERNET, ethernet_netif->device_type);
    TEST_ASSERT(ethernet_netif->pid > KERNEL_PID_UNDEF);
#ifdef DEVELHELP
    TEST_ASSERT_EQUAL_STRING("eth", sched_threads[ethernet_netif->pid]->name);
#endif
    TEST_ASSERT(thread_has_msg_queue(sched_threads[ethernet_netif->pid]));

    TEST_ASSERT_NOT_NULL((ieee802154_netif = gnrc_netif_ieee802154_create(
            ieee802154_netif_stack, IEEE802154_STACKSIZE, GNRC_NETIF_PRIO,
            "wpan", ieee802154_dev
        )));
    TEST_ASSERT_EQUAL_INT(2, gnrc_netif_numof());
    TEST_ASSERT_NOT_NULL((ptr = gnrc_netif_iter(ptr)));
    TEST_ASSERT_NOT_NULL((ptr = gnrc_netif_iter(ptr)));
    TEST_ASSERT_NULL((ptr = gnrc_netif_iter(ptr)));
    TEST_ASSERT_NOT_NULL(ieee802154_netif->ops);
    TEST_ASSERT_NOT_NULL(ieee802154_netif->dev);
    TEST_ASSERT_EQUAL_INT(IPV6_MIN_MTU, ieee802154_netif->ipv6.mtu);
    TEST_ASSERT_EQUAL_INT(TEST_IEEE802154_MAX_FRAG_SIZE,
                          ieee802154_netif->sixlo.max_frag_size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_DEFAULT_HL, ieee802154_netif->cur_hl);
    TEST_ASSERT_EQUAL_INT(NETDEV_TYPE_IEEE802154,
                          ieee802154_netif->device_type);
    TEST_ASSERT(ieee802154_netif->pid > KERNEL_PID_UNDEF);
#ifdef DEVELHELP
    TEST_ASSERT_EQUAL_STRING("wpan", sched_threads[ieee802154_netif->pid]->name);
#endif
    TEST_ASSERT(thread_has_msg_queue(sched_threads[ieee802154_netif->pid]));

    for (unsigned i = 0; i < DEFAULT_DEVS_NUMOF; i++) {
        TEST_ASSERT_NOT_NULL((netifs[i] = gnrc_netif_create(
                netifs_stack[i], THREAD_STACKSIZE_DEFAULT,
                GNRC_NETIF_PRIO, "netif", devs[i], &default_ops
            )));
        TEST_ASSERT_NOT_NULL(netifs[i]->ops);
        TEST_ASSERT_NOT_NULL(netifs[i]->dev);
        TEST_ASSERT_EQUAL_INT(GNRC_NETIF_DEFAULT_HL, netifs[i]->cur_hl);
        TEST_ASSERT_EQUAL_INT(NETDEV_TYPE_TEST, netifs[i]->device_type);
        TEST_ASSERT(netifs[i]->pid > KERNEL_PID_UNDEF);
        TEST_ASSERT(thread_has_msg_queue(sched_threads[netifs[i]->pid]));
        TEST_ASSERT_EQUAL_INT(i + SPECIAL_DEVS + 1, gnrc_netif_numof());
        for (unsigned j = 0; j < (i + SPECIAL_DEVS + 1); j++) {
            TEST_ASSERT_NOT_NULL((ptr = gnrc_netif_iter(ptr)));
        }
        TEST_ASSERT_NULL((ptr = gnrc_netif_iter(ptr)));
    }
    TEST_ASSERT(init_called);
}

static void test_get_by_pid(void)
{
    TEST_ASSERT(ethernet_netif == gnrc_netif_get_by_pid(ethernet_netif->pid));
    TEST_ASSERT(ieee802154_netif == gnrc_netif_get_by_pid(ieee802154_netif->pid));
    for (kernel_pid_t i = 0; i < DEFAULT_DEVS_NUMOF; i++) {
        TEST_ASSERT(netifs[i] == gnrc_netif_get_by_pid(netifs[i]->pid));
    }
}

static void test_addr_to_str(void)
{
    static const uint8_t ethernet_l2addr[] = ETHERNET_SRC;
    static const uint8_t ieee802154_l2addr_long[] = IEEE802154_LONG_SRC;
    static const uint8_t ieee802154_l2addr_short[] = IEEE802154_SHORT_SRC;
    static const uint8_t netif0_l2addr[] = NETIF0_SRC;
    char out[sizeof(netif0_l2addr) * 3];

    TEST_ASSERT(out == gnrc_netif_addr_to_str(NULL, 0, out));
    TEST_ASSERT_EQUAL_STRING("", &out[0]);
    TEST_ASSERT(out == gnrc_netif_addr_to_str(ethernet_l2addr,
                                              sizeof(ethernet_l2addr), out));
    TEST_ASSERT_EQUAL_STRING("3E:E6:B5:22:FD:0A", &out[0]);
    TEST_ASSERT(out == gnrc_netif_addr_to_str(ieee802154_l2addr_long,
                                              sizeof(ieee802154_l2addr_long),
                                              out));
    TEST_ASSERT_EQUAL_STRING("3E:E6:B5:0F:19:22:FD:0A", &out[0]);
    TEST_ASSERT(out == gnrc_netif_addr_to_str(ieee802154_l2addr_short,
                                              sizeof(ieee802154_l2addr_short),
                                              out));
    TEST_ASSERT_EQUAL_STRING("FD:0A", &out[0]);
    TEST_ASSERT(out == gnrc_netif_addr_to_str(netif0_l2addr,
                                              sizeof(netif0_l2addr),
                                              out));
    TEST_ASSERT_EQUAL_STRING("3E:E7:B5:0F:19:22:FD:0A", &out[0]);
}

static void test_addr_from_str(void)
{
    static const uint8_t ethernet_l2addr[] = ETHERNET_SRC;
    static const uint8_t ieee802154_l2addr_long[] = IEEE802154_LONG_SRC;
    static const uint8_t ieee802154_l2addr_short[] = IEEE802154_SHORT_SRC;
    uint8_t out[GNRC_NETIF_L2ADDR_MAXLEN];

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_addr_from_str("", out));
    TEST_ASSERT_EQUAL_INT(sizeof(ethernet_l2addr),
                          gnrc_netif_addr_from_str("3E:E6:B5:22:FD:0A", out));
    TEST_ASSERT_EQUAL_INT(0, memcmp(ethernet_l2addr, out,
                                    sizeof(ethernet_l2addr)));
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_long),
                          gnrc_netif_addr_from_str("3E:E6:B5:0F:19:22:FD:0A",
                                                   out));
    TEST_ASSERT_EQUAL_INT(0, memcmp(ieee802154_l2addr_long, out,
                                    sizeof(ieee802154_l2addr_long)));
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_short),
                          gnrc_netif_addr_from_str("FD:0A", out));
    TEST_ASSERT_EQUAL_INT(0, memcmp(ieee802154_l2addr_short, out,
                                    sizeof(ieee802154_l2addr_short)));
}

static void test_ipv6_addr_add__ENOMEM(void)
{
    ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    for (unsigned i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF;
         i++, addr.u16[3].u16++) {
        TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                                  GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM,
                          gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                                   GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
}

static void test_ipv6_addr_add__success(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_LL };
    int idx;

    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)));
    /* check duplicate addition */
    TEST_ASSERT_EQUAL_INT(idx,
                          gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                                   GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID,
                          netifs[0]->ipv6.addrs_flags[idx]);
    TEST_ASSERT(ipv6_addr_equal(&addr, &netifs[0]->ipv6.addrs[idx]));
}

static void test_ipv6_addr_add__readd_with_free_entry(void)
{
    /* Tests for possible duplicates (see #2965) */
    static const ipv6_addr_t addr1 = { .u8 = NETIF0_IPV6_LL };
    static const ipv6_addr_t addr2 = { .u8 = NETIF0_IPV6_G };
    int idx;

    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr1, 64U,
                                              GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr2, 64U,
                                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)));
    gnrc_netif_ipv6_addr_remove_internal(netifs[0], &addr1);
    TEST_ASSERT_EQUAL_INT(idx,
                          gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr2, 64U,
                                                   GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
}

static void test_ipv6_addr_remove__not_allocated(void)
{
    static const ipv6_addr_t addr1 = { .u8 = NETIF0_IPV6_LL };
    static const ipv6_addr_t addr2 = { .u8 = NETIF0_IPV6_G };

    test_ipv6_addr_add__success();
    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_idx(netifs[0], &addr1));
    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr2, 64U,
                                              GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    gnrc_netif_ipv6_addr_remove_internal(netifs[0], &addr2);
    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_idx(netifs[0], &addr1));
}

static void test_ipv6_addr_remove__success(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_LL };

    test_ipv6_addr_add__success();
    gnrc_netif_ipv6_addr_remove_internal(netifs[0], &addr);
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_idx(netifs[0], &addr));
}

static void test_ipv6_addr_idx__empty(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_LL };

    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_idx(netifs[0], &addr));
}

static void test_ipv6_addr_idx__unspecified_addr(void)
{
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_idx(netifs[0],
                                                       &ipv6_addr_unspecified));
}

static void test_ipv6_addr_idx__wrong_netif(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_LL };

    test_ipv6_addr_add__success();
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_idx(netifs[1], &addr));
}

static void test_ipv6_addr_idx__wrong_addr(void)
{
    static const ipv6_addr_t addr2 = { .u8 = NETIF0_IPV6_G };

    test_ipv6_addr_add__success();
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_idx(netifs[0], &addr2));
}

static void test_ipv6_addr_idx__success(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_LL };

    test_ipv6_addr_add__success();
    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_idx(netifs[0], &addr));
}

static void test_ipv6_addr_match__empty(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_match(netifs[0], &addr));
}

static void test_ipv6_addr_match__unspecified_addr(void)
{
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_match(netifs[0],
                                                         &ipv6_addr_unspecified));
}

static void test_ipv6_addr_match__wrong_netif(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    test_ipv6_addr_add__success();
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_match(netifs[1], &addr));
}

static void test_ipv6_addr_match__wrong_addr(void)
{
    static const ipv6_addr_t addr2 = { .u8 = NETIF0_IPV6_G };

    test_ipv6_addr_add__success();
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_addr_match(netifs[0], &addr2));
}

static void test_ipv6_addr_match__success18(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    static const ipv6_addr_t pfx = { .u8 = GLOBAL_PFX18 };
    int idx;

    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)));
    TEST_ASSERT_EQUAL_INT(idx, gnrc_netif_ipv6_addr_match(netifs[0], &pfx));
    TEST_ASSERT_EQUAL_INT(18, ipv6_addr_match_prefix(&netifs[0]->ipv6.addrs[idx],
                                                     &pfx));
}

static void test_ipv6_addr_match__success23(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    static const ipv6_addr_t pfx = { .u8 = GLOBAL_PFX23 };
    int idx;

    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)));
    TEST_ASSERT_EQUAL_INT(idx, gnrc_netif_ipv6_addr_match(netifs[0], &pfx));
    TEST_ASSERT_EQUAL_INT(23, ipv6_addr_match_prefix(&netifs[0]->ipv6.addrs[idx],
                                                     &pfx));
}

static void test_ipv6_addr_match__success64(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    static const ipv6_addr_t pfx = { .u8 = GLOBAL_PFX64 };
    int idx;

    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)));
    TEST_ASSERT_EQUAL_INT(idx, gnrc_netif_ipv6_addr_match(netifs[0], &pfx));
    TEST_ASSERT_EQUAL_INT(64, ipv6_addr_match_prefix(&netifs[0]->ipv6.addrs[idx],
                                                     &pfx));
}

static void test_ipv6_addr_best_src__multicast_input(void)
{
    static const ipv6_addr_t addr1 = { .u8 = NETIF0_IPV6_G };

    static const ipv6_addr_t addr2 = { .u8 = GLOBAL_PFX18 };
    ipv6_addr_t *out;

    /* adds a link-local address */
    test_ipv6_addr_add__success();
    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr1, 64U,
                                              GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    TEST_ASSERT_NOT_NULL((out = gnrc_netif_ipv6_addr_best_src(netifs[0],
                                                              &addr2,
                                                              false)));
    TEST_ASSERT(!ipv6_addr_equal(&addr2, out));
    TEST_ASSERT(ipv6_addr_equal(&addr1, out));
}

static void test_ipv6_addr_best_src__unspecified_addr(void)
{
    TEST_ASSERT_NULL(gnrc_netif_ipv6_addr_best_src(netifs[0],
                                                   &ipv6_addr_unspecified,
                                                   false));
}

static void test_ipv6_addr_best_src__other_subnet(void)
{
    static const ipv6_addr_t mc_addr = IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL;
    ipv6_addr_t *out = NULL;

    test_ipv6_addr_add__success();
    TEST_ASSERT_NOT_NULL((out = gnrc_netif_ipv6_addr_best_src(netifs[0],
                                                              &mc_addr,
                                                              false)));
    TEST_ASSERT(!ipv6_addr_equal(&mc_addr, out));
    TEST_ASSERT(!ipv6_addr_is_multicast(out));
    TEST_ASSERT(!ipv6_addr_is_unspecified(out));
}

static void test_ipv6_addr_best_src__ula_src_dst(void)
{
    static const ipv6_addr_t ula_src = { .u8 = NETIF0_IPV6_ULA };
    static const ipv6_addr_t ula_dst = { .u8 = { ULA1, ULA2, ULA3, ULA4,
                                                 ULA5, ULA6, ULA7, ULA8,
                                                 0, 0, 0, 0, 0, 0, 0, 1 } };
    ipv6_addr_t *out = NULL;
    int idx;

    test_ipv6_addr_add__success();
    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &ula_src, 64U,
                                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)));
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID,
                          netifs[0]->ipv6.addrs_flags[idx]);
    TEST_ASSERT(ipv6_addr_equal(&ula_src, &netifs[0]->ipv6.addrs[idx]));

    TEST_ASSERT_NOT_NULL((out = gnrc_netif_ipv6_addr_best_src(netifs[0],
                                                              &ula_dst,
                                                              false)));
    TEST_ASSERT(ipv6_addr_equal(&ula_src, out));
}

static void test_ipv6_addr_best_src__global_src_ula_dst(void)
{
    static const ipv6_addr_t src = { .u8 = NETIF0_IPV6_G };
    static const ipv6_addr_t ula_dst = { .u8 = { ULA1, ULA2, ULA3, ULA4,
                                                 ULA5, ULA6, ULA7, ULA8,
                                                 0, 0, 0, 0, 0, 0, 0, 1 } };
    ipv6_addr_t *out = NULL;
    int idx;

    test_ipv6_addr_add__success();  /* adds link-local address */
    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &src, 64U,
                                                     GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)));
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID,
                          netifs[0]->ipv6.addrs_flags[idx]);
    TEST_ASSERT(ipv6_addr_equal(&src, &netifs[0]->ipv6.addrs[idx]));

    TEST_ASSERT_NOT_NULL((out = gnrc_netif_ipv6_addr_best_src(netifs[0],
                                                              &ula_dst,
                                                              false)));
    TEST_ASSERT(ipv6_addr_equal(&src, out));
}

static void test_ipv6_addr_best_src__deprecated_addr(void)
{
    static const ipv6_addr_t src = { .u8 = { LP1, LP2, LP3, LP4,
                                             LP5, LP6, LP7, LP8,
                                             0, 0, 0, 0, 0, 0, 0, 2 } };
    static const ipv6_addr_t dst = { .u8 = { LP1, LP2, LP3, LP4,
                                             LP5, LP6, LP7, LP8,
                                             0, 0, 0, 0, 0, 0, 0, 1 } };
    ipv6_addr_t *out = NULL;
    int idx;
    const unsigned exp_match = ipv6_addr_match_prefix(&src, &dst);

    test_ipv6_addr_add__success();  /* adds EUI-64 based link-local address */
    /* ensure that current addresses have smaller matches */
    for (unsigned i = 0; i < GNRC_NETIF_IPV6_ADDRS_NUMOF; i++) {
        ipv6_addr_t *addr = &netifs[0]->ipv6.addrs[i];
        TEST_ASSERT(exp_match > ipv6_addr_match_prefix(addr, &dst));
    }
    /* add another link-local address but deprecated  */
    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_addr_add_internal(netifs[0], &src, 64U,
                                                    GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED)));
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED,
                          netifs[0]->ipv6.addrs_flags[idx]);
    TEST_ASSERT(ipv6_addr_equal(&src, &netifs[0]->ipv6.addrs[idx]));

    TEST_ASSERT_NOT_NULL((out = gnrc_netif_ipv6_addr_best_src(netifs[0],
                                                              &dst,
                                                              false)));
    /* should be not `src` as it is deprecated */
    TEST_ASSERT(!ipv6_addr_equal(&src, out));
}

static void test_get_by_ipv6_addr__empty(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_LL };

    TEST_ASSERT_NULL(gnrc_netif_get_by_ipv6_addr(&addr));
}

static void test_get_by_ipv6_addr__unspecified_addr(void)
{
    TEST_ASSERT_NULL(gnrc_netif_get_by_ipv6_addr(&ipv6_addr_unspecified));
}

static void test_get_by_ipv6_addr__success(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_LL };

    test_ipv6_addr_add__success();
    TEST_ASSERT_NOT_NULL(netifs[0]);
    TEST_ASSERT(netifs[0] == gnrc_netif_get_by_ipv6_addr(&addr));
}

static void test_get_by_prefix__empty(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    TEST_ASSERT_NULL(gnrc_netif_get_by_prefix(&addr));
}

static void test_get_by_prefix__unspecified_addr(void)
{
    TEST_ASSERT_NULL(gnrc_netif_get_by_prefix(&ipv6_addr_unspecified));
}

static void test_get_by_prefix__success18(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };
    static const ipv6_addr_t pfx = { .u8 = GLOBAL_PFX18 };

    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                              GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    TEST_ASSERT_NOT_NULL(netifs[0]);
    TEST_ASSERT(netifs[0] == gnrc_netif_get_by_prefix(&pfx));
    test_ipv6_addr_match__success18();
}

static void test_get_by_prefix__success23(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };
    static const ipv6_addr_t pfx = { .u8 = GLOBAL_PFX23 };

    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                              GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    TEST_ASSERT_NOT_NULL(netifs[0]);
    TEST_ASSERT(netifs[0] == gnrc_netif_get_by_prefix(&pfx));
    test_ipv6_addr_match__success23();
}

static void test_get_by_prefix__success64(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };
    static const ipv6_addr_t pfx = { .u8 = GLOBAL_PFX64 };

    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_add_internal(netifs[0], &addr, 64U,
                                              GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID));
    TEST_ASSERT_NOT_NULL(netifs[0]);
    TEST_ASSERT(netifs[0] == gnrc_netif_get_by_prefix(&pfx));
    test_ipv6_addr_match__success64();
}

static void test_ipv6_group_join__ENOMEM(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_NODES_LINK_LOCAL;

    for (unsigned i = 0; i < GNRC_NETIF_IPV6_GROUPS_NUMOF;
         i++, addr.u16[7].u16++) {
        TEST_ASSERT(0 <= gnrc_netif_ipv6_group_join_internal(netifs[0], &addr));
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM,
                          gnrc_netif_ipv6_group_join_internal(netifs[0], &addr));
}

static void test_ipv6_group_join__success(void)
{
    int idx;

    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_group_join_internal(netifs[0],
                                                       &ipv6_addr_all_nodes_link_local)));
    /* check duplicate addition */
    TEST_ASSERT_EQUAL_INT(idx,
                          gnrc_netif_ipv6_group_join_internal(netifs[0],
                                                     &ipv6_addr_all_nodes_link_local));
    TEST_ASSERT(ipv6_addr_equal(&ipv6_addr_all_nodes_link_local,
                                &netifs[0]->ipv6.groups[idx]));
}

static void test_ipv6_group_join__readd_with_free_entry(void)
{
    /* Tests for possible duplicates (see #2965) */
    int idx;

    TEST_ASSERT(0 <= gnrc_netif_ipv6_group_join_internal(netifs[0],
                                                &ipv6_addr_all_nodes_link_local));
    TEST_ASSERT(0 <= (idx = gnrc_netif_ipv6_group_join_internal(netifs[0],
                                                       &ipv6_addr_all_routers_link_local)));
    gnrc_netif_ipv6_group_leave_internal(netifs[0], &ipv6_addr_all_nodes_link_local);
    TEST_ASSERT_EQUAL_INT(idx,
                          gnrc_netif_ipv6_group_join_internal(netifs[0],
                                                     &ipv6_addr_all_routers_link_local));
}

static void test_ipv6_group_leave__not_allocated(void)
{
    test_ipv6_group_join__success();
    TEST_ASSERT(0 <= gnrc_netif_ipv6_group_idx(netifs[0],
                                               &ipv6_addr_all_nodes_link_local));
    TEST_ASSERT(0 <= gnrc_netif_ipv6_group_join_internal(netifs[0],
                                                &ipv6_addr_all_routers_link_local));
    gnrc_netif_ipv6_group_leave_internal(netifs[0], &ipv6_addr_all_routers_link_local);
    TEST_ASSERT(0 <= gnrc_netif_ipv6_group_idx(netifs[0],
                                               &ipv6_addr_all_nodes_link_local));
}

static void test_ipv6_group_leave__success(void)
{
    test_ipv6_group_join__success();
    gnrc_netif_ipv6_group_leave_internal(netifs[0], &ipv6_addr_all_nodes_link_local);
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_group_idx(netifs[0],
                                                        &ipv6_addr_all_nodes_link_local));
}

static void test_ipv6_group_idx__empty(void)
{
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_group_idx(netifs[0],
                                                        &ipv6_addr_all_nodes_link_local));
}

static void test_ipv6_group_idx__unspecified_addr(void)
{
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_group_idx(netifs[0],
                                                        &ipv6_addr_unspecified));
}

static void test_ipv6_group_idx__unicast_addr(void)
{
    static const ipv6_addr_t addr = { .u8 = NETIF0_IPV6_G };

    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_group_idx(netifs[0],
                                                        &addr));
}

static void test_ipv6_group_idx__wrong_netif(void)
{
    test_ipv6_group_join__success();
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_group_idx(netifs[1],
                                                        &ipv6_addr_all_nodes_link_local));
}

static void test_ipv6_group_idx__wrong_addr(void)
{
    test_ipv6_group_join__success();
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_ipv6_group_idx(netifs[0],
                                                        &ipv6_addr_all_routers_link_local));
}

static void test_ipv6_group_idx__success(void)
{
    test_ipv6_group_join__success();
    TEST_ASSERT(0 <= gnrc_netif_ipv6_group_idx(netifs[0], &ipv6_addr_all_nodes_link_local));
}

static void test_ipv6_get_iid(void)
{
    static const ipv6_addr_t ethernet_ipv6_ll = { .u8 = ETHERNET_IPV6_LL };
    static const ipv6_addr_t ieee802154_ipv6_ll_long = { .u8 = IEEE802154_IPV6_LL };
    static const uint8_t ieee802154_eui64_short[] = { 0, 0, 0, 0xff,
                                                      0xfe, 0, LA7, LA8 };
    eui64_t res;
    uint16_t ieee802154_l2addr_len = 2U;

    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          gnrc_netif_ipv6_get_iid(ethernet_netif, &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&res, &ethernet_ipv6_ll.u64[1],
                                    sizeof(res)));
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          gnrc_netif_ipv6_get_iid(ieee802154_netif, &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&res, &ieee802154_ipv6_ll_long.u64[1],
                                    sizeof(res)));
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_len),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_SRC_LEN, 0,
                                          &ieee802154_l2addr_len,
                                          sizeof(ieee802154_l2addr_len)));
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          gnrc_netif_ipv6_get_iid(ieee802154_netif, &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&res, &ieee802154_eui64_short, sizeof(res)));
    /* reset to source length 8 */
    ieee802154_l2addr_len = 8U;
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_len),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_SRC_LEN, 0,
                                          &ieee802154_l2addr_len,
                                          sizeof(ieee802154_l2addr_len)));
    for (unsigned i = 0; i < DEFAULT_DEVS_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(-ENOTSUP, gnrc_netif_ipv6_get_iid(netifs[i], &res));
    }
}

static void test_netapi_get__HOP_LIMIT(void)
{
    uint8_t value;

    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_get(netifs[0]->pid, NETOPT_HOP_LIMIT,
                                          0, &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(netifs[0]->cur_hl, value);
}

static void test_netapi_get__IPV6_ADDR(void)
{
    static const ipv6_addr_t exp = { NETIF0_IPV6_LL };
    ipv6_addr_t value[GNRC_NETIF_IPV6_ADDRS_NUMOF];

    test_ipv6_addr_add__success();
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t), gnrc_netapi_get(netifs[0]->pid,
                                                               NETOPT_IPV6_ADDR,
                                                               0, &value,
                                                               sizeof(value)));
    TEST_ASSERT(ipv6_addr_equal(&exp, &value[0]));
}

static void test_netapi_get__IPV6_ADDR_FLAGS(void)
{
    uint8_t value[GNRC_NETIF_IPV6_ADDRS_NUMOF];

    test_ipv6_addr_add__success();
    TEST_ASSERT_EQUAL_INT(sizeof(uint8_t), gnrc_netapi_get(netifs[0]->pid,
                                                           NETOPT_IPV6_ADDR_FLAGS,
                                                           0, &value,
                                                           sizeof(value)));
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID,
                          value[0]);
}

static void test_netapi_get__IPV6_GROUP(void)
{
    ipv6_addr_t value[GNRC_NETIF_IPV6_GROUPS_NUMOF];

    test_ipv6_group_join__success();
    TEST_ASSERT_EQUAL_INT(sizeof(ipv6_addr_t), gnrc_netapi_get(netifs[0]->pid,
                                                               NETOPT_IPV6_GROUP,
                                                               0, &value,
                                                               sizeof(value)));
    TEST_ASSERT(ipv6_addr_equal(&ipv6_addr_all_nodes_link_local, &value[0]));
}

static void test_netapi_get__IPV6_IID(void)
{
    static const ipv6_addr_t ethernet_ipv6_ll = { .u8 = ETHERNET_IPV6_LL };
    static const ipv6_addr_t ieee802154_ipv6_ll_long = { .u8 = IEEE802154_IPV6_LL };
    static const uint8_t ieee802154_eui64_short[] = { 0, 0, 0, 0xff,
                                                      0xfe, 0, LA7, LA8 };
    eui64_t value;
    uint16_t ieee802154_l2addr_len = 2U;

    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t), gnrc_netapi_get(ethernet_netif->pid,
                                                           NETOPT_IPV6_IID,
                                                           0, &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&value, &ethernet_ipv6_ll.u64[1],
                                    sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t), gnrc_netapi_get(ieee802154_netif->pid,
                                                           NETOPT_IPV6_IID,
                                                           0, &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&value, &ieee802154_ipv6_ll_long.u64[1],
                                    sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_len),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_SRC_LEN, 0,
                                          &ieee802154_l2addr_len,
                                          sizeof(ieee802154_l2addr_len)));
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t), gnrc_netapi_get(ieee802154_netif->pid,
                                                           NETOPT_IPV6_IID,
                                                           0, &value,
                                                           sizeof(value)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&value, &ieee802154_eui64_short,
                                    sizeof(value)));
    /* reset to source length 8 */
    ieee802154_l2addr_len = 8U;
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_len),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_SRC_LEN, 0,
                                          &ieee802154_l2addr_len,
                                          sizeof(ieee802154_l2addr_len)));
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, gnrc_netapi_get(netifs[0]->pid,
                                                    NETOPT_IPV6_IID,
                                                    0, &value, sizeof(value)));
}

static void test_netapi_get__MAX_PACKET_SIZE(void)
{
    uint16_t value;

    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), gnrc_netapi_get(ethernet_netif->pid,
                                                            NETOPT_MAX_PDU_SIZE,
                                                            GNRC_NETTYPE_IPV6,
                                                            &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(ETHERNET_DATA_LEN, value);
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), gnrc_netapi_get(ethernet_netif->pid,
                                                            NETOPT_MAX_PDU_SIZE,
                                                            GNRC_NETTYPE_NETIF,
                                                            &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(ETHERNET_DATA_LEN, value);
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), gnrc_netapi_get(ieee802154_netif->pid,
                                                            NETOPT_MAX_PDU_SIZE,
                                                            GNRC_NETTYPE_IPV6,
                                                            &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(IPV6_MIN_MTU, value);
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), gnrc_netapi_get(ieee802154_netif->pid,
                                                            NETOPT_MAX_PDU_SIZE,
                                                            GNRC_NETTYPE_NETIF,
                                                            &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(TEST_IEEE802154_MAX_FRAG_SIZE, value);
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), gnrc_netapi_get(netifs[0]->pid,
                                                            NETOPT_MAX_PDU_SIZE,
                                                            GNRC_NETTYPE_IPV6,
                                                            &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(IPV6_MIN_MTU, value);
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), gnrc_netapi_get(netifs[0]->pid,
                                                            NETOPT_MAX_PDU_SIZE,
                                                            GNRC_NETTYPE_NETIF,
                                                            &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(IPV6_MIN_MTU, value);
}

static void test_netapi_get__6LO_IPHC(void)
{
    netopt_enable_t value;

    TEST_ASSERT_EQUAL_INT(sizeof(netopt_enable_t),
                          gnrc_netapi_get(ethernet_netif->pid,
                                          NETOPT_6LO_IPHC, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(NETOPT_DISABLE, value);
    TEST_ASSERT_EQUAL_INT(sizeof(netopt_enable_t),
                          gnrc_netapi_get(ieee802154_netif->pid,
                                          NETOPT_6LO_IPHC, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(NETOPT_ENABLE, value);
    TEST_ASSERT_EQUAL_INT(sizeof(netopt_enable_t),
                          gnrc_netapi_get(netifs[0]->pid,
                                          NETOPT_6LO_IPHC, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(NETOPT_DISABLE, value);
}

static void test_netapi_get__ADDRESS(void)
{
    static const uint8_t exp_ethernet[] = ETHERNET_SRC;
    static const uint8_t exp_ieee802154[] = IEEE802154_SHORT_SRC;
    uint8_t value[GNRC_NETIF_L2ADDR_MAXLEN];

    TEST_ASSERT_EQUAL_INT(sizeof(exp_ethernet),
                          gnrc_netapi_get(ethernet_netif->pid,
                                          NETOPT_ADDRESS, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_ethernet, value, sizeof(exp_ethernet)));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ieee802154),
                          gnrc_netapi_get(ieee802154_netif->pid,
                                          NETOPT_ADDRESS, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_ieee802154, value,
                                    sizeof(exp_ieee802154)));
}

static void test_netapi_get__ADDRESS_LONG(void)
{
    static const uint8_t exp_ieee802154[] = IEEE802154_LONG_SRC;
    uint8_t value[GNRC_NETIF_L2ADDR_MAXLEN];

    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          gnrc_netapi_get(ethernet_netif->pid,
                                          NETOPT_ADDRESS_LONG, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ieee802154),
                          gnrc_netapi_get(ieee802154_netif->pid,
                                          NETOPT_ADDRESS_LONG, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_ieee802154, value,
                                    sizeof(exp_ieee802154)));
    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          gnrc_netapi_get(netifs[0]->pid,
                                          NETOPT_ADDRESS_LONG, 0,
                                          &value, sizeof(value)));
}

static void test_netapi_set__HOP_LIMIT(void)
{
    uint8_t value = 89;

    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_HOP_LIMIT, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(value, netifs[0]->cur_hl);
}

static void test_netapi_set__IPV6_ADDR(void)
{
    ipv6_addr_t value = { .u8 = NETIF0_IPV6_LL };
    static const uint16_t context = (64U << 8) |
                                    (GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);

    TEST_ASSERT(0 > gnrc_netif_ipv6_addr_idx(netifs[0], &value));
    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_IPV6_ADDR, context,
                                          &value, sizeof(value)));
    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_idx(netifs[0], &value));
}

static void test_netapi_set__IPV6_ADDR_REMOVE(void)
{
    ipv6_addr_t value = { .u8 = NETIF0_IPV6_LL };

    test_ipv6_addr_add__success();
    TEST_ASSERT(0 <= gnrc_netif_ipv6_addr_idx(netifs[0], &value));
    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_IPV6_ADDR_REMOVE, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT(0 > gnrc_netif_ipv6_addr_idx(netifs[0], &value));
}

static void test_netapi_set__IPV6_GROUP(void)
{
    ipv6_addr_t value = IPV6_ADDR_ALL_NODES_LINK_LOCAL;

    TEST_ASSERT(0 > gnrc_netif_ipv6_group_idx(netifs[0], &value));
    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_IPV6_GROUP, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT(0 <= gnrc_netif_ipv6_group_idx(netifs[0], &value));
}

static void test_netapi_set__IPV6_GROUP_LEAVE(void)
{
    ipv6_addr_t value = IPV6_ADDR_ALL_NODES_LINK_LOCAL;

    test_ipv6_group_join__success();
    TEST_ASSERT(0 <= gnrc_netif_ipv6_group_idx(netifs[0], &value));
    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_IPV6_GROUP_LEAVE, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT(0 > gnrc_netif_ipv6_group_idx(netifs[0], &value));
}

static void test_netapi_set__MAX_PACKET_SIZE(void)
{
    uint16_t value = 57194;

    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_MAX_PDU_SIZE,
                                          GNRC_NETTYPE_IPV6,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(value, netifs[0]->ipv6.mtu);
    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_MAX_PDU_SIZE, 0,
                                          &value, sizeof(value)));
}

static void test_netapi_set__6LO_IPHC(void)
{
    netopt_enable_t value = NETOPT_ENABLE;

    TEST_ASSERT_EQUAL_INT(sizeof(value),
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_6LO_IPHC, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT(netifs[0]->flags & GNRC_NETIF_FLAGS_6LO_HC);
}

static void test_netapi_set__ADDRESS(void)
{
    static const uint8_t exp_ethernet[] = ETHERNET_SRC;
    static const uint8_t exp_ieee802154[] = IEEE802154_SHORT_SRC;
    static const uint8_t exp_ieee802154_long[] = IEEE802154_LONG_SRC;
    uint8_t value[] = { LA1 + 1, LA2 + 2, LA3 + 3, LA4 + 4, LA5 + 5, LA6 + 6 };

    TEST_ASSERT_EQUAL_INT(sizeof(exp_ethernet),
                          gnrc_netapi_set(ethernet_netif->pid,
                                          NETOPT_ADDRESS, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(value), ethernet_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(value, ethernet_netif->l2addr,
                                    ETHERNET_ADDR_LEN));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ieee802154),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_ADDRESS, 0,
                                          &value, sizeof(uint16_t)));
    /* we did not change NETOPT_SRC_LEN, so this field shouldn't change */
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ieee802154_long), ieee802154_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_ieee802154_long,
                                    ieee802154_netif->l2addr,
                                    sizeof(exp_ieee802154_long)));
    /* return addresses to previous state for further testing */
    memcpy(value, exp_ethernet, sizeof(exp_ethernet));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ethernet),
                          gnrc_netapi_set(ethernet_netif->pid,
                                          NETOPT_ADDRESS, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(value), ethernet_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(value, ethernet_netif->l2addr,
                                    sizeof(value)));
    memcpy(value, exp_ieee802154, sizeof(exp_ieee802154));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ieee802154),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_ADDRESS, 0,
                                          &value, sizeof(uint16_t)));
}

static void test_netapi_set__ADDRESS_LONG(void)
{
    static const uint8_t exp_ieee802154[] = IEEE802154_LONG_SRC;
    uint8_t value[] = { LA1 + 1, LA2 + 2, LA3 + 3, LA4 + 4, LA5 + 5, LA6 + 6,
                        LA7 + 1, LA8 + 2 };

    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          gnrc_netapi_set(ethernet_netif->pid,
                                          NETOPT_ADDRESS_LONG, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ieee802154),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_ADDRESS_LONG, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(value), ieee802154_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(value, ieee802154_netif->l2addr,
                                    sizeof(value)));
    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          gnrc_netapi_set(netifs[0]->pid,
                                          NETOPT_ADDRESS_LONG, 0,
                                          &value, sizeof(value)));
    /* return addresses to previous state for further testing */
    memcpy(value, exp_ieee802154, sizeof(exp_ieee802154));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ieee802154),
                          gnrc_netapi_set(ieee802154_netif->pid,
                                          NETOPT_ADDRESS_LONG, 0,
                                          &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(value), ieee802154_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(value, ieee802154_netif->l2addr,
                                    sizeof(value)));
}

static void test_netapi_set__SRC_LEN(void)
{
    static const uint8_t exp_l2addr[] = { LA7, LA8 };
    static const uint8_t orig_ieee802154[] = IEEE802154_LONG_SRC;
    uint16_t value = 2U;

    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          gnrc_netapi_set(ethernet_netif->pid, NETOPT_SRC_LEN,
                                          0, &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(value, ieee802154_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_l2addr, ieee802154_netif->l2addr,
                                    sizeof(exp_l2addr)));
    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          gnrc_netapi_set(netifs[0]->pid, NETOPT_SRC_LEN, 0,
                                          &value, sizeof(value)));
    /* return addresses to previous state for further testing */
    value = 8U;
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(value, ieee802154_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(orig_ieee802154, ieee802154_netif->l2addr,
                                    sizeof(orig_ieee802154)));
}

static void test_netif_iter(void)
{
    netif_t *netif = NULL;
    int netif_count = 0;

    while ((netif = netif_iter(netif)) != NULL) {
        netif_count++;
    }
    TEST_ASSERT_EQUAL_INT(gnrc_netif_numof(), netif_count);
}

static void test_netif_get_name(void)
{
    char exp_name[NETIF_NAMELENMAX + 1];
    char name[NETIF_NAMELENMAX];
    int res;
    netif_t *netif = netif_iter(NULL);
    /* there must be at least one interface */
    TEST_ASSERT_NOT_NULL(netif);

    res = netif_get_name(netif, name);
    sprintf(exp_name, "if%d", (int) ((gnrc_netif_t *)netif)->pid);
    TEST_ASSERT_EQUAL_INT(strlen(exp_name), res);
    TEST_ASSERT_EQUAL_STRING(&exp_name[0], &name[0]);
}

static void test_netif_get_by_name(void)
{
    char name[NETIF_NAMELENMAX] = "6nPRK28";
    netif_t *netif = netif_iter(NULL);

    TEST_ASSERT(netif_get_by_name(name) == NULL);
    /* there must be at least one interface */
    TEST_ASSERT_NOT_NULL(netif);
    TEST_ASSERT(netif_get_name(netif, name) > 0);
    TEST_ASSERT(netif == netif_get_by_name(name));
}

static void test_netif_get_opt(void)
{
    /* just repeat one of the gnrc_netapi_get tests, just with netif_get_opt */
    static const uint8_t exp_ethernet[] = ETHERNET_SRC;
    uint8_t value[GNRC_NETIF_L2ADDR_MAXLEN];

    TEST_ASSERT_EQUAL_INT(sizeof(exp_ethernet),
                          netif_get_opt((netif_t *)ethernet_netif,
                                        NETOPT_ADDRESS, 0,
                                        &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_ethernet, value, sizeof(exp_ethernet)));
}

static void test_netif_set_opt(void)
{
    /* just repeat one of the gnrc_netapi_set tests, just with netif_set_opt */
    static const uint8_t exp_ethernet[] = ETHERNET_SRC;
    uint8_t value[] = { LA1 + 1, LA2 + 2, LA3 + 3, LA4 + 4, LA5 + 5, LA6 + 6 };

    TEST_ASSERT_EQUAL_INT(sizeof(exp_ethernet),
                          netif_set_opt((netif_t *)ethernet_netif,
                                        NETOPT_ADDRESS, 0,
                                        &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(value), ethernet_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(value, ethernet_netif->l2addr,
                                    ETHERNET_ADDR_LEN));
    /* return addresses to previous state for further testing */
    memcpy(value, exp_ethernet, sizeof(exp_ethernet));
    TEST_ASSERT_EQUAL_INT(sizeof(exp_ethernet),
                          netif_set_opt((netif_t *)ethernet_netif,
                                        NETOPT_ADDRESS, 0,
                                        &value, sizeof(value)));
    TEST_ASSERT_EQUAL_INT(sizeof(value), ethernet_netif->l2addr_len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(value, ethernet_netif->l2addr,
                                    sizeof(value)));
}

static void test_netapi_send__raw_unicast_ethernet_packet(void)
{
    uint8_t dst[] = { LA1, LA2, LA3, LA4, LA5, LA6 + 1 };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "ABCDEFG",
                                          sizeof("ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, dst, sizeof(dst));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ethernet_netif->pid, pkt);
}

static void test_netapi_send__raw_broadcast_ethernet_packet(void)
{
    gnrc_netif_hdr_t *hdr;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "ABCDEFG",
                                          sizeof("ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    TEST_ASSERT_NOT_NULL(netif);
    hdr = netif->data;
    hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ethernet_netif->pid, pkt);
}

static void test_netapi_send__raw_unicast_ieee802154_long_long_packet(void)
{
    uint8_t dst[] = { LA1, LA2, LA3, LA4, LA5, LA6, LA7, LA8 + 1 };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "123ABCDEFG",
                                          sizeof("123ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, dst, sizeof(dst));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
}

static void test_netapi_send__raw_unicast_ieee802154_long_short_packet(void)
{
    uint8_t dst[] = { LA7, LA8 + 1 };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "123ABCDEFG",
                                          sizeof("123ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, dst, sizeof(dst));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
}

static void test_netapi_send__raw_unicast_ieee802154_short_long_packet1(void)
{
    uint8_t dst[] = { LA1, LA2, LA3, LA4, LA5, LA6, LA7, LA8 + 1 };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "123ABCDEFG",
                                          sizeof("123ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    uint16_t src_len = 2U;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(src_len),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &src_len, sizeof(src_len)));
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, dst, sizeof(dst));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
    /* reset src_len */
    src_len = 8U;
    TEST_ASSERT_EQUAL_INT(sizeof(src_len),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &src_len, sizeof(src_len)));
}

static void test_netapi_send__raw_unicast_ieee802154_short_long_packet2(void)
{
    uint8_t src[] = { LA7, LA8 };
    uint8_t dst[] = { LA1, LA2, LA3, LA4, LA5, LA6, LA7, LA8 + 1 };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "123ABCDEFG",
                                          sizeof("123ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(src, sizeof(src),
                                                 dst, sizeof(dst));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
}

static void test_netapi_send__raw_unicast_ieee802154_short_short_packet(void)
{
    uint8_t dst[] = { LA7, LA8 + 1 };
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "123ABCDEFG",
                                          sizeof("123ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    uint16_t src_len = 2U;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(src_len),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &src_len, sizeof(src_len)));
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, dst, sizeof(dst));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
    /* reset src_len */
    src_len = 8U;
    TEST_ASSERT_EQUAL_INT(sizeof(src_len),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &src_len, sizeof(src_len)));
}

static void test_netapi_send__raw_broadcast_ieee802154_long_packet(void)
{
    gnrc_netif_hdr_t *hdr;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "123ABCDEFG",
                                          sizeof("123ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(pkt);
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    TEST_ASSERT_NOT_NULL(netif);
    hdr = netif->data;
    hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
}

static void test_netapi_send__raw_broadcast_ieee802154_short_packet(void)
{
    gnrc_netif_hdr_t *hdr;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, "123ABCDEFG",
                                          sizeof("123ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    uint16_t src_len = 2U;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(sizeof(src_len),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &src_len, sizeof(src_len)));
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    TEST_ASSERT_NOT_NULL(netif);
    hdr = netif->data;
    hdr->flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
    /* reset src_len */
    src_len = 8U;
    TEST_ASSERT_EQUAL_INT(sizeof(src_len),
                          gnrc_netapi_set(ieee802154_netif->pid, NETOPT_SRC_LEN,
                                          0, &src_len, sizeof(src_len)));
}

static void test_netapi_send__ipv6_unicast_ethernet_packet(void)
{
    ipv6_hdr_t *ipv6_hdr;
    uint8_t dst_netif[] = { LA1, LA2, LA3, LA4, LA5, LA6 + 1 };
    static const ipv6_addr_t dst_ipv6 = { .u8 = { LP1, LP2, LP3, LP4,
                                                  LP5, LP6, LP7, LP8,
                                                  LA1 ^ 0x2, LA2, LA3, 0xff,
                                                  0xfe, LA4, LA5, LA6 + 1} };
    static const ipv6_addr_t src_ipv6 = { .u8 = ETHERNET_IPV6_LL };
    gnrc_pktsnip_t *payload = gnrc_pktbuf_add(NULL, "ABCDEFG",
                                          sizeof("ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(payload);
    /* we don't send through gnrc_ipv6 (because we are lazy and don't want
     * to update the neighbor cache ;-)) so we need to set the IPv6 source
     * address */
    gnrc_pktsnip_t *pkt = gnrc_ipv6_hdr_build(payload, &src_ipv6, &dst_ipv6);
    TEST_ASSERT_NOT_NULL(pkt);
    ipv6_hdr = pkt->data;
    ipv6_hdr->len = byteorder_htons(sizeof("ABCDEFG"));
    ipv6_hdr->nh = PROTNUM_IPV6_NONXT;
    ipv6_hdr->hl = ethernet_netif->cur_hl;
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, dst_netif,
                                                 sizeof(dst_netif));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ethernet_netif->pid, pkt);
}

static void test_netapi_send__ipv6_multicast_ethernet_packet(void)
{
    ipv6_hdr_t *ipv6_hdr;
    gnrc_netif_hdr_t *netif_hdr;
    static const ipv6_addr_t src_ipv6 = { .u8 = ETHERNET_IPV6_LL };
    gnrc_pktsnip_t *payload = gnrc_pktbuf_add(NULL, "ABCDEFG",
                                          sizeof("ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(payload);
    /* we don't send through gnrc_ipv6 (because we are lazy and don't want
     * to update the neighbor cache ;-)) so we need to set the IPv6 source
     * address */
    gnrc_pktsnip_t *pkt = gnrc_ipv6_hdr_build(payload, &src_ipv6,
                                              &ipv6_addr_all_nodes_link_local);
    TEST_ASSERT_NOT_NULL(pkt);
    ipv6_hdr = pkt->data;
    ipv6_hdr->len = byteorder_htons(sizeof("ABCDEFG"));
    ipv6_hdr->nh = PROTNUM_IPV6_NONXT;
    ipv6_hdr->hl = ethernet_netif->cur_hl;
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    TEST_ASSERT_NOT_NULL(netif);
    netif_hdr = netif->data;
    netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_MULTICAST;
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ethernet_netif->pid, pkt);
}

static void test_netapi_send__ipv6_unicast_ieee802154_packet(void)
{
    ipv6_hdr_t *ipv6_hdr;
    uint8_t dst_netif[] = { LA1, LA2, LA3, LA4, LA5, LA6, LA7, LA8 + 1 };
    static const ipv6_addr_t dst_ipv6 = { .u8 = { LP1, LP2, LP3, LP4,
                                                  LP5, LP6, LP7, LP8,
                                                  LA1 ^ 0x2, LA2, LA3, 0xff,
                                                  0xfe, LA4, LA5, LA6 + 1} };
    static const ipv6_addr_t src_ipv6 = { .u8 = IEEE802154_IPV6_LL };
    gnrc_pktsnip_t *payload = gnrc_pktbuf_add(NULL, "ABCDEFG",
                                          sizeof("ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(payload);
    /* we don't send through gnrc_ipv6 (because we are lazy and don't want
     * to update the neighbor cache ;-)) so we need to set the IPv6 source
     * address */
    gnrc_pktsnip_t *pkt = gnrc_ipv6_hdr_build(payload, &src_ipv6, &dst_ipv6);
    TEST_ASSERT_NOT_NULL(pkt);
    ipv6_hdr = pkt->data;
    ipv6_hdr->len = byteorder_htons(sizeof("ABCDEFG"));
    ipv6_hdr->nh = PROTNUM_IPV6_NONXT;
    ipv6_hdr->hl = ieee802154_netif->cur_hl;
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, dst_netif,
                                                 sizeof(dst_netif));
    TEST_ASSERT_NOT_NULL(netif);
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
}

static void test_netapi_send__ipv6_multicast_ieee802154_packet(void)
{
    ipv6_hdr_t *ipv6_hdr;
    gnrc_netif_hdr_t *netif_hdr;
    static const ipv6_addr_t src_ipv6 = { .u8 = IEEE802154_IPV6_LL };
    gnrc_pktsnip_t *payload = gnrc_pktbuf_add(NULL, "ABCDEFG",
                                          sizeof("ABCDEFG"),
                                          GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(payload);
    /* we don't send through gnrc_ipv6 (because we are lazy and don't want
     * to update the neighbor cache ;-)) so we need to set the IPv6 source
     * address */
    gnrc_pktsnip_t *pkt = gnrc_ipv6_hdr_build(payload, &src_ipv6,
                                              &ipv6_addr_all_nodes_link_local);
    TEST_ASSERT_NOT_NULL(pkt);
    ipv6_hdr = pkt->data;
    ipv6_hdr->len = byteorder_htons(sizeof("ABCDEFG"));
    ipv6_hdr->nh = PROTNUM_IPV6_NONXT;
    ipv6_hdr->hl = ieee802154_netif->cur_hl;
    gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    TEST_ASSERT_NOT_NULL(netif);
    netif_hdr = netif->data;
    netif_hdr->flags |= GNRC_NETIF_HDR_FLAGS_MULTICAST;
    LL_PREPEND(pkt, netif);
    gnrc_netapi_send(ieee802154_netif->pid, pkt);
}

static void test_netapi_recv__empty_ethernet_payload(void)
{
    static const uint8_t data[] = { LA1, LA2, LA3, LA6, LA7, LA8,
                                    LA1, LA2, LA3, LA6, LA7, LA8 + 1,
                                    0xff, 0xff };

    puts("pktdump dumping Ethernet packet with empty payload");
    _test_trigger_recv(ethernet_netif, data, sizeof(data));
}

static void test_netapi_recv__empty_ieee802154_payload(void)
{
    static const uint8_t data[] = { 0x41, 0xdc, /* FCF */
                                    0x03,       /* Sequence number */
                                    0x00, 0x00, /* Destination PAN */
                                    LA8, LA7, LA6, LA5, LA4, LA3, LA2, LA1,
                                    LA8 + 1, LA7, LA6, LA5, LA4, LA3, LA2,
                                    LA1 };

    puts("pktdump dumping IEEE 802.15.4 packet with empty payload");
    _test_trigger_recv(ieee802154_netif, data, sizeof(data));
}

static void test_netapi_recv__raw_ethernet_payload(void)
{
    static const uint8_t data[] = { LA1, LA2, LA3, LA6, LA7, LA8,
                                    LA1, LA2, LA3, LA6, LA7, LA8 + 1,
                                    0xff, 0xff, 0x12, 0x34, 0x45, 0x56 };

    puts("pktdump dumping Ethernet packet with payload 12 34 45 56");
    _test_trigger_recv(ethernet_netif, data, sizeof(data));
}

static void test_netapi_recv__raw_ieee802154_payload(void)
{
    static const uint8_t data[] = { 0x41, 0xdc, /* FCF */
                                    0x03,       /* Sequence number */
                                    0x00, 0x00, /* Destination PAN */
                                    LA8, LA7, LA6, LA5, LA4, LA3, LA2, LA1,
                                    LA8 + 1, LA7, LA6, LA5, LA4, LA3, LA2,
                                    LA1, 0x12, 0x34, 0x45, 0x56 };

    puts("pktdump dumping IEEE 802.15.4 packet with payload 12 34 45 56");
    _test_trigger_recv(ieee802154_netif, data, sizeof(data));
}

static void test_netapi_recv__ipv6_ethernet_payload(void)
{
    static const uint8_t data[] = { LA1, LA2, LA3, LA6, LA7, LA8,
                                    LA1, LA2, LA3, LA6, LA7, LA8 + 1,
                                    0x86, 0xdd, /* Ethertype: IPv6 */
                                    0x60, 0, 0, 0,  /* Version + TC + FL */
                                    0, 1, /* payload length 1 */
                                    59, /* next header: no next header */
                                    64, /* hop limit */
                                    /* IPv6 source */
                                    LP1, LP2, LP3, LP4, LP5, LP6, LP7, LP8,
                                    LA1 ^ 1, LA2, LA3, 0xff, 0xfe, LA6, LA7, LA8,
                                    /* IPv6 destination */
                                    LP1, LP2, LP3, LP4, LP5, LP6, LP7, LP8,
                                    LA1 ^ 1, LA2, LA3, 0xff, 0xfe, LA6, LA7, LA8 + 1,
                                    0x01 /* payload */
                                  };

    puts("pktdump dumping IPv6 over Ethernet packet with payload 01");
    _test_trigger_recv(ethernet_netif, data, sizeof(data));
}

static Test *embunit_tests_gnrc_netif(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_creation),
        new_TestFixture(test_get_by_pid),
        new_TestFixture(test_addr_to_str),
        new_TestFixture(test_addr_from_str),
        new_TestFixture(test_ipv6_addr_add__ENOMEM),
        new_TestFixture(test_ipv6_addr_add__success),
        new_TestFixture(test_ipv6_addr_add__readd_with_free_entry),
        new_TestFixture(test_ipv6_addr_remove__not_allocated),
        new_TestFixture(test_ipv6_addr_remove__success),
        new_TestFixture(test_ipv6_addr_idx__empty),
        new_TestFixture(test_ipv6_addr_idx__unspecified_addr),
        new_TestFixture(test_ipv6_addr_idx__wrong_netif),
        new_TestFixture(test_ipv6_addr_idx__wrong_addr),
        new_TestFixture(test_ipv6_addr_idx__success),
        new_TestFixture(test_ipv6_addr_match__empty),
        new_TestFixture(test_ipv6_addr_match__unspecified_addr),
        new_TestFixture(test_ipv6_addr_match__wrong_netif),
        new_TestFixture(test_ipv6_addr_match__wrong_addr),
        new_TestFixture(test_ipv6_addr_match__success18),
        new_TestFixture(test_ipv6_addr_match__success23),
        new_TestFixture(test_ipv6_addr_match__success64),
        new_TestFixture(test_ipv6_addr_best_src__multicast_input),
        new_TestFixture(test_ipv6_addr_best_src__unspecified_addr),
        new_TestFixture(test_ipv6_addr_best_src__other_subnet),
        new_TestFixture(test_ipv6_addr_best_src__ula_src_dst),
        new_TestFixture(test_ipv6_addr_best_src__global_src_ula_dst),
        new_TestFixture(test_ipv6_addr_best_src__deprecated_addr),
        new_TestFixture(test_get_by_ipv6_addr__empty),
        new_TestFixture(test_get_by_ipv6_addr__unspecified_addr),
        new_TestFixture(test_get_by_ipv6_addr__success),
        new_TestFixture(test_get_by_prefix__empty),
        new_TestFixture(test_get_by_prefix__unspecified_addr),
        new_TestFixture(test_get_by_prefix__success18),
        new_TestFixture(test_get_by_prefix__success23),
        new_TestFixture(test_get_by_prefix__success64),
        new_TestFixture(test_ipv6_group_join__ENOMEM),
        new_TestFixture(test_ipv6_group_join__success),
        new_TestFixture(test_ipv6_group_join__readd_with_free_entry),
        new_TestFixture(test_ipv6_group_leave__not_allocated),
        new_TestFixture(test_ipv6_group_leave__success),
        new_TestFixture(test_ipv6_group_idx__empty),
        new_TestFixture(test_ipv6_group_idx__unspecified_addr),
        new_TestFixture(test_ipv6_group_idx__unicast_addr),
        new_TestFixture(test_ipv6_group_idx__wrong_netif),
        new_TestFixture(test_ipv6_group_idx__wrong_addr),
        new_TestFixture(test_ipv6_group_idx__success),
        new_TestFixture(test_ipv6_get_iid),
        new_TestFixture(test_netapi_get__HOP_LIMIT),
        new_TestFixture(test_netapi_get__IPV6_ADDR),
        new_TestFixture(test_netapi_get__IPV6_ADDR_FLAGS),
        new_TestFixture(test_netapi_get__IPV6_GROUP),
        new_TestFixture(test_netapi_get__IPV6_IID),
        new_TestFixture(test_netapi_get__MAX_PACKET_SIZE),
        new_TestFixture(test_netapi_get__6LO_IPHC),
        new_TestFixture(test_netapi_get__ADDRESS),
        new_TestFixture(test_netapi_get__ADDRESS_LONG),
        new_TestFixture(test_netapi_set__HOP_LIMIT),
        new_TestFixture(test_netapi_set__IPV6_ADDR),
        new_TestFixture(test_netapi_set__IPV6_ADDR_REMOVE),
        new_TestFixture(test_netapi_set__IPV6_GROUP),
        new_TestFixture(test_netapi_set__IPV6_GROUP_LEAVE),
        new_TestFixture(test_netapi_set__MAX_PACKET_SIZE),
        new_TestFixture(test_netapi_set__6LO_IPHC),
        new_TestFixture(test_netapi_set__ADDRESS),
        new_TestFixture(test_netapi_set__ADDRESS_LONG),
        new_TestFixture(test_netapi_set__SRC_LEN),
        new_TestFixture(test_netif_iter),
        new_TestFixture(test_netif_get_name),
        new_TestFixture(test_netif_get_by_name),
        new_TestFixture(test_netif_get_opt),
        new_TestFixture(test_netif_set_opt),
        /* only add tests not involving output here */
    };
    EMB_UNIT_TESTCALLER(tests, _set_up, NULL, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    _tests_init();
    netdev_test_set_get_cb((netdev_test_t *)ethernet_dev, NETOPT_ADDRESS,
                           _get_netdev_address);
    netdev_test_set_set_cb((netdev_test_t *)ethernet_dev, NETOPT_ADDRESS,
                           _set_netdev_address);
    netdev_test_set_get_cb((netdev_test_t *)ieee802154_dev, NETOPT_ADDRESS,
                           _get_netdev_address);
    netdev_test_set_set_cb((netdev_test_t *)ieee802154_dev, NETOPT_ADDRESS,
                           _set_netdev_address);
    netdev_test_set_get_cb((netdev_test_t *)ieee802154_dev, NETOPT_ADDRESS_LONG,
                           _get_netdev_address_long);
    netdev_test_set_set_cb((netdev_test_t *)ieee802154_dev, NETOPT_ADDRESS_LONG,
                           _set_netdev_address_long);
    netdev_test_set_get_cb((netdev_test_t *)ieee802154_dev, NETOPT_SRC_LEN,
                           _get_netdev_src_len);
    netdev_test_set_set_cb((netdev_test_t *)ieee802154_dev, NETOPT_SRC_LEN,
                           _set_netdev_src_len);
    TESTS_START();
    TESTS_RUN(embunit_tests_gnrc_netif());
    TESTS_END();
    /* add netapi send and receive tests here */
    test_netapi_send__raw_unicast_ethernet_packet();
    test_netapi_send__raw_broadcast_ethernet_packet();
    test_netapi_send__raw_unicast_ieee802154_long_long_packet();
    test_netapi_send__raw_unicast_ieee802154_long_short_packet();
    test_netapi_send__raw_unicast_ieee802154_short_long_packet1();
    test_netapi_send__raw_unicast_ieee802154_short_long_packet2();
    test_netapi_send__raw_unicast_ieee802154_short_short_packet();
    test_netapi_send__raw_broadcast_ieee802154_long_packet();
    test_netapi_send__raw_broadcast_ieee802154_short_packet();
    test_netapi_send__ipv6_unicast_ethernet_packet();
    test_netapi_send__ipv6_multicast_ethernet_packet();
    test_netapi_send__ipv6_unicast_ieee802154_packet();
    test_netapi_send__ipv6_multicast_ieee802154_packet();
    test_netapi_recv__empty_ethernet_payload();
    test_netapi_recv__empty_ieee802154_payload();
    test_netapi_recv__raw_ethernet_payload();
    test_netapi_recv__raw_ieee802154_payload();
    test_netapi_recv__ipv6_ethernet_payload();
    return 0;
}

static inline int _mock_netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    (void)netif;
    (void)pkt;
    return -1;
}

static inline gnrc_pktsnip_t *_mock_netif_recv(gnrc_netif_t * netif)
{
    (void)netif;
    return NULL;
}

static uint8_t ethernet_l2addr[] = ETHERNET_SRC;
static uint8_t ieee802154_l2addr_long[] = IEEE802154_LONG_SRC;
static uint8_t ieee802154_l2addr_short[] = IEEE802154_SHORT_SRC;
static uint16_t ieee802154_l2addr_len = 8U;

static int _get_netdev_address(netdev_t *dev, void *value, size_t max_len)
{
    (void)max_len;

    if (dev == ethernet_dev) {
        assert(max_len >= sizeof(ethernet_l2addr));
        memcpy(value, ethernet_l2addr, sizeof(ethernet_l2addr));
        return sizeof(ethernet_l2addr);
    }
    else if (dev == ieee802154_dev) {
        assert(max_len >= sizeof(ieee802154_l2addr_short));
        memcpy(value, ieee802154_l2addr_short, sizeof(ieee802154_l2addr_short));
        return sizeof(ieee802154_l2addr_short);
    }
    return -ENOTSUP;
}

static int _set_netdev_address(netdev_t *dev, const void *value,
                               size_t value_len)
{
    if (dev == ethernet_dev) {
        assert(value_len <= sizeof(ethernet_l2addr));
        memcpy(ethernet_l2addr, value, value_len);
        return value_len;
    }
    else if (dev == ieee802154_dev) {
        assert(value_len <= sizeof(ieee802154_l2addr_short));
        memcpy(ieee802154_l2addr_short, value, value_len);
        return value_len;
    }
    return -ENOTSUP;
}

static int _get_netdev_address_long(netdev_t *dev, void *value, size_t max_len)
{
    (void)max_len;

    if (dev == ieee802154_dev) {
        assert(max_len >= sizeof(ieee802154_l2addr_long));
        memcpy(value, ieee802154_l2addr_long, sizeof(ieee802154_l2addr_long));
        return sizeof(ieee802154_l2addr_long);
    }
    return -ENOTSUP;
}

static int _set_netdev_address_long(netdev_t *dev, const void *value,
                                    size_t value_len)
{
    if (dev == ieee802154_dev) {
        assert(value_len <= sizeof(ieee802154_l2addr_long));
        memcpy(ieee802154_l2addr_long, value, value_len);
        return value_len;
    }
    return -ENOTSUP;
}

static int _get_netdev_src_len(netdev_t *dev, void *value, size_t max_len)
{
    (void)max_len;

    if (dev == ieee802154_dev) {
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)value) = ieee802154_l2addr_len;
        return sizeof(uint16_t);
    }
    return -ENOTSUP;
}

static int _set_netdev_src_len(netdev_t *dev, const void *value,
                               size_t value_len)
{
    (void)value_len;

    if (dev == ieee802154_dev) {
        assert(value_len == sizeof(uint16_t));
        ieee802154_l2addr_len = *((uint16_t *)value);
        return sizeof(uint16_t);
    }
    return -ENOTSUP;
}
