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
#include "kernel_types.h"
#include "net/gnrc/netif.h"

#include "unittests-constants.h"
#include "tests-netif.h"

static void set_up(void)
{
    gnrc_netif_init();
}

static void test_ng_netif_add__KERNEL_PID_UNDEF(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_add(KERNEL_PID_UNDEF));
    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(0, size);
}

static void test_ng_netif_add__memfull(void)
{
    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_add(TEST_UINT8 + i));
    }

    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_netif_add(TEST_UINT8 - 1));
}

static void test_ng_netif_add__success(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_add(TEST_UINT8));

    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, ifs[0]);
}

static void test_ng_netif_add__duplicate_entry(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;

    for (int i = 0; i < GNRC_NETIF_NUMOF + 4; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_add(TEST_UINT8));
    }

    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, ifs[0]);
}

static void test_ng_netif_remove__KERNEL_PID_UNDEF(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;

    test_ng_netif_add__success();

    gnrc_netif_remove(KERNEL_PID_UNDEF);

    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, ifs[0]);
}

static void test_ng_netif_remove__not_an_if(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;

    test_ng_netif_add__success();

    gnrc_netif_remove(TEST_UINT8 + 1);

    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, ifs[0]);
}

static void test_ng_netif_remove__success(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;

    test_ng_netif_add__success();

    gnrc_netif_remove(TEST_UINT8);

    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(0, size);
}

static void test_ng_netif_get__empty(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(0, size);
}

/* takes one out of the middle of the netif list and checks if all interfaces
 * are gotten regardless */
static void test_ng_netif_get__success_3_minus_one(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;
    int count = 0;

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_add(TEST_UINT8 + i));
    }

    gnrc_netif_remove(TEST_UINT8 + 1);

    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(2, size);

    for (size_t i = 0; i < size; i++) {
        if ((ifs[i] == TEST_UINT8) || ifs[i] == (TEST_UINT8 + 2)) {
            count++;
        }
    }

    TEST_ASSERT_EQUAL_INT(size, count);
}

static void test_ng_netif_get__full(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t size;

    for (int i = 0; i < GNRC_NETIF_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_add(TEST_UINT8 + i));
    }

    size = gnrc_netif_get(ifs);
    TEST_ASSERT_EQUAL_INT(GNRC_NETIF_NUMOF, size);
}

static void test_ng_netif_addr_to_str__out_too_short(void)
{
    static const uint8_t addr[] = {0x05, 0xcd};
    char out[2];

    TEST_ASSERT_NULL(gnrc_netif_addr_to_str(out, sizeof(out), addr, sizeof(addr)));
}

static void test_ng_netif_addr_to_str__success(void)
{
    static const uint8_t addr[] = {0x05, 0xcd};
    char out[3 * sizeof(addr)];

    TEST_ASSERT_EQUAL_STRING("05:cd", gnrc_netif_addr_to_str(out, sizeof(out),
                             addr, sizeof(addr)));
}

static void test_ng_netif_addr_from_str__out_too_short(void)
{
    static const char str[] = "05:cd";
    uint8_t out[1];

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_addr_from_str(out, sizeof(out), str));
}

static void test_ng_netif_addr_from_str__omitted_delimitter(void)
{
    static const char str[] = "4567:cd";
    uint8_t out[3];

    TEST_ASSERT_EQUAL_INT(3, gnrc_netif_addr_from_str(out, sizeof(out), str));
    TEST_ASSERT_EQUAL_INT(0x45, out[0]);
    TEST_ASSERT_EQUAL_INT(0x67, out[1]);
    TEST_ASSERT_EQUAL_INT(0xcd, out[2]);
}

static void test_ng_netif_addr_from_str__ill_formated2(void)
{
    static const char str[] = TEST_STRING8;
    uint8_t out[sizeof(str)];

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_addr_from_str(out, sizeof(out), str));
}

