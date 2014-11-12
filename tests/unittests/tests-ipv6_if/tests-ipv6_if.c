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
 * @file    tests-ipv6_if.c
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "byteorder.h"
#include "kernel_types.h"
#include "ipv6/addr.h"
#include "ipv6/if.h"

#include "tests-ipv6_if.h"

#define TEST_MAC_PID    (KERNEL_PID_FIRST)

static int if_id = -1;

static void set_up(void)
{
    kernel_pid_t mac_pid = TEST_MAC_PID;

    ipv6_if_init();

    if_id = ipv6_if_init_if(mac_pid);
}

static void test_ipv6_if_init_if_EINVAL(void)
{
    kernel_pid_t mac_pid = KERNEL_PID_UNDEF;

    ipv6_if_init();

    TEST_ASSERT_EQUAL_INT(-EINVAL, ipv6_if_init_if(mac_pid));
}

static void test_ipv6_if_init_if_EISCONN(void)
{
    kernel_pid_t mac_pid = TEST_MAC_PID;

    ipv6_if_init();

    TEST_ASSERT(ipv6_if_init_if(mac_pid) >= 0);
    TEST_ASSERT_EQUAL_INT(-EISCONN, ipv6_if_init_if(mac_pid));
}

static void test_ipv6_if_init_if_ENOBUFS(void)
{
    kernel_pid_t mac_pid = TEST_MAC_PID;

    ipv6_if_init();

    for (if_id = 0; if_id < IPV6_IF_NUM; if_id++) {
        TEST_ASSERT(ipv6_if_init_if(mac_pid++) >= 0);
    }

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, ipv6_if_init_if(mac_pid));
}

static void test_ipv6_if_reset_if_and_test_if_initialized(void)
{
    ipv6_if_reset_if(if_id);

    TEST_ASSERT_EQUAL_INT(false, ipv6_if_initialized(if_id));
}

static void test_ipv6_if_initialized(void)
{
    TEST_ASSERT_EQUAL_INT(true, ipv6_if_initialized(if_id));
}

static void test_ipv6_if_get_by_id_uninitialized(void)
{
    TEST_ASSERT_NULL(ipv6_if_get_by_id(INT_MAX));
}

static void test_ipv6_if_get_by_id_negative(void)
{
    TEST_ASSERT_NULL(ipv6_if_get_by_id(INT_MIN));
}

static void test_ipv6_if_get_by_id(void)
{
    TEST_ASSERT_NOT_NULL(ipv6_if_get_by_id(if_id));
}

static void test_ipv6_if_get_id_by_mac_KERNEL_PID_UNDEF(void)
{
    TEST_ASSERT_EQUAL_INT(-ENOENT, ipv6_if_get_id_by_mac(KERNEL_PID_UNDEF));
}

static void test_ipv6_if_get_id_by_mac_KERNEL_PID_LAST(void)
{
    TEST_ASSERT_EQUAL_INT(-ENOENT, ipv6_if_get_id_by_mac(KERNEL_PID_LAST));
}

static void test_ipv6_if_get_id_by_mac(void)
{
    TEST_ASSERT_EQUAL_INT(if_id, ipv6_if_get_id_by_mac(TEST_MAC_PID));
}

static void test_ipv6_if_get_by_mac_KERNEL_PID_UNDEF(void)
{
    TEST_ASSERT_NULL(ipv6_if_get_by_mac(KERNEL_PID_UNDEF));
}

static void test_ipv6_if_get_by_mac_KERNEL_PID_LAST(void)
{
    TEST_ASSERT_NULL(ipv6_if_get_by_mac(KERNEL_PID_LAST));
}

static void test_ipv6_if_get_by_mac(void)
{
    TEST_ASSERT_NOT_NULL(ipv6_if_get_by_mac(TEST_MAC_PID));
}

static void test_ipv6_if_add_addr_wrong_interface_with_anycast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(-ENOENT, ipv6_if_add_addr(INT_MAX, &addr, true));
}

static void test_ipv6_if_add_addr_wrong_interface_without_anycast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(-ENOENT, ipv6_if_add_addr(INT_MAX, &addr, false));
}

static void test_ipv6_if_add_unspecified_addr_with_anycast(void)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ipv6_if_add_addr(if_id, &addr, true));
}

