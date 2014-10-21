/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    tests-pktqueue.c
 */
#include <string.h>

#include "embUnit/embUnit.h"

#include "pktqueue.h"

#include "tests-pktqueue.h"

#define Q_LEN (4)

static pktqueue_t q = PKTQUEUE_INIT;
static pktqueue_node_t qe[Q_LEN];

static void set_up(void)
{
    pktqueue_init(&q);

    for (unsigned i = 0; i < Q_LEN; ++i) {
        pktqueue_node_init(&(qe[i]));
    }
}

static void test_pktqueue_remove_head_empty(void)
{
    pktqueue_t *root = &q;
    pktqueue_node_t *res;

    res = pktqueue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_pktqueue_remove_head_one(void)
{
    pktqueue_t *root = &q;
    pktqueue_node_t *elem = &(qe[1]), *res;

    elem->data = (void *)62801;

    pktqueue_add(root, elem);

    res = pktqueue_remove_head(root);

    TEST_ASSERT(res == elem);
    TEST_ASSERT(((void *)62801) == res->data);

    res = pktqueue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_pktqueue_add_one(void)
{
    pktqueue_t *root = &q;
    pktqueue_node_t *elem = &(qe[1]);

    elem->data = (void *)7317;
    elem->priority = 713643658;

    pktqueue_add(root, elem);

    TEST_ASSERT(root->first == elem);
    TEST_ASSERT(((void *)7317) == root->first->data);
    TEST_ASSERT_EQUAL_INT(713643658, root->first->priority);

    TEST_ASSERT_NULL(root->first->next);
}

static void test_pktqueue_add_two_equal(void)
{
    pktqueue_t *root = &q;
    pktqueue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]);

    elem1->data = (void *)27088;
    elem1->priority = 14202;

    elem2->data = (void *)4356;
    elem2->priority = 14202;

    pktqueue_add(root, elem1);
    pktqueue_add(root, elem2);

    TEST_ASSERT(root->first == elem2);
    TEST_ASSERT(((void *)4356) == root->first->data);
    TEST_ASSERT_EQUAL_INT(14202, root->first->priority);

    TEST_ASSERT(root->first->next == elem1);
    TEST_ASSERT(((void *)27088) == root->first->next->data);
    TEST_ASSERT_EQUAL_INT(14202, root->first->next->priority);

    TEST_ASSERT_NULL(root->first->next->next);
}

static void test_pktqueue_add_two_distinct(void)
{
    pktqueue_t *root = &q;
    pktqueue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]);

    elem1->data = (void *)46421;
    elem1->priority = 4567;

    elem2->data = (void *)43088;
    elem2->priority = 1234;

    pktqueue_add(root, elem1);
    pktqueue_add(root, elem2);

    TEST_ASSERT(root->first == elem2);
    TEST_ASSERT(((void *)43088) == root->first->data);
    TEST_ASSERT_EQUAL_INT(1234, root->first->priority);

    TEST_ASSERT(root->first->next == elem1);
    TEST_ASSERT(((void *)46421) == root->first->next->data);
    TEST_ASSERT_EQUAL_INT(4567, root->first->next->priority);

    TEST_ASSERT_NULL(root->first->next->next);
}

static void test_pktqueue_remove_one(void)
{
    pktqueue_t *root = &q;
    pktqueue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]), *elem3 = &(qe[3]);

    pktqueue_add(root, elem1);
    pktqueue_add(root, elem2);
    pktqueue_add(root, elem3);
    pktqueue_remove(root, elem2);

    TEST_ASSERT(root->first == elem1);
    TEST_ASSERT(root->first->next == elem3);
    TEST_ASSERT_NULL(root->first->next->next);
}

Test *tests_pktqueue_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktqueue_remove_head_empty),
        new_TestFixture(test_pktqueue_remove_head_one),
        new_TestFixture(test_pktqueue_add_one),
        new_TestFixture(test_pktqueue_add_two_equal),
        new_TestFixture(test_pktqueue_add_two_distinct),
        new_TestFixture(test_pktqueue_remove_one),
    };

    EMB_UNIT_TESTCALLER(pktqueue_tests, set_up, NULL, fixtures);

    return (Test *)&pktqueue_tests;
}

void tests_pktqueue(void)
{
    TESTS_RUN(tests_pktqueue_tests());
}
/** @} */
