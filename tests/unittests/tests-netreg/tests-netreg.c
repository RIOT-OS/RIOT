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

#include "net/gnrc/netreg.h"
#include "net/gnrc/nettype.h"

#include "unittests-constants.h"
#include "tests-netreg.h"

static gnrc_netreg_entry_t entries[] = {
    GNRC_NETREG_ENTRY_INIT_PID(TEST_UINT16, TEST_UINT8),
    GNRC_NETREG_ENTRY_INIT_PID(TEST_UINT16, TEST_UINT8 + 1)
};

static void set_up(void)
{
    gnrc_netreg_init();
}

static void test_netreg_register__inval_numof(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_netreg_register(GNRC_NETTYPE_NUMOF, &entries[0]));
}

static void test_netreg_register__success(void)
{
    gnrc_netreg_entry_t *res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16);

    TEST_ASSERT_NULL(res);

    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, res->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res->target.pid);
    TEST_ASSERT_NULL((gnrc_netreg_getnext(res)));
}

void test_netreg_unregister__success(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_NOT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[0]);
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
}

void test_netreg_unregister__success2(void)
{
    gnrc_netreg_entry_t *res = NULL;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[1]));
    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[0]);
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, res->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 + 1, res->target.pid);
    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[1]);
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
}

void test_netreg_unregister__success3(void)
{
    gnrc_netreg_entry_t *res = NULL;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[1]));
    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[1]);
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, res->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res->target.pid);
    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[0]);
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
}

void test_netreg_lookup__empty(void)
{
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16 + 1));
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, GNRC_NETREG_DEMUX_CTX_ALL));
}

void test_netreg_lookup__wrong_type_undef(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_UNDEF, TEST_UINT16));
}

void test_netreg_lookup__wrong_type_numof(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_NUMOF, TEST_UINT16));
}

void test_netreg_lookup__2_entries(void)
{
    gnrc_netreg_entry_t *res = NULL;
    /* add first entry, first lookup != NULL; second == NULL */
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_NULL((res = gnrc_netreg_getnext(res)));
    /* add second entry, both lookups != NULL */
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[1]));
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_getnext(res)));
}

void test_netreg_getnext__NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_NULL(gnrc_netreg_getnext(NULL));
}

void test_netreg_getnext__2_entries(void)
{
    gnrc_netreg_entry_t *res = NULL;

    test_netreg_lookup__2_entries();
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_NOT_NULL(gnrc_netreg_getnext(res));
}

Test *tests_netreg_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_netreg_register__inval_numof),
        new_TestFixture(test_netreg_register__success),
        new_TestFixture(test_netreg_unregister__success),
        new_TestFixture(test_netreg_unregister__success2),
        new_TestFixture(test_netreg_unregister__success3),
        new_TestFixture(test_netreg_lookup__empty),
        new_TestFixture(test_netreg_lookup__wrong_type_undef),
        new_TestFixture(test_netreg_lookup__wrong_type_numof),
        new_TestFixture(test_netreg_lookup__2_entries),
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
