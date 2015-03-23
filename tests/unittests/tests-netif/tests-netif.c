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
 * @file    tests-netif.c
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"
#include "kernel_types.h"
#include "net/ng_netif.h"

#include "unittests-constants.h"
#include "tests-netif.h"

static void set_up(void)
{
    ng_netif_init();
}

static void test_ng_netif_add__KERNEL_PID_UNDEF(void)
{
    size_t size = TEST_UINT8;

    TEST_ASSERT_EQUAL_INT(0, ng_netif_add(KERNEL_PID_UNDEF));
    ng_netif_get(&size);
    TEST_ASSERT_EQUAL_INT(0, size);
}

static void test_ng_netif_add__memfull(void)
{
    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, ng_netif_add(TEST_UINT8 + i));
    }

    TEST_ASSERT_EQUAL_INT(-ENOMEM, ng_netif_add(TEST_UINT8 - 1));
}

static void test_ng_netif_add__success(void)
{
    size_t size = TEST_UINT8;
    kernel_pid_t *res = NULL;

    TEST_ASSERT_EQUAL_INT(0, ng_netif_add(TEST_UINT8));

    res = ng_netif_get(&size);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res[0]);
}

static void test_ng_netif_add__duplicate_entry(void)
{
    size_t size = TEST_UINT8;
    kernel_pid_t *res = NULL;

    for (int i = 0; i < NG_NETIF_NUMOF + 4; i++) {
        TEST_ASSERT_EQUAL_INT(0, ng_netif_add(TEST_UINT8));
    }

    res = ng_netif_get(&size);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res[0]);
}

static void test_ng_netif_remove__KERNEL_PID_UNDEF(void)
{
    size_t size = TEST_UINT8;
    kernel_pid_t *res = NULL;

    test_ng_netif_add__success();

    ng_netif_remove(KERNEL_PID_UNDEF);

    res = ng_netif_get(&size);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res[0]);
}

static void test_ng_netif_remove__not_an_if(void)
{
    size_t size = TEST_UINT8;
    kernel_pid_t *res = NULL;

    test_ng_netif_add__success();

    ng_netif_remove(TEST_UINT8 + 1);

    res = ng_netif_get(&size);
    TEST_ASSERT_EQUAL_INT(1, size);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res[0]);
}

static void test_ng_netif_remove__success(void)
{
    size_t size = TEST_UINT8;

    test_ng_netif_add__success();

    ng_netif_remove(TEST_UINT8);

    ng_netif_get(&size);
    TEST_ASSERT_EQUAL_INT(0, size);
}

static void test_ng_netif_get__empty(void)
{
    size_t size = TEST_UINT8;

    ng_netif_get(&size);
    TEST_ASSERT_EQUAL_INT(0, size);
}

/* takes one out of the middle of the netif list and checks if all interfaces
 * are gotten regardless */
static void test_ng_netif_get__success_3_minus_one(void)
{
    size_t size = TEST_UINT8;
    kernel_pid_t *ifs;
    int count = 0;

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT(0, ng_netif_add(TEST_UINT8 + i));
    }

    ng_netif_remove(TEST_UINT8 + 1);

    ifs = ng_netif_get(&size);
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
    size_t size = TEST_UINT8;

    for (int i = 0; i < NG_NETIF_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, ng_netif_add(TEST_UINT8 + i));
    }

    TEST_ASSERT_NOT_NULL(ng_netif_get(&size));
    TEST_ASSERT_EQUAL_INT(NG_NETIF_NUMOF, size);
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
    };

    EMB_UNIT_TESTCALLER(netif_tests, set_up, NULL, fixtures);

    return (Test *)&netif_tests;
}

void tests_netif(void)
{
    TESTS_RUN(tests_netif_tests());
}
/** @} */