static void test_ipv6_if_add_unspecified_addr_without_anycast(void)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ipv6_if_add_addr(if_id, &addr, false));
}

static void test_ipv6_if_add_loopback_addr_with_anycast(void)
{
    ipv6_addr_t addr = IPV6_ADDR_LOOPBACK;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ipv6_if_add_addr(if_id, &addr, true));
}

static void test_ipv6_if_add_loopback_addr_without_anycast(void)
{
    ipv6_addr_t addr = IPV6_ADDR_LOOPBACK;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ipv6_if_add_addr(if_id, &addr, false));
}

static void test_ipv6_if_add_multicast_addr_with_anycast(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_NODES_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ipv6_if_add_addr(if_id, &addr, true));
}

static void test_ipv6_if_add_addr_too_many(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    for (uint32_t i = 0; i < IPV6_IF_ADDR_NUM; i++) {
        addr.u32[3].u32 = i;
        TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    }

    addr.u32[2].u32 = 0;
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, ipv6_if_add_addr(if_id, &addr, false));
}

static void test_ipv6_if_add_addr_two_times_same(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[0].addr)));

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[0].addr)));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[1].addr)));

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[0].addr)));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[1].addr)));
}

static void test_ipv6_if_add_unicast_addr(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *internal = NULL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    internal = ipv6_if_find_addr_on_if(if_id, &addr);

    TEST_ASSERT_NOT_NULL(internal);
    TEST_ASSERT(&addr != internal);

    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_unicast(internal));
}

static void test_ipv6_if_add_anycast_addr(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *internal = NULL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, true));

    internal = ipv6_if_find_addr_on_if(if_id, &addr);

    TEST_ASSERT_NOT_NULL(internal);
    TEST_ASSERT(&addr != internal);

    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_anycast(internal));
}

static void test_ipv6_if_add_multicast_addr(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_NODES_IF_LOCAL;
    ipv6_addr_t *internal = NULL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    internal = ipv6_if_find_addr_on_if(if_id, &addr);

    TEST_ASSERT_NOT_NULL(internal);
    TEST_ASSERT(&addr != internal);

    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_multicast(internal));
}

static void test_ipv6_if_add_link_local_unicast_addr(void)
{
    ipv6_addr_t addr = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *internal = NULL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    internal = ipv6_if_find_addr_on_if(if_id, &addr);

    TEST_ASSERT_NOT_NULL(internal);
    TEST_ASSERT(&addr != internal);

    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_link_local(internal));
}

static void test_ipv6_if_add_link_local_multicast_addr(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ipv6_addr_t *internal = NULL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    internal = ipv6_if_find_addr_on_if(if_id, &addr);

    TEST_ASSERT_NOT_NULL(internal);
    TEST_ASSERT(&addr != internal);

    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_link_local(internal));
}

static void test_ipv6_if_rem_addr_wrong_interface(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(-ENOENT, ipv6_if_rem_addr(INT_MAX, &addr));
}

static void test_ipv6_if_rem_addr(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[0].addr)));

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[0].addr)));

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_rem_addr(if_id, &addr));

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unspecified(&(ipv6_if_get_by_id(if_id)->addrs[0].addr)));
}

static void test_ipv6_if_iter_addr_wrong_interface(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_iter_addr(INT_MAX, &addr));
}

static void test_ipv6_if_iter_addr_not_registered(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_iter_addr(if_id, &addr));
}

static void test_ipv6_if_iter_addr_one(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));

    TEST_ASSERT_NOT_NULL(res = ipv6_if_iter_addr(if_id, NULL));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&addr, res));
    TEST_ASSERT_NULL(ipv6_if_iter_addr(if_id, res));
}

static void test_ipv6_if_iter_addr_two(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &b, false));

    TEST_ASSERT_NOT_NULL(res = ipv6_if_iter_addr(if_id, NULL));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, res));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_iter_addr(if_id, res));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&b, res));
    TEST_ASSERT_NULL(ipv6_if_iter_addr(if_id, res));
}

