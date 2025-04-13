/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "embUnit.h"

#include "clist.h"
#include "container.h"

#include "tests-core.h"

#define TEST_CLIST_LEN    (64)

/* Test list data structure */
struct test_list_node {
    list_node_t list;
    unsigned value;
};

static struct test_list_node tests_clist_buf[TEST_CLIST_LEN];
static list_node_t test_clist;

static struct test_list_node * _get_test_list_node(list_node_t *node)
{
    return container_of(node, struct test_list_node, list);
}

static void set_up(void)
{
    memset(tests_clist_buf, 0, sizeof(tests_clist_buf));
    test_clist.next = NULL;
}

static void test_clist_rpush(void)
{
    list_node_t *elem = &(tests_clist_buf[0].list);
    list_node_t *list = &test_clist;

    clist_rpush(list, elem);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == list->next);
}

static void test_clist_add_two(void)
{
    list_node_t *list = &test_clist;

    list_node_t *elem = &(tests_clist_buf[1].list);

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
        clist_rpush(list, &(tests_clist_buf[i].list));
    }

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next == &(tests_clist_buf[2].list));
    TEST_ASSERT(list->next->next == &(tests_clist_buf[0].list));
    TEST_ASSERT(list->next->next->next == &(tests_clist_buf[1].list));
    TEST_ASSERT(list->next->next->next->next == &(tests_clist_buf[2].list));
}

static void test_clist_find(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_three();

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT(clist_find(list, &(tests_clist_buf[i].list)) == &(tests_clist_buf[i].list));
    }

    TEST_ASSERT_NULL(clist_find(list, &(tests_clist_buf[3].list)));
}

static void test_clist_find_before(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_three();

    TEST_ASSERT(clist_find_before(list, &(tests_clist_buf[0].list)) == &(tests_clist_buf[2].list));

    for (int i = 1; i < 3; i++) {
        TEST_ASSERT(clist_find_before(list, &(tests_clist_buf[i].list)) == &(tests_clist_buf[i-1].list));
    }

    TEST_ASSERT_NULL(clist_find_before(list, &(tests_clist_buf[3].list)));
}

static void test_clist_remove(void)
{
    list_node_t *list = &test_clist;

    for (int i = 0; i < 3; i++) {
        set_up();
        test_clist_add_three();
        clist_remove(list, &(tests_clist_buf[i].list));

        for (int j = 0; j < 3; j++) {
            if (i == j) {
                TEST_ASSERT_NULL(clist_find(list, &(tests_clist_buf[j].list)));
            }
            else {
                TEST_ASSERT(clist_find(list, &(tests_clist_buf[j].list)) == &(tests_clist_buf[j].list));
            }
        }
    }

    /* list now contains 0, 1 */
    TEST_ASSERT(list->next == &(tests_clist_buf[1].list));
    TEST_ASSERT(list->next->next == &(tests_clist_buf[0].list));

    clist_remove(list, &(tests_clist_buf[1].list));
    TEST_ASSERT(list->next == &(tests_clist_buf[0].list));
    TEST_ASSERT(list->next->next == &(tests_clist_buf[0].list));

    clist_remove(list, &(tests_clist_buf[0].list));
    TEST_ASSERT_NULL(list->next);
}

static void test_clist_lpop(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_three();

    TEST_ASSERT(clist_lpop(list) == &tests_clist_buf[0].list);
    TEST_ASSERT_NOT_NULL(list->next);

    TEST_ASSERT(clist_lpop(list) == &tests_clist_buf[1].list);
    TEST_ASSERT_NOT_NULL(list->next);

    TEST_ASSERT(clist_lpop(list) == &tests_clist_buf[2].list);
    TEST_ASSERT_NULL(list->next);
    TEST_ASSERT_NULL(clist_lpop(list));
}

static void test_clist_lpush(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_two();
    clist_lpush(list, &tests_clist_buf[2].list);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == &tests_clist_buf[2].list);
}

static void test_clist_rpop(void)
{
    list_node_t *list = &test_clist;

    test_clist_add_two();

    clist_rpop(list);

    TEST_ASSERT_NOT_NULL(list->next);
    TEST_ASSERT(list->next->next == &tests_clist_buf[0].list);
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

    TEST_ASSERT(list->next->next == &tests_clist_buf[1].list);

    clist_lpoprpush(list);

    TEST_ASSERT(list->next->next == &tests_clist_buf[0].list);
}

static int _foreach_called;
static int _foreach_visited[TEST_CLIST_LEN];
static int _foreach_abort_after = TEST_CLIST_LEN/2;

