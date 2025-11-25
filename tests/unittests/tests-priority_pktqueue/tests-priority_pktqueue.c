/*
 * Copyright (C) 2016, 2016 Shuguo Zhuo <shuguo.zhuo@inria.fr>
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

#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/priority_pktqueue.h"

#include "unittests-constants.h"
#include "tests-priority_pktqueue.h"

#define PKT_INIT_ELEM(len, data, next) \
    { (next), (void *)(data), (len), 1, GNRC_NETTYPE_UNDEF }
#define PKT_INIT_ELEM_STATIC_DATA(data, next) PKT_INIT_ELEM(sizeof(data), (void *)(data), (next))
#define PKTQUEUE_INIT_ELEM(pkt) { NULL, pkt }

static gnrc_priority_pktqueue_t pkt_queue;

static void set_up(void)
{
    pkt_queue.first = NULL;
    gnrc_pktbuf_init();
}

static void test_gnrc_priority_pktqueue_init(void)
{
    gnrc_priority_pktqueue_node_t elem;

    pkt_queue.first = (priority_queue_node_t *)(&elem);
    gnrc_priority_pktqueue_init(&pkt_queue);

    TEST_ASSERT_NULL(pkt_queue.first);
}

static void test_gnrc_priority_pktqueue_node_init(void)
{
    gnrc_pktsnip_t pkt = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_priority_pktqueue_node_t elem;

    gnrc_priority_pktqueue_node_init(&elem, TEST_UINT32, &pkt);

    TEST_ASSERT_NULL(elem.next);
    TEST_ASSERT(elem.pkt == &pkt);
    TEST_ASSERT_EQUAL_INT(TEST_UINT32, elem.priority);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, elem.pkt->data);
}

static void test_gnrc_priority_pktqueue_push_one(void)
{
    gnrc_pktsnip_t pkt = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_priority_pktqueue_node_t elem = PRIORITY_PKTQUEUE_NODE_INIT(TEST_UINT32, &pkt);

    gnrc_priority_pktqueue_push(&pkt_queue, &elem);

    TEST_ASSERT((gnrc_priority_pktqueue_node_t *)(pkt_queue.first) == &elem);
    TEST_ASSERT_NULL(((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next);
    TEST_ASSERT_EQUAL_INT(TEST_UINT32,
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->priority);
    TEST_ASSERT_EQUAL_INT(1, ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->users);
    TEST_ASSERT_NULL(((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8,
                             ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8),
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF,
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->type);
}

static void test_gnrc_priority_pktqueue_push_two(void)
{
    gnrc_pktsnip_t pkt1 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t pkt2 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING16, NULL);
    gnrc_priority_pktqueue_node_t elem1 = PRIORITY_PKTQUEUE_NODE_INIT(1, &pkt1);
    gnrc_priority_pktqueue_node_t elem2 = PRIORITY_PKTQUEUE_NODE_INIT(0, &pkt2);

    gnrc_priority_pktqueue_push(&pkt_queue, &elem1);
    gnrc_priority_pktqueue_push(&pkt_queue, &elem2);

    TEST_ASSERT((gnrc_priority_pktqueue_node_t *)(pkt_queue.first) == &elem2);
    TEST_ASSERT((gnrc_priority_pktqueue_node_t *)(pkt_queue.first->next) == &elem1);
    TEST_ASSERT_NULL(((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next->next);
    TEST_ASSERT_EQUAL_INT(0, ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->priority);
    TEST_ASSERT_EQUAL_INT(1, ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next->priority);
    TEST_ASSERT_EQUAL_INT(1, ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->users);
    TEST_ASSERT_NULL(((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16,
                             ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16),
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF,
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->pkt->type);
    TEST_ASSERT_EQUAL_INT(1,
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next->pkt->users);
    TEST_ASSERT_NULL(((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8,
                             ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8),
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next->pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF,
                          ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->next->pkt->type);
}

static void test_gnrc_priority_pktqueue_length(void)
{
    gnrc_pktsnip_t pkt1 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t pkt2 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING16, NULL);
    gnrc_priority_pktqueue_node_t elem1 = PRIORITY_PKTQUEUE_NODE_INIT(1, &pkt1);
    gnrc_priority_pktqueue_node_t elem2 = PRIORITY_PKTQUEUE_NODE_INIT(0, &pkt2);

    TEST_ASSERT_EQUAL_INT(0, gnrc_priority_pktqueue_length(&pkt_queue));

    gnrc_priority_pktqueue_push(&pkt_queue, &elem1);
    TEST_ASSERT_EQUAL_INT(1, gnrc_priority_pktqueue_length(&pkt_queue));

    gnrc_priority_pktqueue_push(&pkt_queue, &elem2);
    TEST_ASSERT_EQUAL_INT(2, gnrc_priority_pktqueue_length(&pkt_queue));
}

static void test_gnrc_priority_pktqueue_flush(void)
{
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8),
                                           GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_TEST);
    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL(pkt2);

    gnrc_priority_pktqueue_node_t elem1 = PRIORITY_PKTQUEUE_NODE_INIT(1, pkt1);
    gnrc_priority_pktqueue_node_t elem2 = PRIORITY_PKTQUEUE_NODE_INIT(0, pkt2);

    gnrc_priority_pktqueue_push(&pkt_queue, &elem1);
    gnrc_priority_pktqueue_push(&pkt_queue, &elem2);

    gnrc_priority_pktqueue_flush(&pkt_queue);

    TEST_ASSERT_NULL(pkt_queue.first);
    TEST_ASSERT_EQUAL_INT(0, gnrc_priority_pktqueue_length(&pkt_queue));
}

static void test_gnrc_priority_pktqueue_head(void)
{
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8),
                                           GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, TEST_STRING12, sizeof(TEST_STRING12),
                                           GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt3 = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_TEST);
    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL(pkt3);

    gnrc_priority_pktqueue_node_t elem1 = PRIORITY_PKTQUEUE_NODE_INIT(1, pkt1);
    gnrc_priority_pktqueue_node_t elem2 = PRIORITY_PKTQUEUE_NODE_INIT(1, pkt2);
    gnrc_priority_pktqueue_node_t elem3 = PRIORITY_PKTQUEUE_NODE_INIT(0, pkt3);

    gnrc_pktsnip_t *head = gnrc_priority_pktqueue_head(&pkt_queue);
    TEST_ASSERT_NULL(head);

    gnrc_priority_pktqueue_push(&pkt_queue, &elem1);
    head = gnrc_priority_pktqueue_head(&pkt_queue);
    TEST_ASSERT(head == pkt1);

    gnrc_priority_pktqueue_push(&pkt_queue, &elem2);
    head = gnrc_priority_pktqueue_head(&pkt_queue);

    TEST_ASSERT(head == pkt1);

    gnrc_priority_pktqueue_push(&pkt_queue, &elem3);
    head = gnrc_priority_pktqueue_head(&pkt_queue);

    TEST_ASSERT(head == pkt3);

    gnrc_priority_pktqueue_flush(&pkt_queue);

    head = gnrc_priority_pktqueue_head(&pkt_queue);
    TEST_ASSERT_NULL(head);

}

static void test_gnrc_priority_pktqueue_pop_empty(void)
{
    gnrc_pktsnip_t *res;

    res = gnrc_priority_pktqueue_pop(&pkt_queue);

    TEST_ASSERT_NULL(pkt_queue.first);
    TEST_ASSERT_NULL(res);
    TEST_ASSERT_EQUAL_INT(0, gnrc_priority_pktqueue_length(&pkt_queue));
}

static void test_gnrc_priority_pktqueue_pop(void)
{
    gnrc_pktsnip_t pkt1 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t pkt2 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING16, NULL);
    gnrc_priority_pktqueue_node_t elem1 = PRIORITY_PKTQUEUE_NODE_INIT(1, &pkt1);
    gnrc_priority_pktqueue_node_t elem2 = PRIORITY_PKTQUEUE_NODE_INIT(0, &pkt2);
    gnrc_pktsnip_t *res;
    gnrc_pktsnip_t *head;

    gnrc_priority_pktqueue_push(&pkt_queue, &elem1);
    gnrc_priority_pktqueue_push(&pkt_queue, &elem2);

    res = gnrc_priority_pktqueue_pop(&pkt_queue);

    TEST_ASSERT(res == &pkt2);
    TEST_ASSERT_NULL(elem2.pkt);
    TEST_ASSERT_NULL(elem2.next);
    TEST_ASSERT_EQUAL_INT(0, elem2.priority);
    TEST_ASSERT((gnrc_priority_pktqueue_node_t *)(pkt_queue.first) == &elem1);
    TEST_ASSERT_EQUAL_INT(1, ((gnrc_priority_pktqueue_node_t *)(pkt_queue.first))->priority);
    TEST_ASSERT_EQUAL_INT(1, res->users);
    TEST_ASSERT_NULL(res->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, res->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), res->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, res->type);

    res = gnrc_priority_pktqueue_pop(&pkt_queue);

    TEST_ASSERT_NULL(pkt_queue.first);
    TEST_ASSERT_NULL(elem1.pkt);
    TEST_ASSERT_NULL(elem1.next);
    TEST_ASSERT_EQUAL_INT(0, elem1.priority);
    TEST_ASSERT(res == &pkt1);
    TEST_ASSERT_EQUAL_INT(1, res->users);
    TEST_ASSERT_NULL(res->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, res->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), res->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, res->type);

    head = gnrc_priority_pktqueue_head(&pkt_queue);

    TEST_ASSERT_NULL(pkt_queue.first);
    TEST_ASSERT_NULL(head);

}

Test *tests_priority_pktqueue_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_gnrc_priority_pktqueue_init),
        new_TestFixture(test_gnrc_priority_pktqueue_node_init),
        new_TestFixture(test_gnrc_priority_pktqueue_push_one),
        new_TestFixture(test_gnrc_priority_pktqueue_push_two),
        new_TestFixture(test_gnrc_priority_pktqueue_length),
        new_TestFixture(test_gnrc_priority_pktqueue_flush),
        new_TestFixture(test_gnrc_priority_pktqueue_head),
        new_TestFixture(test_gnrc_priority_pktqueue_pop_empty),
        new_TestFixture(test_gnrc_priority_pktqueue_pop),
    };

    EMB_UNIT_TESTCALLER(priority_pktqueue_tests, set_up, NULL, fixtures);

    return (Test *)&priority_pktqueue_tests;
}

void tests_priority_pktqueue(void)
{
    TESTS_RUN(tests_priority_pktqueue_tests());
}
/** @} */
