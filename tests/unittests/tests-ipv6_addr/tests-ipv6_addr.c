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
 * @file    tests-ipv6_addr.c
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "byteorder.h"
#include "net/ng_ipv6/addr.h"

#include "tests-ipv6_addr.h"

static void test_ipv6_addr_equal_not_equal(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_not_equal2(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_not_equal3(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0e
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_not_equal4(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x25, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_equal_equal(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_is_unspecified_not_unspecified(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_unspecified(&a));
}

static void test_ipv6_addr_is_unspecified_unspecified(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;

    TEST_ASSERT_EQUAL_INT(0, a.u64[0].u64); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, a.u64[1].u64);
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_unspecified(&a));
}

static void test_ipv6_addr_is_multicast_not_multicast(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0xff,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_multicast(&a));
}

static void test_ipv6_addr_is_multicast_multicast(void)
{
    ng_ipv6_addr_t a = { {
            0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_multicast(&a));
}

static void test_ipv6_addr_is_loopback_not_loopback(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_loopback(&a));
}

static void test_ipv6_addr_is_loopback_loopback(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_LOOPBACK;

    TEST_ASSERT_EQUAL_INT(0, a.u64[0].u64); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(a.u64[1]));
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_loopback(&a));
}

static void test_ipv6_addr_is_link_local_not_link_local(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_nearly_link_local(void)
{
    ng_ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_link_local_unicast(void)
{
    ng_ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_link_local_multicast1(void)
{
    ng_ipv6_addr_t a = { {
            0xff, 0x12, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_link_local_link_local_multicasta(void)
{
    ng_ipv6_addr_t a = { {
            0xff, 0xa2, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_link_local(&a));
}

static void test_ipv6_addr_is_unique_local_unicast_not_unique_local_unicast(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_unique_local_unicast(&a));
}

static void test_ipv6_addr_is_unique_local_unicast_unique_local_unicast_locally_assigned(void)
{
    ng_ipv6_addr_t a = { {
            0xfd, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_unique_local_unicast(&a));
}

static void test_ipv6_addr_is_unique_local_unicast_unique_local_unicast_not_locally_assigned(void)
{
    ng_ipv6_addr_t a = { {
            0xfc, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_unique_local_unicast(&a));
}

static void test_ipv6_addr_is_solicited_node_no_solicited_node_multicast(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_solicited_node(&a));
}

static void test_ipv6_addr_is_solicited_node_multicast_but_no_solicited_node(void)
{
    ng_ipv6_addr_t a = { {
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(false, ng_ipv6_addr_is_solicited_node(&a));
}

static void test_ipv6_addr_is_solicited_node_solicited_node_multicast(void)
{
    ng_ipv6_addr_t a = { {
            0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x01, 0xff, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_solicited_node(&a));
}

static void test_ipv6_addr_match_prefix_first_NULL(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_addr_match_prefix(NULL, &a));
}

static void test_ipv6_addr_match_prefix_second_NULL(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_addr_match_prefix(&a, NULL));
}

static void test_ipv6_addr_match_prefix_no_match(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8,
            0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0
        }
    };
    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_1(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(1, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_2(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x20, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(2, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_3(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x10, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(3, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_6(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x02, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(6, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_127(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0e
        }
    };
    TEST_ASSERT_EQUAL_INT(127, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_match_128(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
    TEST_ASSERT_EQUAL_INT(128, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_match_prefix_same_pointer(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    TEST_ASSERT_EQUAL_INT(128, ng_ipv6_addr_match_prefix(&a, &a));
}

static void test_ipv6_addr_init_prefix(void)
{
    ng_ipv6_addr_t a, b = { {
            0x00, 0x01, 0x02, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };

    ng_ipv6_addr_init_prefix(&a, &b, 31);
    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_unspecified(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ng_ipv6_addr_set_unspecified(&a);

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_unspecified(&a));
}

static void test_ipv6_addr_set_loopback(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ng_ipv6_addr_set_loopback(&a);

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_is_loopback(&a));
}

static void test_ipv6_addr_set_link_local_prefix(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ng_ipv6_addr_t b = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };

    ng_ipv6_addr_set_link_local_prefix(&a);

    TEST_ASSERT_EQUAL_INT(64, ng_ipv6_addr_match_prefix(&a, &b));
}

static void test_ipv6_addr_set_iid(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };

    ng_ipv6_addr_set_iid(&a, byteorder_ntohll(b.u64[1]));

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_aiid(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };

    ng_ipv6_addr_set_aiid(&a, &(b.u8[8]));

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_multicast(void)
{
    ng_ipv6_addr_t a;
    ng_ipv6_addr_mcast_flag_t flags = (ng_ipv6_addr_mcast_flag_t)5;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_REALM_LOCAL;

    ng_ipv6_addr_set_multicast(&a, flags, scope);

    TEST_ASSERT_EQUAL_INT(0xff, a.u8[0]);
    TEST_ASSERT_EQUAL_INT((flags << 4) | scope, a.u8[1]);
}

static void test_ipv6_addr_set_all_nodes_multicast_if_local(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t b = NG_IPV6_ADDR_ALL_NODES_IF_LOCAL;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_IF_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff010000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(b.u64[1]));

    ng_ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_nodes_multicast_link_local(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t b = NG_IPV6_ADDR_ALL_NODES_LINK_LOCAL;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff020000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(b.u64[1]));

    ng_ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_nodes_multicast_unusual(void)
{
    ng_ipv6_addr_t a;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_REALM_LOCAL;

    ng_ipv6_addr_set_all_nodes_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(0xff030000, byteorder_ntohl(a.u32[0]));
    TEST_ASSERT_EQUAL_INT(0, a.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohll(a.u64[1]));
}

static void test_ipv6_addr_set_all_routers_multicast_if_local(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t b = NG_IPV6_ADDR_ALL_ROUTERS_IF_LOCAL;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_IF_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff010000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(b.u64[1]));

    ng_ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_routers_multicast_link_local(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t b = NG_IPV6_ADDR_ALL_ROUTERS_LINK_LOCAL;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_LINK_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff020000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(b.u64[1]));

    ng_ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_routers_multicast_site_local(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t b = NG_IPV6_ADDR_ALL_ROUTERS_SITE_LOCAL;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_SITE_LOCAL;

    TEST_ASSERT_EQUAL_INT(0xff050000, byteorder_ntohl(b.u32[0])); /* Don't trust the macro ;) */
    TEST_ASSERT_EQUAL_INT(0, b.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(b.u64[1]));

    ng_ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(true, ng_ipv6_addr_equal(&a, &b));
}

static void test_ipv6_addr_set_all_routers_multicast_unusual(void)
{
    ng_ipv6_addr_t a;
    ng_ipv6_addr_mcast_scp_t scope = NG_IPV6_ADDR_MCAST_SCP_ORG_LOCAL;

    ng_ipv6_addr_set_all_routers_multicast(&a, scope);

    TEST_ASSERT_EQUAL_INT(0xff080000, byteorder_ntohl(a.u32[0]));
    TEST_ASSERT_EQUAL_INT(0, a.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(2, byteorder_ntohll(a.u64[1]));
}

static void test_ipv6_addr_set_solicited_nodes(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t b = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };

    ng_ipv6_addr_set_solicited_nodes(&a, &b);

    TEST_ASSERT_EQUAL_INT(0xff020000, byteorder_ntohl(a.u32[0]));
    TEST_ASSERT_EQUAL_INT(0, a.u32[1].u32);
    TEST_ASSERT_EQUAL_INT(1, byteorder_ntohl(a.u32[2]));
    TEST_ASSERT_EQUAL_INT(0xff0d0e0f, byteorder_ntohl(a.u32[3]));
}

static void test_ipv6_addr_to_str__string_too_short(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    char result[1];

    TEST_ASSERT_NULL(ng_ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__addr_NULL(void)
{
    char result[NG_IPV6_ADDR_MAX_STR_LEN];

    TEST_ASSERT_NULL(ng_ipv6_addr_to_str(result, NULL, sizeof(result)));
}

static void test_ipv6_addr_to_str__result_NULL(void)
{
    ng_ipv6_addr_t a;

    TEST_ASSERT_NULL(ng_ipv6_addr_to_str(NULL, &a, NG_IPV6_ADDR_MAX_STR_LEN));
}

static void test_ipv6_addr_to_str__success(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    char result[NG_IPV6_ADDR_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("1:203:405:607:809:a0b:c0d:e0f",
                             ng_ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success2(void)
{
    ng_ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };
    char result[NG_IPV6_ADDR_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("fe80::f8f9:fafb:fcfd:feff",
                             ng_ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success3(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    char result[sizeof("::")];

    TEST_ASSERT_EQUAL_STRING("::", ng_ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success4(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_LOOPBACK;
    char result[sizeof("::1")];

    TEST_ASSERT_EQUAL_STRING("::1", ng_ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_to_str__success5(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xff, 0xff, 192, 168, 0, 1
        }
    };
    char result[NG_IPV6_ADDR_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("::ffff:192.168.0.1",
                             ng_ipv6_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv6_addr_from_str__one_colon_start(void)
{
    ng_ipv6_addr_t result;

    TEST_ASSERT_NULL(ng_ipv6_addr_from_str(&result, ":ff::1"));
}

static void test_ipv6_addr_from_str__three_colons(void)
{
    ng_ipv6_addr_t result;

    TEST_ASSERT_NULL(ng_ipv6_addr_from_str(&result, "ff02:::1"));
}

static void test_ipv6_addr_from_str__string_too_long(void)
{
    ng_ipv6_addr_t result;

    TEST_ASSERT_NULL(ng_ipv6_addr_from_str(&result, "ffff:ffff:ffff:ffff:"
                                           "ffff:ffff:ffff:ffff:ffff"));
}

static void test_ipv6_addr_from_str__illegal_chars(void)
{
    ng_ipv6_addr_t result;

    TEST_ASSERT_NULL(ng_ipv6_addr_from_str(&result, ":-)"));
}

static void test_ipv6_addr_from_str__illegal_encapsulated_ipv4(void)
{
    ng_ipv6_addr_t result;

    TEST_ASSERT_NULL(ng_ipv6_addr_from_str(&result, "192.168.0.1"));
}

static void test_ipv6_addr_from_str__addr_NULL(void)
{
    ng_ipv6_addr_t result;

    TEST_ASSERT_NULL(ng_ipv6_addr_from_str(&result, NULL));
}

static void test_ipv6_addr_from_str__result_NULL(void)
{
    TEST_ASSERT_NULL(ng_ipv6_addr_from_str(NULL, "::"));
}

static void test_ipv6_addr_from_str__success(void)
{
    ng_ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ng_ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ng_ipv6_addr_from_str(&result, "1:203:405:607:809:a0b:c0d:e0f"));
    TEST_ASSERT(ng_ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success2(void)
{
    ng_ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };
    ng_ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ng_ipv6_addr_from_str(&result, "fe80::f8f9:fafb:fcfd:feff"));
    TEST_ASSERT(ng_ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success3(void)
{
    ng_ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };
    ng_ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ng_ipv6_addr_from_str(&result, "FE80::F8F9:FAFB:FCFD:FEFF"));
    TEST_ASSERT(ng_ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success4(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ng_ipv6_addr_from_str(&result, "::"));
    TEST_ASSERT(ng_ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success5(void)
{
    ng_ipv6_addr_t a = NG_IPV6_ADDR_LOOPBACK;
    ng_ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ng_ipv6_addr_from_str(&result, "::1"));
    TEST_ASSERT(ng_ipv6_addr_equal(&a, &result));
}

static void test_ipv6_addr_from_str__success6(void)
{
    ng_ipv6_addr_t a = { {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 255, 255, 255, 255
        }
    };
    ng_ipv6_addr_t result;

    TEST_ASSERT_NOT_NULL(ng_ipv6_addr_from_str(&result, "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"));
    TEST_ASSERT(ng_ipv6_addr_equal(&a, &result));
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
    };

    EMB_UNIT_TESTCALLER(ipv6_addr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv6_addr_tests;
}

void tests_ipv6_addr(void)
{
    TESTS_RUN(tests_ipv6_addr_tests());
}
/** @} */
