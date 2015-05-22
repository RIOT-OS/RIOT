/*
 * Copyright (C) 2014, 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include <string.h>

#include "embUnit.h"

#include "net/ng_pkt.h"
#include "net/ng_pktqueue.h"

#include "tests-pktqueue.h"

#define Q_LEN (4)

static ng_pktqueue_t q = NG_PKTQUEUE_INIT;
static ng_pktqueue_node_t qe[Q_LEN];

static void set_up(void)
{
    ng_pktqueue_init(&q);

    for (unsigned i = 0; i < Q_LEN; ++i) {
        ng_pktqueue_node_init(&(qe[i]));
    }
}

static void test_pktqueue_remove_head_empty(void)
{
    ng_pktqueue_t *root = &q;
    ng_pktqueue_node_t *res;

    res = ng_pktqueue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_pktqueue_remove_head_one(void)
{
    ng_pktqueue_t *root = &q;
    ng_pktqueue_node_t *elem = &(qe[1]), *res;

    elem->data = (ng_pktsnip_t *)62801;

    ng_pktqueue_add(root, elem);

    res = ng_pktqueue_remove_head(root);

    TEST_ASSERT(res == elem);
    TEST_ASSERT(((ng_pktsnip_t *)62801) == res->data);

    res = ng_pktqueue_remove_head(root);

    TEST_ASSERT_NULL(res);
}

static void test_pktqueue_add_one(void)
{
    ng_pktqueue_t *root = &q;
    ng_pktqueue_node_t *elem = &(qe[1]);

    elem->data = (ng_pktsnip_t *)7317;
    elem->priority = 713643658;

    ng_pktqueue_add(root, elem);

    TEST_ASSERT(root->first == elem);
    TEST_ASSERT(((ng_pktsnip_t *)7317) == root->first->data);
    TEST_ASSERT_EQUAL_INT(713643658, root->first->priority);

    TEST_ASSERT_NULL(root->first->next);
}

static void test_pktqueue_add_two_equal(void)
{
    ng_pktqueue_t *root = &q;
    ng_pktqueue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]);

    elem1->data = (ng_pktsnip_t *)27088;
    elem1->priority = 14202;

    elem2->data = (ng_pktsnip_t *)4356;
    elem2->priority = 14202;

    ng_pktqueue_add(root, elem1);
    ng_pktqueue_add(root, elem2);

    TEST_ASSERT(root->first == elem1);
    TEST_ASSERT(((ng_pktsnip_t *)27088) == root->first->data);
    TEST_ASSERT_EQUAL_INT(14202, root->first->priority);

    TEST_ASSERT(root->first->next == elem2);
    TEST_ASSERT(((ng_pktsnip_t *)4356) == root->first->next->data);
    TEST_ASSERT_EQUAL_INT(14202, root->first->next->priority);

    TEST_ASSERT_NULL(root->first->next->next);
}

static void test_pktqueue_add_two_distinct(void)
{
    ng_pktqueue_t *root = &q;
    ng_pktqueue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]);

    elem1->data = (ng_pktsnip_t *)46421;
    elem1->priority = 4567;

    elem2->data = (ng_pktsnip_t *)43088;
    elem2->priority = 1234;

    ng_pktqueue_add(root, elem1);
    ng_pktqueue_add(root, elem2);

    TEST_ASSERT(root->first == elem2);
    TEST_ASSERT(((ng_pktsnip_t *)43088) == root->first->data);
    TEST_ASSERT_EQUAL_INT(1234, root->first->priority);

    TEST_ASSERT(root->first->next == elem1);
    TEST_ASSERT(((ng_pktsnip_t *)46421) == root->first->next->data);
    TEST_ASSERT_EQUAL_INT(4567, root->first->next->priority);

    TEST_ASSERT_NULL(root->first->next->next);
}

static void test_pktqueue_remove_one(void)
{
    ng_pktqueue_t *root = &q;
    ng_pktqueue_node_t *elem1 = &(qe[1]), *elem2 = &(qe[2]), *elem3 = &(qe[3]);

    ng_pktqueue_add(root, elem1);
    ng_pktqueue_add(root, elem2);
    ng_pktqueue_add(root, elem3);
    ng_pktqueue_remove(root, elem2);

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
