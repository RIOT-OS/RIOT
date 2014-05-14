/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */
#include <string.h>

#include "embUnit/embUnit.h"

#include "queue.h"

#include "tests-core.h"

#define Q_LEN (4)

static queue_node_t q[Q_LEN];

static void set_up(void)
{
    memset(q, 0, sizeof(q));
}

static void test_queue_remove_head_empty(void)
{
    queue_node_t *root = &(q[0]), *res;

    res = queue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_queue_remove_head_one(void)
{
    queue_node_t *root = &(q[0]), *elem = &(q[1]), *res;

    elem->data = 62801;

    queue_add_head(root, elem);

    res = queue_remove_head(root);

    TEST_ASSERT(res == elem);
    TEST_ASSERT_EQUAL_INT(62801, res->data);

    res = queue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_queue_add_head_one(void)
{
    queue_node_t *root = &(q[0]), *elem = &(q[1]);

    elem->data = 44011;

    queue_add_head(root, elem);

    TEST_ASSERT(root->next == elem);
    TEST_ASSERT_EQUAL_INT(44011, root->next->data);

    TEST_ASSERT_NULL(root->next->next);
}

static void test_queue_add_head_two(void)
{
    queue_node_t *root = &(q[0]), *elem1 = &(q[1]), *elem2 = &(q[2]);

    elem1->data = 25303;
    elem2->data = 64960;

    queue_add_head(root, elem1);
    queue_add_head(root, elem2);

    TEST_ASSERT(root->next == elem2);
    TEST_ASSERT_EQUAL_INT(64960, root->next->data);

    TEST_ASSERT(root->next->next == elem1);
    TEST_ASSERT_EQUAL_INT(25303, root->next->next->data);

    TEST_ASSERT_NULL(root->next->next->next);
}

static void test_queue_add_tail_one(void)
{
    queue_node_t *root = &(q[0]), *elem = &(q[1]);

    elem->data = 33893;

    queue_add_tail(root, elem);

    TEST_ASSERT(root->next == elem);
    TEST_ASSERT_EQUAL_INT(33893, root->next->data);

    TEST_ASSERT_NULL(root->next->next);
}

static void test_queue_add_tail_two(void)
{
    queue_node_t *root = &(q[0]), *elem1 = &(q[1]), *elem2 = &(q[2]);

    elem1->data = 9084;
    elem2->data = 57068;

    queue_add_tail(root, elem1);
    queue_add_tail(root, elem2);

    TEST_ASSERT(root->next == elem1);
    TEST_ASSERT_EQUAL_INT(9084, root->next->data);

    TEST_ASSERT(root->next->next == elem2);
    TEST_ASSERT_EQUAL_INT(57068, root->next->next->data);

    TEST_ASSERT_NULL(root->next->next->next);
}

static void test_queue_priority_add_one(void)
{
    queue_node_t *root = &(q[0]), *elem = &(q[1]);

    elem->data = 7317;
    elem->priority = 713643658;

    queue_priority_add(root, elem);

    TEST_ASSERT(root->next == elem);
    TEST_ASSERT_EQUAL_INT(7317, root->next->data);
    TEST_ASSERT_EQUAL_INT(713643658, root->next->priority);

    TEST_ASSERT_NULL(root->next->next);
}

static void test_queue_priority_add_two_equal(void)
{
    queue_node_t *root = &(q[0]), *elem1 = &(q[1]), *elem2 = &(q[2]);

    elem1->data = 27088;
    elem1->priority = 14202;

    elem2->data = 4356;
    elem2->priority = 14202;

    queue_priority_add(root, elem1);
    queue_priority_add(root, elem2);

    TEST_ASSERT(root->next == elem1);
    TEST_ASSERT_EQUAL_INT(27088, root->next->data);
    TEST_ASSERT_EQUAL_INT(14202, root->next->priority);

    TEST_ASSERT(root->next->next == elem2);
    TEST_ASSERT_EQUAL_INT(4356, root->next->next->data);
    TEST_ASSERT_EQUAL_INT(14202, root->next->next->priority);

    TEST_ASSERT_NULL(root->next->next->next);
}

static void test_queue_priority_add_two_distinct(void)
{
    queue_node_t *root = &(q[0]), *elem1 = &(q[1]), *elem2 = &(q[2]);

    elem1->data = 46421;
    elem1->priority = 4567;

    elem2->data = 43088;
    elem2->priority = 1234;

    queue_priority_add(root, elem1);
    queue_priority_add(root, elem2);

    TEST_ASSERT(root->next == elem2);
    TEST_ASSERT_EQUAL_INT(43088, root->next->data);
    TEST_ASSERT_EQUAL_INT(1234, root->next->priority);

    TEST_ASSERT(root->next->next == elem1);
    TEST_ASSERT_EQUAL_INT(46421, root->next->next->data);
    TEST_ASSERT_EQUAL_INT(4567, root->next->next->priority);

    TEST_ASSERT_NULL(root->next->next->next);
}

static int generic_compare(queue_node_t *a, queue_node_t *b)
{
    return (b->priority) - (a->priority);
}

static void test_queue_priority_add_generic_two_equal(void)
{
    queue_node_t *root = &(q[0]), *elem1 = &(q[1]), *elem2 = &(q[2]);

    elem1->data = 43804;
    elem1->priority = 34572;

    elem2->data = 64016;
    elem2->priority = 34572;

    queue_priority_add_generic(root, elem1, generic_compare);
    queue_priority_add_generic(root, elem2, generic_compare);

    TEST_ASSERT(root->next == elem1);
    TEST_ASSERT_EQUAL_INT(43804, root->next->data);
    TEST_ASSERT_EQUAL_INT(34572, root->next->priority);

    TEST_ASSERT(root->next->next == elem2);
    TEST_ASSERT_EQUAL_INT(64016, root->next->next->data);
    TEST_ASSERT_EQUAL_INT(34572, root->next->next->priority);

    TEST_ASSERT_NULL(root->next->next->next);
}

static void test_queue_priority_add_generic_two_distinct(void)
{
    queue_node_t *root = &(q[0]), *elem1 = &(q[1]), *elem2 = &(q[2]);

    elem1->data = 39152;
    elem1->priority = 45394;

    elem2->data = 54496;
    elem2->priority = 56834;

    queue_priority_add_generic(root, elem1, generic_compare);
    queue_priority_add_generic(root, elem2, generic_compare);

    TEST_ASSERT(root->next == elem1);
    TEST_ASSERT_EQUAL_INT(39152, root->next->data);
    TEST_ASSERT_EQUAL_INT(45394, root->next->priority);

    TEST_ASSERT(root->next->next == elem2);
    TEST_ASSERT_EQUAL_INT(54496, root->next->next->data);
    TEST_ASSERT_EQUAL_INT(56834, root->next->next->priority);

    TEST_ASSERT_NULL(root->next->next->next);
}

static void test_queue_remove_one(void)
{
    queue_node_t *root = &(q[0]), *elem1 = &(q[1]), *elem2 = &(q[2]);
    queue_node_t *elem3 = &(q[3]);

    queue_add_head(root, elem1);
    queue_add_head(root, elem2);
    queue_add_head(root, elem3);
    queue_remove(root, elem2);

    TEST_ASSERT(root->next == elem3);
    TEST_ASSERT(root->next->next == elem1);
    TEST_ASSERT_NULL(root->next->next->next);
}

Test *tests_core_queue_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_queue_remove_head_empty),
        new_TestFixture(test_queue_remove_head_one),
        new_TestFixture(test_queue_add_head_one),
        new_TestFixture(test_queue_add_head_two),
        new_TestFixture(test_queue_add_tail_one),
        new_TestFixture(test_queue_add_tail_two),
        new_TestFixture(test_queue_priority_add_one),
        new_TestFixture(test_queue_priority_add_two_equal),
        new_TestFixture(test_queue_priority_add_two_distinct),
        new_TestFixture(test_queue_priority_add_generic_two_equal),
        new_TestFixture(test_queue_priority_add_generic_two_distinct),
        new_TestFixture(test_queue_remove_one),
    };

    EMB_UNIT_TESTCALLER(core_queue_tests, set_up, NULL,
                        fixtures);

    return (Test *)&core_queue_tests;
}
