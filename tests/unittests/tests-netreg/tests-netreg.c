/*
 * SPDX-FileCopyrightText: 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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
    gnrc_netreg_acquire_shared();
    gnrc_netreg_entry_t *res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16);

    TEST_ASSERT_NULL(res);
    gnrc_netreg_release_shared();

    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, res->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res->target.pid);
    TEST_ASSERT_NULL((gnrc_netreg_getnext(res)));
    gnrc_netreg_release_shared();
}

void test_netreg_unregister__success(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NOT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
    gnrc_netreg_release_shared();

    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[0]);

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_unregister__success2(void)
{
    gnrc_netreg_entry_t *res = NULL;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[1]));
    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[0]);

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, res->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8 + 1, res->target.pid);
    gnrc_netreg_release_shared();

    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[1]);

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_unregister__success3(void)
{
    gnrc_netreg_entry_t *res = NULL;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[1]));
    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[1]);

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_EQUAL_INT(TEST_UINT16, res->demux_ctx);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res->target.pid);
    gnrc_netreg_release_shared();

    gnrc_netreg_unregister(GNRC_NETTYPE_TEST, &entries[0]);

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_lookup__wrong_type_undef(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_UNDEF, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_lookup__wrong_type_numof(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NULL(gnrc_netreg_lookup(GNRC_NETTYPE_NUMOF, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_num__empty(void)
{
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_num(GNRC_NETTYPE_TEST, TEST_UINT16));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_num(GNRC_NETTYPE_TEST, TEST_UINT16 + 1));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_num(GNRC_NETTYPE_TEST, GNRC_NETREG_DEMUX_CTX_ALL));
    gnrc_netreg_release_shared();
}

void test_netreg_num__wrong_type_undef(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_num(GNRC_NETTYPE_UNDEF, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_num__wrong_type_numof(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_num(GNRC_NETTYPE_NUMOF, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_num__2_entries(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_EQUAL_INT(1, gnrc_netreg_num(GNRC_NETTYPE_TEST, TEST_UINT16));
    gnrc_netreg_release_shared();

    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[1]));

    gnrc_netreg_acquire_shared();
    TEST_ASSERT_EQUAL_INT(2, gnrc_netreg_num(GNRC_NETTYPE_TEST, TEST_UINT16));
    gnrc_netreg_release_shared();
}

void test_netreg_getnext__NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netreg_register(GNRC_NETTYPE_TEST, &entries[0]));
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NULL(gnrc_netreg_getnext(NULL));
    gnrc_netreg_release_shared();
}

void test_netreg_getnext__2_entries(void)
{
    gnrc_netreg_entry_t *res = NULL;

    test_netreg_num__2_entries();
    gnrc_netreg_acquire_shared();
    TEST_ASSERT_NOT_NULL((res = gnrc_netreg_lookup(GNRC_NETTYPE_TEST, TEST_UINT16)));
    TEST_ASSERT_NOT_NULL(gnrc_netreg_getnext(res));
    gnrc_netreg_release_shared();
}

Test *tests_netreg_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_netreg_register__inval_numof),
        new_TestFixture(test_netreg_register__success),
        new_TestFixture(test_netreg_unregister__success),
        new_TestFixture(test_netreg_unregister__success2),
        new_TestFixture(test_netreg_unregister__success3),
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