static void test_ipv6_if_iter_addr_two_with_hole(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        }
    };
    ipv6_addr_t c = { {
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &b, false));
    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &c, false));
    TEST_ASSERT_EQUAL_INT(0, ipv6_if_rem_addr(if_id, &b));

    TEST_ASSERT_NOT_NULL(res = ipv6_if_iter_addr(if_id, NULL));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, res));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_iter_addr(if_id, res));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&c, res));
    TEST_ASSERT_NULL(ipv6_if_iter_addr(if_id, res));
}

static void test_ipv6_if_iter_addr_full(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res = NULL;
    uint32_t counter = 0;

    while (ipv6_if_add_addr(if_id, &addr, false) == 0) {
        addr.u32[3].u32 = counter;
        counter++;
    }

    TEST_ASSERT_EQUAL_INT(IPV6_IF_ADDR_NUM, counter);

    while (counter-- > 0) {
        TEST_ASSERT_NOT_NULL(res = ipv6_if_iter_addr(if_id, res));
    }

    TEST_ASSERT_NULL(ipv6_if_iter_addr(if_id, res));
}

static void test_ipv6_if_find_addr_on_if_wrong_interface(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NULL(ipv6_if_find_addr_on_if(INT_MAX, &addr));
}

static void test_ipv6_if_find_addr_on_if_not_found(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_find_addr_on_if(if_id, &addr));
}

static void test_ipv6_if_find_addr_on_if(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&addr, res));
}

static void test_ipv6_if_find_addr_not_found(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_find_addr(&res, &addr));
    TEST_ASSERT_EQUAL_INT(INT_MIN, res.if_id);
    TEST_ASSERT_NULL(res.addr);
}

static void test_ipv6_if_find_addr(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT(&res == ipv6_if_find_addr(&res, &addr));
    TEST_ASSERT_EQUAL_INT(if_id, res.if_id);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&addr, res.addr));
}

static void test_ipv6_if_find_prefix_match_on_if_wrong_if(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NULL(ipv6_if_find_prefix_match_on_if(INT_MAX, &addr));
}

static void test_ipv6_if_find_prefix_match_on_if_no_address_added(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_find_prefix_match_on_if(if_id, &addr));
}

static void test_ipv6_if_find_prefix_match_on_if(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_prefix_match_on_if(if_id, &b));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(res, &a));
}

static void test_ipv6_if_find_prefix_match_on_if_multicast(void)
{
    ipv6_addr_t a = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_prefix_match_on_if(if_id, &b));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(res, &a));
}

static void test_ipv6_if_find_prefix_match_no_address_added(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_find_addr(&res, &addr));
    TEST_ASSERT_EQUAL_INT(INT_MIN, res.if_id);
    TEST_ASSERT_NULL(res.addr);
}

static void test_ipv6_if_find_prefix_match(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        }
    };

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT(&res == ipv6_if_find_prefix_match(&res, &b));
    TEST_ASSERT_EQUAL_INT(if_id, res.if_id);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, res.addr));
}

static void test_ipv6_if_find_prefix_match_multicast(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t a = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT(&res == ipv6_if_find_prefix_match(&res, &b));
    TEST_ASSERT_EQUAL_INT(if_id, res.if_id);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, res.addr));
}

static void test_ipv6_if_get_best_src_addr_on_if_wrong_if(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NULL(ipv6_if_get_best_src_addr_on_if(INT_MAX, &addr));
}

static void test_ipv6_if_get_best_src_addr_on_if_no_address_added(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_get_best_src_addr_on_if(if_id, &addr));
}

static void test_ipv6_if_get_best_src_addr_on_if(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_get_best_src_addr_on_if(if_id, &b));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(res, &a));
}

static void test_ipv6_if_get_best_src_addr_on_if_multicast(void)
{
    ipv6_addr_t a = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT_NULL(ipv6_if_get_best_src_addr_on_if(if_id, &b));
}

static void test_ipv6_if_get_best_src_addr_no_address_added(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_NULL(ipv6_if_find_addr(&res, &addr));
    TEST_ASSERT_EQUAL_INT(INT_MIN, res.if_id);
    TEST_ASSERT_NULL(res.addr);
}

static void test_ipv6_if_get_best_src_addr(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        }
    };

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT(&res == ipv6_if_get_best_src_addr(&res, &b));
    TEST_ASSERT_EQUAL_INT(if_id, res.if_id);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, res.addr));
}

