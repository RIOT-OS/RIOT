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

#include "unittests-constants.h"
#include "tests-pktqueue.h"

#define PKT_INIT_ELEM(len, data, next) \
    { 1, (next), (data), (len), NG_NETTYPE_UNDEF }
#define PKT_INIT_ELEM_STATIC_DATA(data, next) PKT_INIT_ELEM(sizeof(data), data, next)
#define PKTQUEUE_INIT_ELEM(pkt) { NULL, pkt }

static ng_pktqueue_t *root;

static void set_up(void)
{
    root = NULL;
}

static void test_pktqueue_add_one(void)
{
    ng_pktsnip_t pkt = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    ng_pktqueue_t elem = PKTQUEUE_INIT_ELEM(&pkt);

    ng_pktqueue_add(&root, &elem);

    TEST_ASSERT(root == &elem);
    TEST_ASSERT_EQUAL_INT(1, root->pkt->users);
    TEST_ASSERT_NULL(root->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, root->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), root->pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, root->pkt->type);
}

static void test_pktqueue_add_two(void)
{
    ng_pktsnip_t pkt1 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    ng_pktsnip_t pkt2 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING16, NULL);
    ng_pktqueue_t elem1 = PKTQUEUE_INIT_ELEM(&pkt1);
    ng_pktqueue_t elem2 = PKTQUEUE_INIT_ELEM(&pkt2);

    ng_pktqueue_add(&root, &elem1);
    ng_pktqueue_add(&root, &elem2);

    TEST_ASSERT(root == &elem1);
    TEST_ASSERT(root->next == &elem2);
    TEST_ASSERT_EQUAL_INT(1, root->pkt->users);
    TEST_ASSERT_NULL(root->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, root->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), root->pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, root->pkt->type);
    TEST_ASSERT_EQUAL_INT(1, root->next->pkt->users);
    TEST_ASSERT_NULL(root->next->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, root->next->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), root->next->pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, root->next->pkt->type);
}

static void test_pktqueue_remove(void)
{
    ng_pktsnip_t pkt1 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    ng_pktsnip_t pkt2 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING16, NULL);
    ng_pktqueue_t *res;
    ng_pktqueue_t elem1 = PKTQUEUE_INIT_ELEM(&pkt1);
    ng_pktqueue_t elem2 = PKTQUEUE_INIT_ELEM(&pkt2);

    ng_pktqueue_add(&root, &elem1);
    ng_pktqueue_add(&root, &elem2);

    res = ng_pktqueue_remove(&root, &elem2);

    TEST_ASSERT(res == &elem2);
    TEST_ASSERT(root == &elem1);
    TEST_ASSERT_EQUAL_INT(1, res->pkt->users);
    TEST_ASSERT_NULL(res->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, res->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), res->pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, res->pkt->type);

    res = ng_pktqueue_remove(&root, &elem1);

    TEST_ASSERT_NULL(root);
    TEST_ASSERT(res == &elem1);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, res->pkt->type);
    TEST_ASSERT_EQUAL_INT(1, res->pkt->users);
    TEST_ASSERT_NULL(res->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, res->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), res->pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, res->pkt->type);

    res = ng_pktqueue_remove(&root, NULL);

    TEST_ASSERT_NULL(root);
    TEST_ASSERT_NULL(res);
}

static void test_pktqueue_remove_head_empty(void)
{
    ng_pktqueue_t *res;

    res = ng_pktqueue_remove_head(&root);

    TEST_ASSERT_NULL(root);
    TEST_ASSERT_NULL(res);
}

static void test_pktqueue_remove_head(void)
{
    ng_pktsnip_t pkt1 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    ng_pktsnip_t pkt2 = PKT_INIT_ELEM_STATIC_DATA(TEST_STRING16, NULL);
    ng_pktqueue_t *res;
    ng_pktqueue_t elem1 = PKTQUEUE_INIT_ELEM(&pkt1);
    ng_pktqueue_t elem2 = PKTQUEUE_INIT_ELEM(&pkt2);

    ng_pktqueue_add(&root, &elem1);
    ng_pktqueue_add(&root, &elem2);

    res = ng_pktqueue_remove_head(&root);

    TEST_ASSERT(res == &elem1);
    TEST_ASSERT(root == &elem2);
    TEST_ASSERT_EQUAL_INT(1, res->pkt->users);
    TEST_ASSERT_NULL(res->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, res->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), res->pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, res->pkt->type);

    res = ng_pktqueue_remove_head(&root);

    TEST_ASSERT_NULL(root);
    TEST_ASSERT(res == &elem2);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, res->pkt->type);
    TEST_ASSERT_EQUAL_INT(1, res->pkt->users);
    TEST_ASSERT_NULL(res->pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, res->pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), res->pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, res->pkt->type);
}

Test *tests_pktqueue_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktqueue_add_one),
        new_TestFixture(test_pktqueue_add_two),
        new_TestFixture(test_pktqueue_remove),
        new_TestFixture(test_pktqueue_remove_head_empty),
        new_TestFixture(test_pktqueue_remove_head),
    };

    EMB_UNIT_TESTCALLER(pktqueue_tests, set_up, NULL, fixtures);

    return (Test *)&pktqueue_tests;
}

void tests_pktqueue(void)
{
    TESTS_RUN(tests_pktqueue_tests());
}
/** @} */
