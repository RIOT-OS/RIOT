/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>

#include "embUnit.h"

#include "clist.h"

#include "tests-core.h"

#define TEST_CLIST_LEN    (8)

static list_node_t tests_clist_buf[TEST_CLIST_LEN];
static list_node_t test_clist;

static void set_up(void)
{
    memset(tests_clist_buf, 0, sizeof(tests_clist_buf));
}

static void test_clist_add_one(void)
{
    list_node_t *elem = &(tests_clist_buf[0]);
    list_node_t *list = &test_clist;
    list->next = NULL;

    clist_insert(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == list->next);
}

static void test_clist_add_two(void)
{
    list_node_t *list = &test_clist;
    list->next = NULL;

    list_node_t *elem = &(tests_clist_buf[1]);

    test_clist_add_one();

    clist_insert(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next == elem);
    TEST_ASSERT(list->next->next->next == list->next);
}

static void test_clist_remove_head(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_two();

    clist_remove_head(list);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == &tests_clist_buf[1]);
}

static void test_clist_remove_two(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_two();

    clist_remove_head(list);
    clist_remove_head(list);

    TEST_ASSERT_NULL(list->next);
}

static void test_clist_advance(void)
{
    list_node_t *list = &test_clist;
    list->next = NULL;

    test_clist_add_two();

    clist_advance(list);

    TEST_ASSERT(list->next->next == &tests_clist_buf[1]);

    clist_advance(list);

    TEST_ASSERT(list->next->next == &tests_clist_buf[0]);
}

Test *tests_core_clist_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_clist_add_one),
        new_TestFixture(test_clist_add_two),
        new_TestFixture(test_clist_remove_head),
        new_TestFixture(test_clist_remove_two),
        new_TestFixture(test_clist_advance),
    };

    EMB_UNIT_TESTCALLER(core_clist_tests, set_up, NULL,
                        fixtures);

    return (Test *)&core_clist_tests;
}
