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
 * @file    tests-ipv6_addr.c
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "byteorder.h"
#include "ipv6/addr.h"

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

static void test_ipv6_addr_is_multicast_not_multicast(void)
{
    ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
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

static void test_ipv6_addr_set_multicast(void)
{
    ipv6_addr_t a;
    ipv6_addr_mcast_flag_t flags = (ipv6_addr_mcast_flag_t)5;
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_REALM_LOCAL;

    ipv6_addr_set_multicast(&a, flags, scope);

    TEST_ASSERT_EQUAL_INT(0xff, a.u8[0]);
    TEST_ASSERT_EQUAL_INT((flags << 4) | scope, a.u8[1]);
}

static void test_ipv6_addr_set_all_nodes_multicast_if_local(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = IPV6_ADDR_ALL_NODES_IF_LOCAL;
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_IF_LOCAL;

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
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff020000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(b.u64[1]));

    ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_nodes_multicast_unusual(void)
{
    ipv6_addr_t a;
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_REALM_LOCAL;

    ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(0xff030000, byteorder_ntohl(a.u32[0]));
    TEST_ASSERT_EQUAL_INT(0, a.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(a.u64[1]));
}

static void test_ipv6_addr_set_all_routers_multicast_if_local(void)
{
    ipv6_addr_t a = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t b = IPV6_ADDR_ALL_ROUTERS_IF_LOCAL;
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_IF_LOCAL;

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
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_LINK_LOCAL;

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
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_SITE_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff050000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(b.u64[1]));

    ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_routers_multicast_unusual(void)
{
    ipv6_addr_t a;
    ipv6_addr_mcast_scp_t scope = IPV6_ADDR_MCAST_SCP_ORG_LOCAL;

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

Test *tests_ipv6_addr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_addr_equal_not_equal),
        new_TestFixture(test_ipv6_addr_equal_equal),
        new_TestFixture(test_ipv6_addr_is_unspecified_not_unspecified),
        new_TestFixture(test_ipv6_addr_is_unspecified_unspecified),
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
        new_TestFixture(test_ipv6_addr_set_unspecified),
        new_TestFixture(test_ipv6_addr_set_loopback),
        new_TestFixture(test_ipv6_addr_set_link_local_prefix),
        new_TestFixture(test_ipv6_addr_set_iid),
        new_TestFixture(test_ipv6_addr_set_multicast),
        new_TestFixture(test_ipv6_addr_set_all_nodes_multicast_if_local),
        new_TestFixture(test_ipv6_addr_set_all_nodes_multicast_link_local),
        new_TestFixture(test_ipv6_addr_set_all_nodes_multicast_unusual),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_if_local),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_link_local),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_site_local),
        new_TestFixture(test_ipv6_addr_set_all_routers_multicast_unusual),
        new_TestFixture(test_ipv6_addr_set_solicited_nodes),
    };

    EMB_UNIT_TESTCALLER(ipv6_addr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv6_addr_tests;
}

void tests_ipv6_addr(void)
{
    TESTS_RUN(tests_ipv6_addr_tests());
}
/** @} */
