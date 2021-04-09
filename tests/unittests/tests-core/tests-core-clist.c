/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include <stdint.h>

#include "embUnit.h"

#include "clist.h"

#include "tests-core.h"

#define TEST_CLIST_LEN    (8)

static list_node_t tests_clist_buf[TEST_CLIST_LEN];
static list_node_t test_clist;

static void set_up(void)
{
    memset(tests_clist_buf, 0, sizeof(tests_clist_buf));
    test_clist.next = NULL;
}

static void test_clist_rpush(void)
{
    list_node_t *elem = &(tests_clist_buf[0]);
    list_node_t *list = &test_clist;

    clist_rpush(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == list->next);
}

static void test_clist_add_two(void)
{
    list_node_t *list = &test_clist;

    list_node_t *elem = &(tests_clist_buf[1]);

    test_clist_rpush();

    clist_rpush(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next == elem);
    TEST_ASSERT(list->next->next->next == list->next);
}

static void test_clist_add_three(void)
{
    list_node_t *list = &test_clist;

    for (int i = 0; i < 3; i++) {
        clist_rpush(list, &(tests_clist_buf[i]));
    }

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next == &(tests_clist_buf[2]));
    TEST_ASSERT(list->next->next == &(tests_clist_buf[0]));
    TEST_ASSERT(list->next->next->next == &(tests_clist_buf[1]));
    TEST_ASSERT(list->next->next->next->next == &(tests_clist_buf[2]));
}

static void test_clist_find(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_three();

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT(clist_find(list, &(tests_clist_buf[i])) == &(tests_clist_buf[i]));
    }

    TEST_ASSERT_NULL(clist_find(list, &(tests_clist_buf[3])));
}

static void test_clist_find_before(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_three();

    TEST_ASSERT(clist_find_before(list, &(tests_clist_buf[0])) == &(tests_clist_buf[2]));

    for (int i = 1; i < 3; i++) {
        TEST_ASSERT(clist_find_before(list, &(tests_clist_buf[i])) == &(tests_clist_buf[i-1]));
    }

    TEST_ASSERT_NULL(clist_find_before(list, &(tests_clist_buf[3])));
}

static void test_clist_remove(void)
{
    list_node_t *list = &test_clist;

    for (int i = 0; i < 3; i++) {
        set_up();
        test_clist_add_three();
        clist_remove(list, &(tests_clist_buf[i]));

        for (int j = 0; j < 3; j++) {
            if (i == j) {
                TEST_ASSERT_NULL(clist_find(list, &(tests_clist_buf[j])));
            }
            else {
                TEST_ASSERT(clist_find(list, &(tests_clist_buf[j])) == &(tests_clist_buf[j]));
            }
        }
    }

    /* list now contains 0, 1 */
    TEST_ASSERT(list->next == &(tests_clist_buf[1]));
    TEST_ASSERT(list->next->next == &(tests_clist_buf[0]));

    clist_remove(list, &(tests_clist_buf[1]));
    TEST_ASSERT(list->next == &(tests_clist_buf[0]));
    TEST_ASSERT(list->next->next == &(tests_clist_buf[0]));

    clist_remove(list, &(tests_clist_buf[0]));
    TEST_ASSERT_NULL(list->next);
}

static void test_clist_lpop(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_three();

    TEST_ASSERT(clist_lpop(list) == &tests_clist_buf[0]);
    TEST_ASSERT_NOT_NULL(list->next);

    TEST_ASSERT(clist_lpop(list) == &tests_clist_buf[1]);
    TEST_ASSERT_NOT_NULL(list->next);

    TEST_ASSERT(clist_lpop(list) == &tests_clist_buf[2]);
    TEST_ASSERT_NULL(list->next);
    TEST_ASSERT_NULL(clist_lpop(list));
}

static void test_clist_lpush(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_two();
    clist_lpush(list, &tests_clist_buf[2]);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == &tests_clist_buf[2]);
}

static void test_clist_rpop(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_two();

    clist_rpop(list);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == &tests_clist_buf[0]);
}

static void test_clist_remove_two(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_two();

    clist_lpop(list);
    clist_lpop(list);

    TEST_ASSERT_NULL(list->next);
}

static void test_clist_lpoprpush(void)
{
    list_node_t *list = &test_clist;
    list->next = NULL;

    test_clist_add_two();

    clist_lpoprpush(list);

    TEST_ASSERT(list->next->next == &tests_clist_buf[1]);

    clist_lpoprpush(list);

    TEST_ASSERT(list->next->next == &tests_clist_buf[0]);
}

static int _foreach_called;
static int _foreach_visited[TEST_CLIST_LEN];
static int _foreach_abort_after = TEST_CLIST_LEN/2;

static void _foreach_test(clist_node_t *node)
{
    TEST_ASSERT(node == &tests_clist_buf[_foreach_called]);

    for (int i = 0; i < TEST_CLIST_LEN; i++) {
        if (node == &tests_clist_buf[i]) {
            _foreach_visited[i]++;
            break;
        }
    }

    for (int i = 0; i < TEST_CLIST_LEN; i++) {
        if (i <= _foreach_called) {
            TEST_ASSERT(_foreach_visited[i] == 1);
        }
        else {
            TEST_ASSERT(_foreach_visited[i] == 0);
        }
    }

    _foreach_called++;
}

