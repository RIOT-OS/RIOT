/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <string.h>

#include "embUnit/embUnit.h"

#include "queue.h"

#include "tests-core.h"

#define Q_LEN (4)

static queue_t q;
static queue_node_t qe[Q_LEN];

static void set_up(void)
{
    q.first = NULL;
    memset(qe, 0, sizeof(qe));
}

static void test_queue_remove_head_empty(void)
{
    queue_t *root = &q;
    queue_node_t *res;

    res = queue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_queue_remove_head_one(void)
{
    queue_t *root = &q;
    queue_node_t *elem = &(qe[1]), *res;

    elem->data = 62801;

    queue_priority_add(root, elem);

    res = queue_remove_head(root);

    TEST_ASSERT(res == elem);
    TEST_ASSERT_EQUAL_INT(62801, res->data);

    res = queue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_queue_priority_add_one(void)
{
    queue_t *root = &q;
    queue_node_t *elem = &(qe[1]);

    elem->data = 7317;
    elem->priority = 713643658;

    queue_priority_add(root, elem);

    TEST_ASSERT(root->first == elem);
    TEST_ASSERT_EQUAL_INT(7317, root->first->data);
    TEST_ASSERT_EQUAL_INT(713643658, root->first->priority);

    TEST_ASSERT_NULL(root->first->next);
}

static void test_queue_priority_add_two_equal(void)
{
    queue_t *root = &q;
    queue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]);

    elem1->data = 27088;
    elem1->priority = 14202;

    elem2->data = 4356;
    elem2->priority = 14202;

    queue_priority_add(root, elem1);
    queue_priority_add(root, elem2);

    TEST_ASSERT(root->first == elem1);
    TEST_ASSERT_EQUAL_INT(27088, root->first->data);
    TEST_ASSERT_EQUAL_INT(14202, root->first->priority);

    TEST_ASSERT(root->first->next == elem2);
    TEST_ASSERT_EQUAL_INT(4356, root->first->next->data);
    TEST_ASSERT_EQUAL_INT(14202, root->first->next->priority);

    TEST_ASSERT_NULL(root->first->next->next);
}

static void test_queue_priority_add_two_distinct(void)
{
    queue_t *root = &q;
    queue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]);

    elem1->data = 46421;
    elem1->priority = 4567;

    elem2->data = 43088;
    elem2->priority = 1234;

    queue_priority_add(root, elem1);
    queue_priority_add(root, elem2);

    TEST_ASSERT(root->first == elem2);
    TEST_ASSERT_EQUAL_INT(43088, root->first->data);
    TEST_ASSERT_EQUAL_INT(1234, root->first->priority);

    TEST_ASSERT(root->first->next == elem1);
    TEST_ASSERT_EQUAL_INT(46421, root->first->next->data);
    TEST_ASSERT_EQUAL_INT(4567, root->first->next->priority);

    TEST_ASSERT_NULL(root->first->next->next);
}

static void test_queue_remove_one(void)
{
    queue_t *root = &q;
    queue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]), *elem3 = &(qe[3]);

    queue_priority_add(root, elem1);
    queue_priority_add(root, elem2);
    queue_priority_add(root, elem3);
    queue_remove(root, elem2);

    TEST_ASSERT(root->first == elem1);
    TEST_ASSERT(root->first->next == elem3);
    TEST_ASSERT_NULL(root->first->next->next);
}

Test *tests_core_queue_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_queue_remove_head_empty),
        new_TestFixture(test_queue_remove_head_one),
        new_TestFixture(test_queue_priority_add_one),
        new_TestFixture(test_queue_priority_add_two_equal),
        new_TestFixture(test_queue_priority_add_two_distinct),
        new_TestFixture(test_queue_remove_one),
    };

    EMB_UNIT_TESTCALLER(core_queue_tests, set_up, NULL,
                        fixtures);

    return (Test *)&core_queue_tests;
}
