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

#include "embUnit.h"

#include "net/ng_netreg.h"
#include "net/ng_nettype.h"

#include "tests-netreg.h"

static void set_up(void)
{
    ng_netreg_init();
}

static void test_netreg_register__inval_undef(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, ng_netreg_register(NG_NETTYPE_UNDEF, TEST_UINT16,
                          TEST_UINT8));
}

static void test_netreg_register__inval_numof(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, ng_netreg_register(NG_NETTYPE_NUMOF, TEST_UINT16,
                          TEST_UINT8));
}

static void test_netreg_register__memfull(void)
{
    for (int i = 0; i < NG_NETREG_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, ng_netreg_register(NG_NETTYPE_TEST, TEST_UINT16 + i,
                              TEST_UINT8));
    }

    TEST_ASSERT_EQUAL_INT(-ENOMEM, ng_netreg_register(NG_NETTYPE_TEST, TEST_UINT16,
                          TEST_UINT8 + 1));
}

static void test_netreg_register__success(void)
{
    ng_netreg_entry_t *entry = ng_netreg_lookup(NG_NETTYPE_TEST, TEST_UINT16);

    TEST_ASSERT_NULL(entry);

    TEST_ASSERT_EQUAL_INT(0, ng_netreg_register(NG_NETTYPE_TEST, TEST_UINT16,
                          TEST_UINT8));
    TEST_ASSERT_NOT_NULL((entry = ng_netreg_lookup(NG_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, entry->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, entry->pid);
    TEST_ASSERT_NULL((ng_netreg_getnext(entry)));
}

static void test_netreg_register__double_registration(void)
{
    ng_netreg_entry_t *entry = NULL;

    for (int i = 0; i < NG_NETREG_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, ng_netreg_register(NG_NETTYPE_TEST, TEST_UINT16,
                              TEST_UINT8));
    }

    TEST_ASSERT_NOT_NULL((entry = ng_netreg_lookup(NG_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_NULL((ng_netreg_getnext(entry)));
}

void test_netreg_unregister__success(void)
{
    test_netreg_register__success();
    ng_netreg_unregister(NG_NETTYPE_TEST, TEST_UINT16, TEST_UINT8);
    TEST_ASSERT_NULL(ng_netreg_lookup(NG_NETTYPE_TEST, TEST_UINT16));
}

void test_netreg_unregister__success2(void)
{
    ng_netreg_entry_t *entry = NULL;

    test_netreg_register__success();
    TEST_ASSERT_EQUAL_INT(0, ng_netreg_register(NG_NETTYPE_TEST, TEST_UINT16,
                          TEST_UINT8 + 1));
    ng_netreg_unregister(NG_NETTYPE_TEST, TEST_UINT16, TEST_UINT8);
    TEST_ASSERT_NOT_NULL((entry = ng_netreg_lookup(NG_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, entry->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 + 1, entry->pid);
    ng_netreg_unregister(NG_NETTYPE_TEST, TEST_UINT16 + 1, TEST_UINT8 + 1);
}

void test_netreg_lookup__wrong_type_undef(void)
{
    test_netreg_register__success();
    TEST_ASSERT_NULL(ng_netreg_lookup(NG_NETTYPE_UNDEF, TEST_UINT16));
}

void test_netreg_lookup__wrong_type_numof(void)
{
    test_netreg_register__success();
    TEST_ASSERT_NULL(ng_netreg_lookup(NG_NETTYPE_NUMOF, TEST_UINT16));
}

void test_netreg_num__empty(void)
{
    TEST_ASSERT_EQUAL_INT(0, ng_netreg_num(NG_NETTYPE_TEST, TEST_UINT16));
    TEST_ASSERT_EQUAL_INT(0, ng_netreg_num(NG_NETTYPE_TEST, TEST_UINT16 + 1));
}

void test_netreg_num__wrong_type_undef(void)
{
    test_netreg_register__success();
    TEST_ASSERT_EQUAL_INT(0, ng_netreg_num(NG_NETTYPE_UNDEF, TEST_UINT16));
}

void test_netreg_num__wrong_type_numof(void)
{
    test_netreg_register__success();
    TEST_ASSERT_EQUAL_INT(0, ng_netreg_num(NG_NETTYPE_NUMOF, TEST_UINT16));
}

void test_netreg_num__2_entries(void)
{
    test_netreg_register__success();
    TEST_ASSERT_EQUAL_INT(1, ng_netreg_num(NG_NETTYPE_TEST, TEST_UINT16));
    TEST_ASSERT_EQUAL_INT(0, ng_netreg_register(NG_NETTYPE_TEST, TEST_UINT16,
                          TEST_UINT8 + 1));
    TEST_ASSERT_EQUAL_INT(2, ng_netreg_num(NG_NETTYPE_TEST, TEST_UINT16));
}

void test_netreg_getnext__NULL(void)
{
    test_netreg_register__success();
    TEST_ASSERT_NULL(ng_netreg_getnext(NULL));
}

void test_netreg_getnext__2_entries(void)
{
    ng_netreg_entry_t *entry = NULL;

    test_netreg_num__2_entries();
    TEST_ASSERT_NOT_NULL((entry = ng_netreg_lookup(NG_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_NOT_NULL(ng_netreg_getnext(entry));
}

Test *tests_netreg_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_netreg_register__inval_undef),
        new_TestFixture(test_netreg_register__inval_numof),
        new_TestFixture(test_netreg_register__memfull),
        new_TestFixture(test_netreg_register__success),
        new_TestFixture(test_netreg_register__double_registration),
        new_TestFixture(test_netreg_unregister__success),
        new_TestFixture(test_netreg_unregister__success2),
        new_TestFixture(test_netreg_lookup__wrong_type_undef),
        new_TestFixture(test_netreg_lookup__wrong_type_numof),
        new_TestFixture(test_netreg_num__empty),
        new_TestFixture(test_netreg_num__wrong_type_undef),
        new_TestFixture(test_netreg_num__wrong_type_numof),
        new_TestFixture(test_netreg_num__2_entries),
        new_TestFixture(test_netreg_getnext__NULL),
        new_TestFixture(test_netreg_getnext__2_entries),
    };

    EMB_UNIT_TESTCALLER(netreg_tests, set_up, NULL, fixtures);

    return (Test *)&netreg_tests;
}

void tests_netreg(void)
{
    TESTS_RUN(tests_netreg_tests());
}
/** @} */