/* embunit test macros only work within void returning functions, so this
 * trampoline function is needed */
static int _foreach_test_trampoline(clist_node_t *node, void *arg)
{
    (void)arg;
    _foreach_test(node);
    if (_foreach_called == _foreach_abort_after) {
        return 1;
    }
    else {
        return 0;
    }
}

static void test_clist_foreach(void)
{
    void *res;
    list_node_t *list = &test_clist;

    _foreach_called = 0;
    res = clist_foreach(list, _foreach_test_trampoline, NULL);
    TEST_ASSERT(_foreach_called == 0);
    TEST_ASSERT(res == NULL);

    for (int i = 0; i < TEST_CLIST_LEN; i++) {
        clist_rpush(list, &tests_clist_buf[i]);
    }

    res = clist_foreach(list, _foreach_test_trampoline, NULL);

    TEST_ASSERT(_foreach_called == _foreach_abort_after);
    TEST_ASSERT(res == &tests_clist_buf[_foreach_abort_after-1]);

    _foreach_called = 0;
    for (int i = 0; i < TEST_CLIST_LEN; i++) {
        _foreach_visited[i] = 0;
    }

    _foreach_abort_after = (TEST_CLIST_LEN + 1);
    res = clist_foreach(list, _foreach_test_trampoline, NULL);

    TEST_ASSERT(_foreach_called == TEST_CLIST_LEN);
    TEST_ASSERT(res == NULL);
}

static int _cmp(clist_node_t *a, clist_node_t *b)
{
    /* this comparison function will sort by the actual memory address of the
     * list node (descending) */
    return (uintptr_t)a - (uintptr_t) b;
}

static void test_clist_sort_empty(void)
{
    clist_node_t empty = { .next=NULL };
    clist_sort(&empty, _cmp);

    TEST_ASSERT(empty.next == NULL);
}

/*
 * This test works by first adding all list nodes of tests_clist_buf to a new
 * list. As the array is traversed in order, the memory addresses of the list
 * nodes are naturally sorted ascending.
 * The list is then rotated (using clist_lpoprpush()) a couple of times in
 * order to create a somewhat arbitrary sorting.
 * Then clist_sort() is run with a comparison function that just returns the
 * difference (a-b), which effectively leads to a list sorted by descending
 * list node addresses.
 */
static void test_clist_sort(void)
{
    clist_node_t *list = &test_clist;

    for (int i = 0; i < TEST_CLIST_LEN; i++) {
        clist_rpush(list, &tests_clist_buf[i]);
    }

    /* rotate the list a couple of times in order to mess up the sorting */
    clist_lpoprpush(list);
    clist_lpoprpush(list);
    clist_lpoprpush(list);

    /* sort list */
    clist_sort(list, _cmp);

    uintptr_t last = (uintptr_t) list->next;

    for (int i = 0; i < TEST_CLIST_LEN; i++) {
        TEST_ASSERT((uintptr_t) clist_rpop(list) <= last);
    }
}

static void test_clist_count(void)
{
    size_t n = clist_count(&test_clist);
    TEST_ASSERT(n == 0);

    for (unsigned i = 1; i <= TEST_CLIST_LEN; i++) {
        clist_rpush(&test_clist, &tests_clist_buf[i - 1]);
        n = clist_count(&test_clist);
        TEST_ASSERT(n == i);
    }
    for (unsigned i = TEST_CLIST_LEN; i > 0; i--) {
        clist_lpop(&test_clist);
        n = clist_count(&test_clist);
        TEST_ASSERT(n == (i - 1));
    }
}

static void test_clist_is_empty(void)
{
    TEST_ASSERT(clist_is_empty(&test_clist));

    for (unsigned i = 1; i <= TEST_CLIST_LEN; i++) {
        clist_rpush(&test_clist, &tests_clist_buf[i - 1]);
        TEST_ASSERT(!clist_is_empty(&test_clist));
    }
    for (unsigned i = TEST_CLIST_LEN; i > 0; i--) {
        clist_lpop(&test_clist);
        /* when i == 1 at the beginning of the iteration, there's one element
           left, which is then dropped in the line above.
           So in all cases but that last one, the list is not empty. */
        TEST_ASSERT(clist_is_empty(&test_clist) == (i == 1));
    }
}

Test *tests_core_clist_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_clist_rpush),
        new_TestFixture(test_clist_add_two),
        new_TestFixture(test_clist_lpop),
        new_TestFixture(test_clist_rpop),
        new_TestFixture(test_clist_lpush),
        new_TestFixture(test_clist_remove_two),
        new_TestFixture(test_clist_add_three),
        new_TestFixture(test_clist_find),
        new_TestFixture(test_clist_find_before),
        new_TestFixture(test_clist_remove),
        new_TestFixture(test_clist_lpoprpush),
        new_TestFixture(test_clist_foreach),
        new_TestFixture(test_clist_sort_empty),
        new_TestFixture(test_clist_sort),
        new_TestFixture(test_clist_count),
        new_TestFixture(test_clist_is_empty),
    };

    EMB_UNIT_TESTCALLER(core_clist_tests, set_up, NULL,
                        fixtures);

    return (Test *)&core_clist_tests;
}
