/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests btree implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "btree.h"
#include "embUnit.h"

static btree_t _btree;
static btree_node_t _nodes[15];

static const uint32_t _keys[] = {
    8, 3, 10, 1, 6, 14, 0, 2
};

static uint32_t prev_key = 0;

static void _btree_node_print(btree_node_t *node, size_t depth, void *ctx)
{
    (void)ctx;
    (void)depth;
    TEST_ASSERT_EQUAL_INT(prev_key + 1, node->key);
    prev_key = node->key;
}

static void tests_btree_empty(void)
{
    btree_t tree = {.start = NULL};
    btree_traverse(&tree, _btree_node_print, NULL);
    TEST_ASSERT(NULL == btree_remove(&tree, 0));
    TEST_ASSERT(NULL == btree_find_key(&tree, 0));
}

static void tests_btree_construct(void)
{

    for (size_t i = 0; i < ARRAY_SIZE(_nodes); i++) {
        /* Worst case insertions */
        btree_insert(&_btree, &_nodes[i], i + 1);
    }

    TEST_ASSERT_EQUAL_INT(4, btree_max_depth(&_btree));

    btree_traverse(&_btree, _btree_node_print, NULL);
}

static void tests_btree_lookup(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(_nodes); i++) {
        btree_node_t *n = btree_find_key(&_btree, i + 1);
        TEST_ASSERT(n);
        TEST_ASSERT_EQUAL_INT(i + 1, n->key);
    }

    TEST_ASSERT_EQUAL_INT(14, btree_remove(&_btree, 14)->key);
    TEST_ASSERT_EQUAL_INT(15, btree_remove(&_btree, 15)->key);
    TEST_ASSERT_EQUAL_INT(13, btree_remove(&_btree, 13)->key);
}

static void tests_btree_remove(void)
{
    memset(&_btree, 0, sizeof(_btree));
    memset(_nodes, 0, sizeof(_nodes));

    for (size_t i = 0; i < ARRAY_SIZE(_keys); i++) {
        btree_insert(&_btree, &_nodes[i], _keys[i]);
    }
    TEST_ASSERT_EQUAL_INT(6, btree_remove(&_btree, 6)->key);
    TEST_ASSERT_EQUAL_INT(14, btree_remove(&_btree, 14)->key);
    TEST_ASSERT_EQUAL_INT(BTREE_OK, btree_insert(&_btree,
                                                 &_nodes[ARRAY_SIZE(_keys) + 1],
                                                 14));
}

static void tests_btree_remove2(void)
{
    memset(&_btree, 0, sizeof(_btree));
    memset(_nodes, 0, sizeof(_nodes));

    for (size_t i = 0; i < 3; i++) {
        btree_insert(&_btree, &_nodes[i], _keys[i]);
    }

    btree_remove(&_btree, 0);
}

Test *tests_btree(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_btree_empty),
        new_TestFixture(tests_btree_construct),
        new_TestFixture(tests_btree_lookup),
        new_TestFixture(tests_btree_remove),
        new_TestFixture(tests_btree_remove2),
    };

    EMB_UNIT_TESTCALLER(btree_tests, NULL, NULL, fixtures);
    return (Test*)&btree_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_btree());
    TESTS_END();

    return 0;
}