static void test_ipv6_if_get_best_src_addr_multicast(void)
{
    ipv6_if_addr_pair_t res = { INT_MIN, NULL };
    ipv6_addr_t a = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &a, false));
    TEST_ASSERT_NULL(ipv6_if_get_best_src_addr(&res, &b));
}

static void test_ipv6_if_addr_is_anycast_no_anycast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(false, ipv6_if_addr_is_anycast(res));
}

static void test_ipv6_if_addr_is_anycast_anycast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, true));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_anycast(res));
}

static void test_ipv6_if_addr_is_multicast_no_multicast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(false, ipv6_if_addr_is_multicast(res));
}

static void test_ipv6_if_addr_is_multicast_multicast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    ipv6_addr_set_solicited_nodes(&addr, &addr);

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_multicast(res));
}

static void test_ipv6_if_addr_is_unicast_anycast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, true));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(false, ipv6_if_addr_is_unicast(res));
}

static void test_ipv6_if_addr_is_unicast_multicast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    ipv6_addr_set_solicited_nodes(&addr, &addr);

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(false, ipv6_if_addr_is_unicast(res));
}

static void test_ipv6_if_addr_is_unicast_unicast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_unicast(res));
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_unspecified(&(ipv6_ifs[if_id].addrs[0].addr)));
    TEST_ASSERT_EQUAL_INT(IPV6_IF_ADDR_FLAGS_UNICAST, ipv6_ifs[if_id].addrs[0].flags);
}

static void test_ipv6_if_addr_is_link_local_no_link_local_unicast(void)
{
    ipv6_addr_t addr = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(false, ipv6_if_addr_is_link_local(res));
}

static void test_ipv6_if_addr_is_link_local_no_link_local_multicast(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_ROUTERS_IF_LOCAL;
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(false, ipv6_if_addr_is_link_local(res));
}

static void test_ipv6_if_addr_is_link_local_unicast(void)
{
    ipv6_addr_t addr = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_link_local(res));
}

static void test_ipv6_if_addr_is_link_local_multicast(void)
{
    ipv6_addr_t addr = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
    ipv6_addr_t *res;

    TEST_ASSERT_EQUAL_INT(0, ipv6_if_add_addr(if_id, &addr, false));
    TEST_ASSERT_NOT_NULL(res = ipv6_if_find_addr_on_if(if_id, &addr));
    TEST_ASSERT_EQUAL_INT(true, ipv6_if_addr_is_link_local(res));
}

Test *tests_ipv6_if_uninit_tests(void)
{
    EMB_UNIT_TESTFIXTURES(uninit_fixtures) {
        new_TestFixture(test_ipv6_if_init_if_EINVAL),
        new_TestFixture(test_ipv6_if_init_if_EISCONN),
        new_TestFixture(test_ipv6_if_init_if_ENOBUFS),
    };

    EMB_UNIT_TESTCALLER(ipv6_if_tests, NULL, NULL, uninit_fixtures);

    return (Test *)&ipv6_if_tests;
}