static void test_ng_netif_addr_from_str__dash_delimitter(void)
{
    static const char str[] = "05-cd";
    uint8_t out[2];

    TEST_ASSERT_EQUAL_INT(2, gnrc_netif_addr_from_str(out, sizeof(out), str));
    TEST_ASSERT_EQUAL_INT(0x05, out[0]);
    TEST_ASSERT_EQUAL_INT(0xcd, out[1]);
}

static void test_ng_netif_addr_from_str__zero_omitted_back(void)
{
    static const char str[] = "05:c";
    uint8_t out[2];

    TEST_ASSERT_EQUAL_INT(2, gnrc_netif_addr_from_str(out, sizeof(out), str));
    TEST_ASSERT_EQUAL_INT(0x05, out[0]);
    TEST_ASSERT_EQUAL_INT(0x0c, out[1]);
}

static void test_ng_netif_addr_from_str__zero_omitted_front(void)
{
    static const char str[] = "5:cd";
    uint8_t out[2];

    TEST_ASSERT_EQUAL_INT(2, gnrc_netif_addr_from_str(out, sizeof(out), str));
    TEST_ASSERT_EQUAL_INT(0x05, out[0]);
    TEST_ASSERT_EQUAL_INT(0xcd, out[1]);
}

static void test_ng_netif_addr_from_str__ill_trailing_delimitter(void)
{
    static const char str[] = "05:cd:";
    uint8_t out[sizeof(str)];

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_addr_from_str(out, sizeof(out), str));
}

static void test_ng_netif_addr_from_str__ill_leading_delimitter(void)
{
    static const char str[] = ":05:cd";
    uint8_t out[sizeof(str)];

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_addr_from_str(out, sizeof(out), str));
}

static void test_ng_netif_addr_from_str__ill_extra_delimitter(void)
{
    static const char str[] = "05::cd";
    uint8_t out[sizeof(str)];

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_addr_from_str(out, sizeof(out), str));
}

static void test_ng_netif_addr_from_str__success(void)
{
    static const char str[] = "05:cd";
    uint8_t out[2];

    TEST_ASSERT_EQUAL_INT(2, gnrc_netif_addr_from_str(out, sizeof(out), str));
    TEST_ASSERT_EQUAL_INT(0x05, out[0]);
    TEST_ASSERT_EQUAL_INT(0xcd, out[1]);
}

Test *tests_netif_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ng_netif_add__KERNEL_PID_UNDEF),
        new_TestFixture(test_ng_netif_add__memfull),
        new_TestFixture(test_ng_netif_add__success),
        new_TestFixture(test_ng_netif_add__duplicate_entry),
        new_TestFixture(test_ng_netif_remove__KERNEL_PID_UNDEF),
        new_TestFixture(test_ng_netif_remove__not_an_if),
        new_TestFixture(test_ng_netif_remove__success),
        new_TestFixture(test_ng_netif_get__empty),
        new_TestFixture(test_ng_netif_get__success_3_minus_one),
        new_TestFixture(test_ng_netif_get__full),
        new_TestFixture(test_ng_netif_addr_to_str__out_too_short),
        new_TestFixture(test_ng_netif_addr_to_str__success),
        new_TestFixture(test_ng_netif_addr_from_str__out_too_short),
        new_TestFixture(test_ng_netif_addr_from_str__omitted_delimitter),
        new_TestFixture(test_ng_netif_addr_from_str__ill_formated2),
        new_TestFixture(test_ng_netif_addr_from_str__dash_delimitter),
        new_TestFixture(test_ng_netif_addr_from_str__zero_omitted_back),
        new_TestFixture(test_ng_netif_addr_from_str__zero_omitted_front),
        new_TestFixture(test_ng_netif_addr_from_str__ill_trailing_delimitter),
        new_TestFixture(test_ng_netif_addr_from_str__ill_leading_delimitter),
        new_TestFixture(test_ng_netif_addr_from_str__ill_extra_delimitter),
        new_TestFixture(test_ng_netif_addr_from_str__success),
    };

    EMB_UNIT_TESTCALLER(netif_tests, set_up, NULL, fixtures);

    return (Test *)&netif_tests;
}

void tests_netif(void)
{
    TESTS_RUN(tests_netif_tests());
}
/** @} */