static void _foreach_test(clist_node_t *node)
{
    TEST_ASSERT(node == &tests_clist_buf[_foreach_called].list);

    for (int i = 0; i < TEST_CLIST_LEN; i++) {
        if (node == &tests_clist_buf[i].list) {
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
        clist_rpush(list, &tests_clist_buf[i].list);
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

static int _cmp(clist_node_t *_a, clist_node_t *_b)
{
    struct test_list_node *a = _get_test_list_node(_a), *b = _get_test_list_node(_b);
    return a->value - b->value;
}

static bool _is_clist_stable_sorted(clist_node_t *list)
{
    /* empty list is always considered as sorted */
    if (!list->next) {
        return true;
    }

    clist_node_t *end_of_clist = list->next;
    clist_node_t *prev = list->next->next;
    clist_node_t *cur = prev->next;

    while (prev != end_of_clist) {
        struct test_list_node *a = _get_test_list_node(prev);
        struct test_list_node *b = _get_test_list_node(cur);

        if (a->value > b->value) {
            return false;
        }
        if (a->value == b->value) {
            /* stable sort requires that order is not touched when elements
             * are equal */
            if ((uintptr_t)a > (uintptr_t)b) {
                return false;
            }
        }

        prev = cur;
        cur = cur->next;
    }

    return true;
}

static void _prepare_unsorted_clist(size_t len)
{
    TEST_ASSERT(len <= ARRAY_SIZE(tests_clist_buf));
    /*
     * Test data generated using following python snippet:
     *
     * import random
     *
     * random.seed(1337)
     * for i in range(8):
     *     result = " " * 8
     *     for i in range(7):
     *         result += f"0x{random.getrandbits(8):02x},   "
     *     result += f"0x{random.getrandbits(8):02x},"
     *     print(result)
     */
    static const unsigned values[] = {
        0x9e,   0xec,   0x88,   0xb5,   0x5d,   0x92,   0x95,   0xbb,
        0x2a,   0xc6,   0xd3,   0x55,   0x62,   0xa2,   0xca,   0x5c,
        0xeb,   0xfe,   0x4e,   0x64,   0xfe,   0xb2,   0x34,   0xc2,
        0xa8,   0xdd,   0xe9,   0x5d,   0x1b,   0x6c,   0xd2,   0xa2,
        0x66,   0xe6,   0x10,   0x81,   0xb2,   0xab,   0x59,   0xe3,
        0x67,   0x67,   0xcd,   0xbc,   0xcc,   0x4f,   0xa9,   0x04,
        0xd5,   0x5a,   0x98,   0x1e,   0x75,   0x3f,   0xf5,   0x2b,
        0xcb,   0xda,   0x50,   0xad,   0xb0,   0xeb,   0xea,   0x9a,
    };
    static_assert(ARRAY_SIZE(tests_clist_buf) <= ARRAY_SIZE(values),
                  "Not enough test data for sort test");

    set_up();
    clist_node_t *list = &test_clist;
    for (size_t i = 0; i < len; i++) {
        tests_clist_buf[i].value = values[i];
        clist_rpush(list, &tests_clist_buf[i].list);
    }
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

    for (size_t cur_len = 0; cur_len < TEST_CLIST_LEN; cur_len++) {
        _prepare_unsorted_clist(cur_len);
        clist_sort(list, _cmp);
        TEST_ASSERT(_is_clist_stable_sorted(list));
        TEST_ASSERT_EQUAL_INT(cur_len, clist_count(list));
    }
}

static void test_clist_count(void)
{
    size_t n = clist_count(&test_clist);
    TEST_ASSERT(n == 0);

    for (unsigned i = 1; i <= TEST_CLIST_LEN; i++) {
        clist_rpush(&test_clist, &tests_clist_buf[i - 1].list);
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
        clist_rpush(&test_clist, &tests_clist_buf[i - 1].list);
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

static void test_clist_special_cardinality(void)
{
    unsigned i = 0;
    TEST_ASSERT(clist_is_empty(&test_clist));
    TEST_ASSERT(!clist_exactly_one(&test_clist));
    TEST_ASSERT(!clist_more_than_one(&test_clist));

    clist_rpush(&test_clist, &tests_clist_buf[i++].list);

    TEST_ASSERT(!clist_is_empty(&test_clist));
    TEST_ASSERT(clist_exactly_one(&test_clist));
    TEST_ASSERT(!clist_more_than_one(&test_clist));

    while (i < TEST_CLIST_LEN) {
        clist_rpush(&test_clist, &tests_clist_buf[i++].list);

        TEST_ASSERT(!clist_is_empty(&test_clist));
        TEST_ASSERT(!clist_exactly_one(&test_clist));
        TEST_ASSERT(clist_more_than_one(&test_clist));
    }
    do {
        TEST_ASSERT(!clist_is_empty(&test_clist));
        TEST_ASSERT(!clist_exactly_one(&test_clist));
        TEST_ASSERT(clist_more_than_one(&test_clist));

        clist_lpop(&test_clist);
    } while (--i > 1);

    TEST_ASSERT(!clist_is_empty(&test_clist));
    TEST_ASSERT(clist_exactly_one(&test_clist));
    TEST_ASSERT(!clist_more_than_one(&test_clist));

    clist_lpop(&test_clist);

    TEST_ASSERT(clist_is_empty(&test_clist));
    TEST_ASSERT(!clist_exactly_one(&test_clist));
    TEST_ASSERT(!clist_more_than_one(&test_clist));
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
        new_TestFixture(test_clist_sort),
        new_TestFixture(test_clist_count),
        new_TestFixture(test_clist_is_empty),
        new_TestFixture(test_clist_special_cardinality),
    };

    EMB_UNIT_TESTCALLER(core_clist_tests, set_up, NULL,
                        fixtures);

    return (Test *)&core_clist_tests;
}