Test *tests_ipv6_if_init_tests(void)
{
    EMB_UNIT_TESTFIXTURES(init_fixtures) {
        new_TestFixture(test_ipv6_if_reset_if_and_test_if_initialized),
        new_TestFixture(test_ipv6_if_initialized),
        new_TestFixture(test_ipv6_if_get_by_id_uninitialized),
        new_TestFixture(test_ipv6_if_get_by_id_negative),
        new_TestFixture(test_ipv6_if_get_by_id),
        new_TestFixture(test_ipv6_if_get_id_by_mac_KERNEL_PID_UNDEF),
        new_TestFixture(test_ipv6_if_get_id_by_mac_KERNEL_PID_LAST),
        new_TestFixture(test_ipv6_if_get_id_by_mac),
        new_TestFixture(test_ipv6_if_get_by_mac_KERNEL_PID_UNDEF),
        new_TestFixture(test_ipv6_if_get_by_mac_KERNEL_PID_LAST),
        new_TestFixture(test_ipv6_if_get_by_mac),
        new_TestFixture(test_ipv6_if_add_addr_wrong_interface_with_anycast),
        new_TestFixture(test_ipv6_if_add_addr_wrong_interface_without_anycast),
        new_TestFixture(test_ipv6_if_add_unspecified_addr_with_anycast),
        new_TestFixture(test_ipv6_if_add_unspecified_addr_without_anycast),
        new_TestFixture(test_ipv6_if_add_loopback_addr_with_anycast),
        new_TestFixture(test_ipv6_if_add_loopback_addr_without_anycast),
        new_TestFixture(test_ipv6_if_add_multicast_addr_with_anycast),
        new_TestFixture(test_ipv6_if_add_addr_too_many),
        new_TestFixture(test_ipv6_if_add_addr_two_times_same),
        new_TestFixture(test_ipv6_if_add_anycast_addr),
        new_TestFixture(test_ipv6_if_add_unicast_addr),
        new_TestFixture(test_ipv6_if_add_multicast_addr),
        new_TestFixture(test_ipv6_if_add_link_local_unicast_addr),
        new_TestFixture(test_ipv6_if_add_link_local_multicast_addr),
        new_TestFixture(test_ipv6_if_rem_addr_wrong_interface),
        new_TestFixture(test_ipv6_if_rem_addr),
        new_TestFixture(test_ipv6_if_iter_addr_wrong_interface),
        new_TestFixture(test_ipv6_if_iter_addr_not_registered),
        new_TestFixture(test_ipv6_if_iter_addr_one),
        new_TestFixture(test_ipv6_if_iter_addr_two),
        new_TestFixture(test_ipv6_if_iter_addr_two_with_hole),
        new_TestFixture(test_ipv6_if_iter_addr_full),
        new_TestFixture(test_ipv6_if_find_addr_on_if_wrong_interface),
        new_TestFixture(test_ipv6_if_find_addr_on_if_not_found),
        new_TestFixture(test_ipv6_if_find_addr_on_if),
        new_TestFixture(test_ipv6_if_find_addr_not_found),
        new_TestFixture(test_ipv6_if_find_addr),
        new_TestFixture(test_ipv6_if_find_prefix_match_on_if_wrong_if),
        new_TestFixture(test_ipv6_if_find_prefix_match_on_if_no_address_added),
        new_TestFixture(test_ipv6_if_find_prefix_match_on_if),
        new_TestFixture(test_ipv6_if_find_prefix_match_on_if_multicast),
        new_TestFixture(test_ipv6_if_find_prefix_match_no_address_added),
        new_TestFixture(test_ipv6_if_find_prefix_match),
        new_TestFixture(test_ipv6_if_find_prefix_match_multicast),
        new_TestFixture(test_ipv6_if_get_best_src_addr_on_if_wrong_if),
        new_TestFixture(test_ipv6_if_get_best_src_addr_on_if_no_address_added),
        new_TestFixture(test_ipv6_if_get_best_src_addr_on_if),
        new_TestFixture(test_ipv6_if_get_best_src_addr_on_if_multicast),
        new_TestFixture(test_ipv6_if_get_best_src_addr_no_address_added),
        new_TestFixture(test_ipv6_if_get_best_src_addr),
        new_TestFixture(test_ipv6_if_get_best_src_addr_multicast),
        new_TestFixture(test_ipv6_if_addr_is_anycast_no_anycast),
        new_TestFixture(test_ipv6_if_addr_is_anycast_anycast),
        new_TestFixture(test_ipv6_if_addr_is_multicast_no_multicast),
        new_TestFixture(test_ipv6_if_addr_is_multicast_multicast),
        new_TestFixture(test_ipv6_if_addr_is_unicast_anycast),
        new_TestFixture(test_ipv6_if_addr_is_unicast_multicast),
        new_TestFixture(test_ipv6_if_addr_is_unicast_unicast),
        new_TestFixture(test_ipv6_if_addr_is_link_local_no_link_local_unicast),
        new_TestFixture(test_ipv6_if_addr_is_link_local_no_link_local_multicast),
        new_TestFixture(test_ipv6_if_addr_is_link_local_unicast),
        new_TestFixture(test_ipv6_if_addr_is_link_local_multicast),
    };

    EMB_UNIT_TESTCALLER(ipv6_if_tests, set_up, NULL, init_fixtures);

    return (Test *)&ipv6_if_tests;
}

void tests_ipv6_if(void)
{
    TESTS_RUN(tests_ipv6_if_uninit_tests());
    TESTS_RUN(tests_ipv6_if_init_tests());
}
/** @} */
