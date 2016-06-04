/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include <stdlib.h>
#include <string.h>

#include "embUnit.h"

#include "net/netif.h"

#include "unittests-constants.h"
#include "tests-netif.h"

#define NETIFS  (3U)

static netif_t test_netifs[NETIFS];
static unsigned int netif = 0;

static void check_netif(void)
{
    TEST_ASSERT(netif <= NETIFS);
}

static void test_netif_add__success(void)
{
    strncpy(test_netifs[netif].name, TEST_STRING4, NETIF_NAME_LEN);
    TEST_ASSERT_EQUAL_INT(0, netif_add(&test_netifs[netif]));
    netif++;
}

static void test_netif_add__duplicate(void)
{
    strncpy(test_netifs[netif].name, TEST_STRING4, NETIF_NAME_LEN);
    TEST_ASSERT_MESSAGE(netif_add(&test_netifs[netif]) == -EEXIST,
                        "res != -EEXIST");
}

static void test_netif_add__non_duplicate(void)
{
    strncpy(test_netifs[netif].name, TEST_STRING8, NETIF_NAME_LEN);
    TEST_ASSERT_EQUAL_INT(0, netif_add(&test_netifs[netif]));
    netif++;
}

static void test_netif_get_id(void)
{
    TEST_ASSERT(netif > 0);
    TEST_ASSERT_EQUAL_INT(netif, netif_get_id(&test_netifs[netif - 1]));
}

static void test_netif_get_id__ENOENT(void)
{
    TEST_ASSERT_MESSAGE(netif_get_id(&test_netifs[netif + 1]) == -ENOENT,
                        "res != -ENOENT");
}

static void test_netif_get_id__NULL(void)
{
    TEST_ASSERT_MESSAGE(netif_get_id(NULL) == -ENOENT,
                        "res != -ENOENT");
}

static void test_netif_by_id(void)
{
    for (unsigned int i = 1; i <= netif; i++) {
        TEST_ASSERT(netif_by_id(i) == &test_netifs[i - 1]);
    }
    TEST_ASSERT_NULL(netif_by_id(netif + 1));
}

static void test_netif_by_name(void)
{
    for (unsigned int i = 0; i < netif; i++) {
        TEST_ASSERT(netif_by_name(test_netifs[i].name) == &test_netifs[i]);
    }
    TEST_ASSERT_NULL(netif_by_name(TEST_STRING64));
}

static void test_netif_set_name(void)
{
    TEST_ASSERT_EQUAL_INT(0, netif_set_name(&test_netifs[0],
                                            test_netifs[0].name));
    TEST_ASSERT_EQUAL_INT(0, netif_set_name(&test_netifs[0], TEST_STRING64));
}

static void test_netif_set_name__duplicate(void)
{
    TEST_ASSERT(netif >= 1);
    TEST_ASSERT_MESSAGE(netif_set_name(&test_netifs[0],
                                       test_netifs[1].name) == -EEXIST,
                        "res != -EEXIST");
    TEST_ASSERT_MESSAGE(netif_set_name(&test_netifs[1],
                                       test_netifs[0].name) == -EEXIST,
                        "res != -EEXIST");
}

Test *tests_netif_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_netif_add__success),
        new_TestFixture(test_netif_add__duplicate),
        new_TestFixture(test_netif_add__non_duplicate),
        new_TestFixture(test_netif_get_id),
        new_TestFixture(test_netif_get_id__ENOENT),
        new_TestFixture(test_netif_get_id__NULL),
        new_TestFixture(test_netif_by_id),
        new_TestFixture(test_netif_by_name),
        new_TestFixture(test_netif_set_name),
        new_TestFixture(test_netif_set_name__duplicate),
    };

    EMB_UNIT_TESTCALLER(netif_tests, NULL, check_netif, fixtures);

    return (Test *)&netif_tests;
}

void tests_netif(void)
{
    TESTS_RUN(tests_netif_tests());
}
/** @} */
