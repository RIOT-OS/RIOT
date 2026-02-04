/*
 * SPDX-FileCopyrightText: 2023 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
  * @ingroup    tests
  * @{
  *
  * @file
  * @brief      Unit tests for l2scan list
  *
  * @author     Fabian Hüßler <fabian.huessler@ml-pa.com>
  */

#include <stdint.h>
#include <string.h>

#include "embUnit.h"
#include "container.h"
#include "net/netopt.h"
#include "net/l2scan_list.h"

typedef struct my_scan_result_node {
    list_node_t node;
    netopt_scan_result_t result;
} my_scan_result_node_t;

static const netopt_scan_result_t _test_results[] = {
    [0] = {
        .channel = 11,
        .strength = -40,
    },
    [1] = {
        .channel = 11,
        .strength = -52,
    },
    [2] = {
        .channel = 6,
        .strength = -53,
    },
    [3] = {
        .channel = 6,
        .strength = -54,
    },
    [4] = {
        .channel = 6,
        .strength = -55,
    },
    [5] = {
        .channel = 1,
        .strength = -60,
    },
    [6] = {
        .channel = 1,
        .strength = -70,
    },
    [7] = {
        .channel = 1,
        .strength = -80,
    }
};

struct {
    my_scan_result_node_t *head;
    my_scan_result_node_t result[ARRAY_SIZE(_test_results)];
} _scan_list;

static void test_l2scan_list_empty(void)
{
    l2scan_list_empty((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                      ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t));
    TEST_ASSERT(!_scan_list.head);
}

static void test_l2scan_list_not_empty(void)
{
    l2scan_list_empty((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                      ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t));
    TEST_ASSERT(!_scan_list.head);
    l2scan_list_insert((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[0]);
    TEST_ASSERT(_scan_list.head);
}

static void test_l2scan_list_insert(void)
{
    l2scan_list_empty((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                      ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t));
    TEST_ASSERT(!_scan_list.head);
    /* random order insert */
    l2scan_list_insert((l2scan_list_t *)&_scan_list,  (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[6]);
    l2scan_list_insert((l2scan_list_t *)&_scan_list,  (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[0]);
    l2scan_list_insert((l2scan_list_t *)&_scan_list,  (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[3]);
    l2scan_list_insert((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[5]);
    l2scan_list_insert((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[1]);
    l2scan_list_insert((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[2]);
    l2scan_list_insert((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[7]);
    l2scan_list_insert((l2scan_list_t *)&_scan_list, (list_node_t *)_scan_list.result,
                       ARRAY_SIZE(_scan_list.result), sizeof(my_scan_result_node_t),
                       &_test_results[4]);
    /* now expect it to be sorted by strength as it is in the test data */
    my_scan_result_node_t *node = _scan_list.head;
    for (unsigned i = 0; i < ARRAY_SIZE(_test_results); i++) {
        TEST_ASSERT(node);
        TEST_ASSERT(node->result.channel == _test_results[i].channel);
        TEST_ASSERT(node->result.strength == _test_results[i].strength);
        node = (my_scan_result_node_t *)node->node.next;
    }
}

static void _setup(void)
{
    memset(&_scan_list, 0, sizeof(_scan_list));
}

Test* test_l2scan_list(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_l2scan_list_empty),
        new_TestFixture(test_l2scan_list_not_empty),
        new_TestFixture(test_l2scan_list_insert),
    };

    EMB_UNIT_TESTCALLER(tests_l2scan_list, _setup, NULL, fixtures);
    return (Test *)&tests_l2scan_list;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(test_l2scan_list());
    TESTS_END();
}
