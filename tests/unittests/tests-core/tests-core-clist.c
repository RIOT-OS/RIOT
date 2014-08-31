/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>

#include "embUnit/embUnit.h"

#include "clist.h"

#include "tests-core.h"

#define TEST_CLIST_LEN    (8)

static clist_node_t tests_clist_buf[TEST_CLIST_LEN];

static void set_up(void)
{
    memset(tests_clist_buf, 0, sizeof(tests_clist_buf));
}

static void test_clist_add_one(void)
{
    clist_node_t *list = NULL, *elem = &(tests_clist_buf[0]);

    clist_add(&list, elem);

    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT(list->next == list);
}

static void test_clist_add_two(void)
{
    clist_node_t *list = &(tests_clist_buf[0]), *elem = &(tests_clist_buf[1]);

    test_clist_add_one();

    clist_add(&list, elem);

    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT(list->next == elem);
    TEST_ASSERT(list->next->next == list);
}

static void test_clist_remove_one(void)
{
    clist_node_t *list = &(tests_clist_buf[0]), *elem = &(tests_clist_buf[1]);

    test_clist_add_two();

    clist_remove(&list, elem);

    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT(list->next == list);
}

static void test_clist_remove_two(void)
{
    clist_node_t *list = &(tests_clist_buf[0]), *elem = &(tests_clist_buf[1]);

    test_clist_add_two();

    clist_remove(&list, elem);
    clist_remove(&list, list);

    TEST_ASSERT_NULL(list);
}

static void test_clist_advance(void)
{
    clist_node_t *list = &(tests_clist_buf[0]);

    test_clist_add_two();

    clist_advance(&list);

    TEST_ASSERT(list == &(tests_clist_buf[1]));

    clist_advance(&list);

    TEST_ASSERT(list == &(tests_clist_buf[0]));
}

Test *tests_core_clist_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_clist_add_one),
        new_TestFixture(test_clist_add_two),
        new_TestFixture(test_clist_remove_one),
        new_TestFixture(test_clist_remove_two),
        new_TestFixture(test_clist_advance),
    };

    EMB_UNIT_TESTCALLER(core_clist_tests, set_up, NULL,
                        fixtures);

    return (Test *)&core_clist_tests;
}
