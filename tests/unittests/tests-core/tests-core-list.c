/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>

#include "embUnit.h"

#include "list.h"

#include "tests-core.h"

#define TEST_LIST_LEN    (8)

static list_node_t test_list_buf[TEST_LIST_LEN];
static list_node_t test_list;

static void set_up(void)
{
    memset(test_list_buf, 0, sizeof(test_list_buf));
    test_list.next = NULL;
}

static void test_list_add(void)
{
    list_node_t *elem = &(test_list_buf[0]);
    list_node_t *list = &test_list;

    list_add(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next == &test_list_buf[0]);
    TEST_ASSERT_NULL(list->next->next);
}

static void test_list_add_two(void)
{
    list_node_t *list = &test_list;

    list_node_t *elem = &(test_list_buf[1]);

    test_list_add();

    list_add(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next == elem);
    TEST_ASSERT(list->next->next == &test_list_buf[0]);
    TEST_ASSERT_NULL(list->next->next->next);
}

static void test_list_add_three(void)
{
    list_node_t *list = &test_list;

    for (int i = 0; i < 3; i++) {
        list_add(list, &(test_list_buf[i]));
    }

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next == &(test_list_buf[2]));
    TEST_ASSERT(list->next->next == &(test_list_buf[1]));
    TEST_ASSERT(list->next->next->next == &(test_list_buf[0]));
    TEST_ASSERT_NULL(list->next->next->next->next);
}

static void test_list_remove_head(void)
{
    list_node_t *list = &test_list;
    list_node_t *elem;

    test_list_add_two();

    elem = list_remove_head(list);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(elem == &test_list_buf[1]);
    TEST_ASSERT(list->next == &test_list_buf[0]);
    TEST_ASSERT_NULL(list->next->next);
}

static void test_list_remove_head_two(void)
{
    list_node_t *list = &test_list;
    list_node_t *elem;

    test_list_add_two();

    elem = list_remove_head(list);

    TEST_ASSERT(elem == &test_list_buf[1]);

    elem = list_remove_head(list);

    TEST_ASSERT_NULL(list->next);
    TEST_ASSERT(elem == &test_list_buf[0]);
}

static void test_list_remove_head_empty(void)
{
    list_node_t *list = &test_list;
    list_node_t *elem;

    elem = list_remove_head(list);

    TEST_ASSERT_NULL(list->next);
    TEST_ASSERT_NULL(elem);
}

static void test_list_remove(void)
{
    list_node_t *list = &test_list;
    list_node_t *elem = &test_list_buf[0];

    test_list_add_two();

    elem = list_remove(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(elem == &test_list_buf[0]);
    TEST_ASSERT(list->next == &test_list_buf[1]);
    TEST_ASSERT_NULL(list->next->next);
}

static void test_list_remove_two(void)
{
    list_node_t *list = &test_list;
    list_node_t *elem1 = &test_list_buf[0];
    list_node_t *elem2 = &test_list_buf[2];

    test_list_add_three();

    elem1 = list_remove(list, elem1);
    elem2 = list_remove(list, elem2);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(elem1 == &test_list_buf[0]);
    TEST_ASSERT(elem2 == &test_list_buf[2]);
    TEST_ASSERT(list->next == &test_list_buf[1]);
    TEST_ASSERT_NULL(list->next->next);
}

static void test_list_remove_empty(void)
{
    list_node_t *list = &test_list;
    list_node_t *elem;

    elem = list_remove(list, &test_list_buf[0]);

    TEST_ASSERT_NULL(list->next);
    TEST_ASSERT_NULL(elem);
}

static void test_list_remove_unknown(void)
{
    list_node_t *list = &test_list;
    list_node_t *elem = &test_list_buf[3];

    test_list_add_two();

    elem = list_remove(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT_NOT_NULL(list->next->next);
    TEST_ASSERT_NULL(list->next->next->next);
    TEST_ASSERT_NULL(elem);
}

Test *tests_core_list_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_list_add),
        new_TestFixture(test_list_add_two),
        new_TestFixture(test_list_add_three),
        new_TestFixture(test_list_remove_head),
        new_TestFixture(test_list_remove_head_two),
        new_TestFixture(test_list_remove_head_empty),
        new_TestFixture(test_list_remove),
        new_TestFixture(test_list_remove_two),
        new_TestFixture(test_list_remove_empty),
        new_TestFixture(test_list_remove_unknown),
    };

    EMB_UNIT_TESTCALLER(core_list_tests, set_up, NULL,
                        fixtures);

    return (Test *)&core_list_tests;
}
