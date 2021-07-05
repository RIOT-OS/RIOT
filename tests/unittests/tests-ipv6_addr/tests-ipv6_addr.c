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

#include "embUnit/embUnit.h"

#include "byteorder.h"
#include "net/ipv6/addr.h"

#include "tests-ipv6_addr.h"

static void test_ipv6_addr_equal_not_equal(void)
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
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_not_equal2(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_not_equal3(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0e
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_not_equal4(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x25, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_equal(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_is_unspecified_not_unspecified(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_unspecified(&a));
}

static void test_ipv6_addr_is_unspecified_unspecified(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;

    TEST_ASSERT_EQUAL_INT(0, a.u64[0].u64); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, a.u64[1].u64);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unspecified(&a));
}

static void test_ipv6_addr_is_global_is_link_local(void)
{
    ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_global(&a));
}

static void test_ipv6_addr_is_global1(void)
{
    /* riot-os.org has IPv6 address 2a01:4f8:151:64::11 */
    ipv6_addr_t a = { {
            0x2a, 0x01, 0x04, 0xf8, 0x01, 0x51, 0x00, 0x64,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_global(&a));
}

static void test_ipv6_addr_is_global2(void)
{
    ipv6_addr_t a = { {
            0xaf, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xbe, 0xef, 0xca, 0xfe, 0x12, 0x34, 0xab, 0xcd
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_global(&a));
}

static void test_ipv6_addr_is_global_multicast_not_global(void)
{
    ipv6_addr_t a = { {
            0xff, 0x15, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_global(&a));
}

static void test_ipv6_addr_is_global_multicast(void)
{
    ipv6_addr_t a = { {
            0xff, 0x1e, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_global(&a));
}

static void test_ipv6_addr_is_ipv4_compat_not_ipv4_compat1(void)
{
    ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xff, 0xff, 0xc0, 0xa8, 0x00, 0x01
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_ipv4_compat(&a));
}

static void test_ipv6_addr_is_ipv4_compat_not_ipv4_compat2(void)
{
    ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0xc0, 0xa8, 0x00, 0x01
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_ipv4_compat(&a));
}

static void test_ipv6_addr_is_ipv4_compat_ipv4_compat(void)
{
    ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_ipv4_compat(&a));
}

static void test_ipv6_addr_is_ipv4_mapped_not_ipv4_mapped1(void)
{
    ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_ipv4_mapped(&a));
}

static void test_ipv6_addr_is_ipv4_mapped_not_ipv4_mapped2(void)
{
    ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x01, 0xff, 0xff, 0xc0, 0xa8, 0x00, 0x01
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_ipv4_mapped(&a));
}

static void test_ipv6_addr_is_ipv4_mapped_ipv4_mapped(void)
{
    ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xff, 0xff, 0xc0, 0xa8, 0x00, 0x01
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_ipv4_mapped(&a));
}

static void test_ipv6_addr_is_site_local_is_link_local(void)
{
    ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_site_local(&a));
}

static void test_ipv6_addr_is_site_local_not_site_local(void)
{
    ipv6_addr_t a = { {
            0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_site_local(&a));
}

static void test_ipv6_addr_is_site_local_site_local_multicast(void)
{
    ipv6_addr_t a = { {
            0xff, 0x15, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_site_local(&a));
}


static void test_ipv6_addr_is_site_local(void)
{
    ipv6_addr_t a = { {
            0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_site_local(&a));
}

static void test_ipv6_addr_is_multicast_not_multicast(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0xff,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_multicast(&a));
}

static void test_ipv6_addr_is_multicast_multicast(void)
{
    ipv6_addr_t a = { {
            0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_multicast(&a));
}

static void test_ipv6_addr_is_loopback_not_loopback(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_loopback(&a));
}

static void test_ipv6_addr_is_loopback_loopback(void)
{
    ipv6_addr_t a = IPV6_ADDR_LOOPBACK;

    TEST_ASSERT_EQUAL_INT(0, a.u64[0].u64); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(a.u64[1]));
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_loopback(&a));
}

static void test_ipv6_addr_is_link_local_not_link_local(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_nearly_link_local(void)
{
    ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_link_local_unicast(void)
{
    ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_link_local_multicast1(void)
{
    ipv6_addr_t a = { {
            0xff, 0x12, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_link_local_multicasta(void)
{
    ipv6_addr_t a = { {
            0xff, 0xa2, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_unique_local_unicast_not_unique_local_unicast(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_unique_local_unicast(&a));
}

static void test_ipv6_addr_is_unique_local_unicast_unique_local_unicast_locally_assigned(void)
{
    ipv6_addr_t a = { {
            0xfd, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unique_local_unicast(&a));
}

static void test_ipv6_addr_is_unique_local_unicast_unique_local_unicast_not_locally_assigned(void)
{
    ipv6_addr_t a = { {
            0xfc, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unique_local_unicast(&a));
}

static void test_ipv6_addr_is_solicited_node_no_solicited_node_multicast(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_solicited_node(&a));
}

static void test_ipv6_addr_is_solicited_node_multicast_but_no_solicited_node(void)
{
    ipv6_addr_t a = { {
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ipv6_addr_is_solicited_node(&a));
}

static void test_ipv6_addr_is_solicited_node_solicited_node_multicast(void)
{
    ipv6_addr_t a = { {
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0xff, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_solicited_node(&a));
}

static void test_ipv6_addr_match_prefix_first_NULL(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(0, ipv6_addr_match_prefix(NULL, &a));
}

static void test_ipv6_addr_match_prefix_second_NULL(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(0, ipv6_addr_match_prefix(&a, NULL));
}

static void test_ipv6_addr_match_prefix_no_match(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
            0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0
        }
    };
    TEST_ASSERT_EQUAL_INT(0, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_1(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(1, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_2(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x20, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(2, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_3(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x10, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(3, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_6(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x02, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(6, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_127(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0e
        }
    };
    TEST_ASSERT_EQUAL_INT(127, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_128(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
    TEST_ASSERT_EQUAL_INT(128, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_same_pointer(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(128, ipv6_addr_match_prefix(&a, &a));
}

static void test_ipv6_addr_init_prefix(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x02, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };
    ipv6_addr_t c =  { {
            0xff, 0xfe, 0xfd, 0xfe, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ipv6_addr_init_prefix(&c, &b, 31);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &c));
}

static void test_ipv6_addr_init_iid(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0xff, 0x0d, 0x0e, 0x0f
        }
    };

    const uint8_t iid[] = { 0x13, 0x02, 0x01, 0x00 };

    ipv6_addr_t c =  { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x93, 0x02, 0x01, 0x00
        }
    };

    ipv6_addr_init_iid(&a, iid, 31);
    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &c));
}

static void test_ipv6_addr_set_unspecified(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ipv6_addr_set_unspecified(&a);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_unspecified(&a));
}

static void test_ipv6_addr_set_loopback(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ipv6_addr_set_loopback(&a);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_is_loopback(&a));
}

static void test_ipv6_addr_set_link_local_prefix(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ipv6_addr_t b = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };

    ipv6_addr_set_link_local_prefix(&a);

    TEST_ASSERT_EQUAL_INT(64, ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_set_iid(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };

    ipv6_addr_set_iid(&a, byteorder_ntohll(b.u64[1]));

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_aiid(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };

    ipv6_addr_set_aiid(&a, &(b.u8[8]));

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_multicast(void)
{
    ipv6_addr_t a;
    unsigned int flags = 5U;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_REALM_LOCAL;

    ipv6_addr_set_multicast(&a, flags, scope);

    TEST_ASSERT_EQUAL_INT(0xff, a.u8[0]);
    TEST_ASSERT_EQUAL_INT((flags << 4) | scope, a.u8[1]);
}

static void test_ipv6_addr_set_all_nodes_multicast_if_local(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = IPV6_ADDR_ALL_NODES_IF_LOCAL;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_IF_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff010000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(b.u64[1]));

    ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_nodes_multicast_link_local(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff020000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(b.u64[1]));

    ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_nodes_multicast_unusual(void)
{
    ipv6_addr_t a;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_REALM_LOCAL;

    ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(0xff030000, byteorder_ntohl(a.u32[0]));
    TEST_ASSERT_EQUAL_INT(0, a.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(a.u64[1]));
}

static void test_ipv6_addr_set_all_routers_multicast_if_local(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_IF_LOCAL;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_IF_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff010000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(b.u64[1]));

    ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_routers_multicast_link_local(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff020000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(b.u64[1]));

    ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_routers_multicast_site_local(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_SITE_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff050000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(b.u64[1]));

    ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_routers_multicast_unusual(void)
{
    ipv6_addr_t a;
    unsigned int scope = IPV6_ADDR_MCAST_SCP_ORG_LOCAL;

    ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(0xff080000, byteorder_ntohl(a.u32[0]));
    TEST_ASSERT_EQUAL_INT(0, a.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(a.u64[1]));
}

static void test_ipv6_addr_set_solicited_nodes(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ipv6_addr_set_solicited_nodes(&a, &b);

    TEST_ASSERT_EQUAL_INT(0xff020000, byteorder_ntohl(a.u32[0]));
    TEST_ASSERT_EQUAL_INT(0, a.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohl(a.u32[2]));
    TEST_ASSERT_EQUAL_INT(0xff0d0e0f, byteorder_ntohl(a.u32[3]));
}

static void test_ipv6_addr_to_str__string_too_short(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    char result[1];

    TEST_ASSERT_NULL(ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__addr_NULL(void)
{
    char result[IPV6_ADDR_MAX_STR_LEN];

    TEST_ASSERT_NULL(ipv6_addr_to_str(result, NULL, sizeof(result)));
}

static void test_ipv6_addr_to_str__result_NULL(void)
{
    ipv6_addr_t a = {0};

    TEST_ASSERT_NULL(ipv6_addr_to_str(NULL, &a, IPV6_ADDR_MAX_STR_LEN));
}

static void test_ipv6_addr_to_str__success(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    char result[IPV6_ADDR_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("1:203:405:607:809:a0b:c0d:e0f",
                             ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success2(void)
{
    ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };
    char result[IPV6_ADDR_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("fe80::f8f9:fafb:fcfd:feff",
                             ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success3(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    char result[sizeof("::")];

    TEST_ASSERT_EQUAL_STRING("::", ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success4(void)
{
    ipv6_addr_t a = IPV6_ADDR_LOOPBACK;
    char result[sizeof("::1")];

    TEST_ASSERT_EQUAL_STRING("::1", ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success5(void)
{
    ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xff, 0xff, 192, 168, 0, 1
        }
    };
    char result[IPV6_ADDR_MAX_STR_LEN];

#ifdef MODULE_IPV4_ADDR
    TEST_ASSERT_EQUAL_STRING("::ffff:192.168.0.1", ipv6_addr_to_str(result, &a, sizeof(result)));
#else
    TEST_ASSERT_EQUAL_STRING("::ffff:c0a8:1", ipv6_addr_to_str(result, &a, sizeof(result)));
#endif
}

static void test_ipv6_addr_from_str__one_colon_start(void)
{
    ipv6_addr_t result;

    TEST_ASSERT_NULL(ipv6_addr_from_str(&result, ":ff::1"));
}

#define CANARY_DATA     0xc2, 0x8c, 0x36, 0x26, 0x24, 0x16, 0xd1, 0xd6

static void test_ipv6_addr_from_str__overflow(void)
{
    uint8_t result[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                         CANARY_DATA };
    static const uint8_t canary_exp[] = { CANARY_DATA };
    ipv6_addr_t *addr = (ipv6_addr_t *)&result[0];
    uint8_t *canary = &result[sizeof(ipv6_addr_t)];

    TEST_ASSERT_NULL(ipv6_addr_from_str(addr, "::A:A:A:A:A:A:A:A:A:A"));
    TEST_ASSERT_EQUAL_INT(0, memcmp(canary, canary_exp, sizeof(canary_exp)));
}

static void test_ipv6_addr_from_str__three_colons(void)
{
    ipv6_addr_t result;

    TEST_ASSERT_NULL(ipv6_addr_from_str(&result, "ff02:::1"));
}

static void test_ipv6_addr_from_str__string_too_long(void)
{
    ipv6_addr_t result;

    TEST_ASSERT_NULL(ipv6_addr_from_str(&result, "ffff:ffff:ffff:ffff:"
                                        "ffff:ffff:ffff:ffff:ffff"));
}

static void test_ipv6_addr_from_str__illegal_chars(void)
{
    ipv6_addr_t result;

    TEST_ASSERT_NULL(ipv6_addr_from_str(&result, ":-)"));
}

static void test_ipv6_addr_from_str__illegal_encapsulated_ipv4(void)
{
    ipv6_addr_t result;

    TEST_ASSERT_NULL(ipv6_addr_from_str(&result, "192.168.0.1"));
}

static void test_ipv6_addr_from_str__addr_NULL(void)
{
    ipv6_addr_t result;

    TEST_ASSERT_NULL(ipv6_addr_from_str(&result, NULL));
}

static void test_ipv6_addr_from_str__result_NULL(void)
{
    TEST_ASSERT_NULL(ipv6_addr_from_str(NULL, "::"));
}

static void test_ipv6_addr_from_str__success(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&result, "1:203:405:607:809:a0b:c0d:e0f"));
    TEST_ASSERT(ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success2(void)
{
    ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };
    ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&result, "fe80::f8f9:fafb:fcfd:feff"));
    TEST_ASSERT(ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success3(void)
{
    ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };
    ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&result, "FE80::F8F9:FAFB:FCFD:FEFF"));
    TEST_ASSERT(ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success4(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&result, "::"));
    TEST_ASSERT(ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success5(void)
{
    ipv6_addr_t a = IPV6_ADDR_LOOPBACK;
    ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&result, "::1"));
    TEST_ASSERT(ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success6(void)
{
    ipv6_addr_t a = { {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 255, 255, 255, 255
        }
    };
    ipv6_addr_t result;

#ifdef MODULE_IPV4_ADDR
    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&result, "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"));
#else
    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&result, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"));
#endif
    TEST_ASSERT(ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_split_iface__no_iface(void)
{
    char a[] = "fe80::f8f9:fafb:fcfd:feff";
    TEST_ASSERT_NULL(ipv6_addr_split_iface(a));
}

static void test_ipv6_addr_split_iface__with_iface(void)
{
    char a[] = "fe80::f8f9:fafb:fcfd:feff%eth0";
    char *iface = ipv6_addr_split_iface(a);
    TEST_ASSERT_NOT_NULL(iface);
    TEST_ASSERT_EQUAL_INT(strcmp("eth0", iface), 0);
    /* check that the separator has been replaced with '\0' */
    TEST_ASSERT_EQUAL_INT(*(iface - 1), '\0');
}

static void test_ipv6_addr_split_prefix__no_prefix(void)
{
    char a[] = "fd00:dead:beef::1";
    TEST_ASSERT_EQUAL_INT(ipv6_addr_split_prefix(a), 128);
}

static void test_ipv6_addr_split_prefix__with_prefix(void)
{
    char a[] = "fd00:dead:beef::1/64";
    TEST_ASSERT_EQUAL_INT(ipv6_addr_split_prefix(a), 64);
    /* check that the separator has been replaced with '\0' */
    TEST_ASSERT_EQUAL_INT(strcmp("fd00:dead:beef::1", a), 0);
}

static void test_ipv6_addr_from_buf__success(void)
{
    ipv6_addr_t a = { {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 255, 255, 255, 255
        }
    };
    ipv6_addr_t result;

#ifdef MODULE_IPV4_ADDR
    TEST_ASSERT_NOT_NULL(ipv6_addr_from_buf(&result, "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255%tap0", 45));
#else
    TEST_ASSERT_NOT_NULL(ipv6_addr_from_buf(&result, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff%tap0", 39));
#endif
    TEST_ASSERT(ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_buf__too_long_len(void)
{
    ipv6_addr_t result;

    TEST_ASSERT_NULL(ipv6_addr_from_buf(&result, "::1", IPV6_ADDR_MAX_STR_LEN + 1));
}

Test *tests_ipv6_addr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_addr_equal_not_equal),
        new_TestFixture(test_ipv6_addr_equal_not_equal2),
        new_TestFixture(test_ipv6_addr_equal_not_equal3),
        new_TestFixture(test_ipv6_addr_equal_not_equal4),
        new_TestFixture(test_ipv6_addr_equal_equal),
        new_TestFixture(test_ipv6_addr_is_unspecified_not_unspecified),
        new_TestFixture(test_ipv6_addr_is_unspecified_unspecified),
        new_TestFixture(test_ipv6_addr_is_global_is_link_local),
        new_TestFixture(test_ipv6_addr_is_global1),
        new_TestFixture(test_ipv6_addr_is_global2),
        new_TestFixture(test_ipv6_addr_is_global_multicast_not_global),
        new_TestFixture(test_ipv6_addr_is_global_multicast),
        new_TestFixture(test_ipv6_addr_is_ipv4_compat_not_ipv4_compat1),
        new_TestFixture(test_ipv6_addr_is_ipv4_compat_not_ipv4_compat2),
        new_TestFixture(test_ipv6_addr_is_ipv4_compat_ipv4_compat),
        new_TestFixture(test_ipv6_addr_is_ipv4_mapped_not_ipv4_mapped1),
        new_TestFixture(test_ipv6_addr_is_ipv4_mapped_not_ipv4_mapped2),
        new_TestFixture(test_ipv6_addr_is_ipv4_mapped_ipv4_mapped),
        new_TestFixture(test_ipv6_addr_is_site_local_is_link_local),
        new_TestFixture(test_ipv6_addr_is_site_local_site_local_multicast),
        new_TestFixture(test_ipv6_addr_is_site_local_not_site_local),
        new_TestFixture(test_ipv6_addr_is_site_local),
        new_TestFixture(test_ipv6_addr_is_multicast_not_multicast),
        new_TestFixture(test_ipv6_addr_is_multicast_multicast),
        new_TestFixture(test_ipv6_addr_is_loopback_not_loopback),
        new_TestFixture(test_ipv6_addr_is_loopback_loopback),
        new_TestFixture(test_ipv6_addr_is_link_local_not_link_local),
        new_TestFixture(test_ipv6_addr_is_link_local_nearly_link_local),
        new_TestFixture(test_ipv6_addr_is_link_local_link_local_unicast),
        new_TestFixture(test_ipv6_addr_is_link_local_link_local_multicast1),
        new_TestFixture(test_ipv6_addr_is_link_local_link_local_multicasta),
        new_TestFixture(test_ipv6_addr_is_unique_local_unicast_not_unique_local_unicast),
        new_TestFixture(test_ipv6_addr_is_unique_local_unicast_unique_local_unicast_locally_assigned),
        new_TestFixture(test_ipv6_addr_is_unique_local_unicast_unique_local_unicast_not_locally_assigned),
        new_TestFixture(test_ipv6_addr_is_solicited_node_no_solicited_node_multicast),
        new_TestFixture(test_ipv6_addr_is_solicited_node_multicast_but_no_solicited_node),
        new_TestFixture(test_ipv6_addr_is_solicited_node_solicited_node_multicast),
        new_TestFixture(test_ipv6_addr_match_prefix_first_NULL),
        new_TestFixture(test_ipv6_addr_match_prefix_second_NULL),
        new_TestFixture(test_ipv6_addr_match_prefix_no_match),
        new_TestFixture(test_ipv6_addr_match_prefix_match_1),
        new_TestFixture(test_ipv6_addr_match_prefix_match_2),
        new_TestFixture(test_ipv6_addr_match_prefix_match_3),
        new_TestFixture(test_ipv6_addr_match_prefix_match_6),
        new_TestFixture(test_ipv6_addr_match_prefix_match_127),
        new_TestFixture(test_ipv6_addr_match_prefix_match_128),
        new_TestFixture(test_ipv6_addr_match_prefix_same_pointer),
        new_TestFixture(test_ipv6_addr_init_prefix),
        new_TestFixture(test_ipv6_addr_init_iid),
        new_TestFixture(test_ipv6_addr_set_unspecified),
        new_TestFixture(test_ipv6_addr_set_loopback),
        new_TestFixture(test_ipv6_addr_set_link_local_prefix),
        new_TestFixture(test_ipv6_addr_set_iid),
        new_TestFixture(test_ipv6_addr_set_aiid),
        new_TestFixture(test_ipv6_addr_set_multicast),
        new_TestFixture(test_ipv6_addr_set_all_nodes_multicast_if_local),
        new_TestFixture(test_ipv6_addr_set_all_nodes_multicast_link_local),
        new_TestFixture(test_ipv6_addr_set_all_nodes_multicast_unusual),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_if_local),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_link_local),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_site_local),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_unusual),
        new_TestFixture(test_ipv6_addr_set_solicited_nodes),
        new_TestFixture(test_ipv6_addr_to_str__string_too_short),
        new_TestFixture(test_ipv6_addr_to_str__addr_NULL),
        new_TestFixture(test_ipv6_addr_to_str__result_NULL),
        new_TestFixture(test_ipv6_addr_to_str__success),
        new_TestFixture(test_ipv6_addr_to_str__success2),
        new_TestFixture(test_ipv6_addr_to_str__success3),
        new_TestFixture(test_ipv6_addr_to_str__success4),
        new_TestFixture(test_ipv6_addr_to_str__success5),
        new_TestFixture(test_ipv6_addr_from_str__one_colon_start),
        new_TestFixture(test_ipv6_addr_from_str__overflow),
        new_TestFixture(test_ipv6_addr_from_str__three_colons),
        new_TestFixture(test_ipv6_addr_from_str__string_too_long),
        new_TestFixture(test_ipv6_addr_from_str__illegal_chars),
        new_TestFixture(test_ipv6_addr_from_str__illegal_encapsulated_ipv4),
        new_TestFixture(test_ipv6_addr_from_str__addr_NULL),
        new_TestFixture(test_ipv6_addr_from_str__result_NULL),
        new_TestFixture(test_ipv6_addr_from_str__success),
        new_TestFixture(test_ipv6_addr_from_str__success2),
        new_TestFixture(test_ipv6_addr_from_str__success3),
        new_TestFixture(test_ipv6_addr_from_str__success4),
        new_TestFixture(test_ipv6_addr_from_str__success5),
        new_TestFixture(test_ipv6_addr_from_str__success6),
        new_TestFixture(test_ipv6_addr_split_iface__no_iface),
        new_TestFixture(test_ipv6_addr_split_iface__with_iface),
        new_TestFixture(test_ipv6_addr_split_prefix__no_prefix),
        new_TestFixture(test_ipv6_addr_split_prefix__with_prefix),
        new_TestFixture(test_ipv6_addr_from_buf__success),
        new_TestFixture(test_ipv6_addr_from_buf__too_long_len),
    };

    EMB_UNIT_TESTCALLER(ipv6_addr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv6_addr_tests;
}

void tests_ipv6_addr(void)
{
    TESTS_RUN(tests_ipv6_addr_tests());
}
/** @} */
