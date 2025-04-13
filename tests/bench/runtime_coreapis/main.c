/*
 * Copyright (C) 2018 Freie Universität Berlin
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
 * @brief       Measure runtime of select core API functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "benchmark.h"
#include "mutex.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "thread_flags.h"

#ifndef BENCH_RUNS
#  define BENCH_RUNS          (1000UL * 1000UL)
#endif
#ifndef BENCH_CLIST_RUNS
#  if defined(BOARD_NATIVE64) || defined(BOARD_NATIVE32)
#    define BENCH_CLIST_RUNS BENCH_RUNS
#  else
#    define BENCH_CLIST_RUNS (BENCH_RUNS / 1000UL)
#  endif
#endif

#ifndef BENCH_CLIST_SORT_TEST_NODES
#  define BENCH_CLIST_SORT_TEST_NODES 256
#endif

struct test_node {
    clist_node_t list;
    int value;
};

static mutex_t _lock;
static thread_t *t;
static thread_flags_t _flag = 0x0001;
static msg_t _msg;
static struct test_node nodes[BENCH_CLIST_SORT_TEST_NODES];

static void _mutex_lockunlock(void)
{
    mutex_lock(&_lock);
    mutex_unlock(&_lock);
}

static void _flag_waitany(void)
{
    thread_flags_set(t, _flag);
    thread_flags_wait_any(_flag);
}

static void _flag_waitall(void)
{
    thread_flags_set(t, _flag);
    thread_flags_wait_all(_flag);
}

static void _flag_waitone(void)
{
    thread_flags_set(t, _flag);
    thread_flags_wait_one(_flag);
}

static clist_node_t clist;

static int _insert_reversed_data(clist_node_t *item, void *arg)
{
    unsigned *val = arg;
    struct test_node *n = container_of(item, struct test_node, list);
    n->value = *val--;
    return 0;
}

static int _insert_fully_sorted_data(clist_node_t *item, void *arg)
{
    unsigned *val = arg;
    struct test_node *n = container_of(item, struct test_node, list);
    n->value = *val++;
    return 0;
}

static int _insert_almost_sorted_data(clist_node_t *item, void *arg)
{
    unsigned *val = arg;
    struct test_node *n = container_of(item, struct test_node, list);
    if (*val == 3) {
        n->value = BENCH_CLIST_SORT_TEST_NODES;
    }
    else {
        n->value = *val;
    }
    *val += 1;
    return 0;
}

static uint16_t _prng_next(uint16_t val)
{
    val ^= val << 7;
    val ^= val >> 9;
    val ^= val << 8;
    return val;
}

static int _insert_prng_data(clist_node_t *item, void *arg)
{
    uint16_t *seed = arg;
    struct test_node *n = container_of(item, struct test_node, list);
    *seed = _prng_next(*seed);

    /* By masking off some bits from the full sequence PRNG input we introduce
     * duplicates: 2 dups in 64 items, 3 in 128 items, 20 in 256 items.
     *
     * The API explicitly says stable sort, so we better have some input data
     * where this attribute would make a difference. */
    const uint16_t dup_mask = 0x3ff;
    n->value = *seed & dup_mask;

    return 0;
}

static int _cmp(clist_node_t *_lhs, clist_node_t *_rhs)
{
    struct test_node *lhs = container_of(_lhs, struct test_node, list);
    struct test_node *rhs = container_of(_rhs, struct test_node, list);

    return lhs->value - rhs->value;
}

static void _build_test_clist(size_t len)
{

    assert(len <= ARRAY_SIZE(nodes));

    memset(nodes, 0, sizeof(nodes));
    clist.next = NULL;

    for (size_t i = 0; i < len; i++) {
        clist_rpush(&clist, &nodes[i].list);
    }
}

static void _clist_sort_test_reversed(void)
{
    clist_node_t *list = &clist;

    unsigned value = BENCH_CLIST_SORT_TEST_NODES;
    clist_foreach(list, _insert_reserved_data, &value);
    clist_sort(list, _cmp);
}

static void _clist_sort_test_prng(void)
{
    clist_node_t *list = &clist;

    uint16_t seed = 1337; /* What else as seed? ;-P */
    clist_foreach(list, _insert_prng_data, &seed);
    clist_sort(list, _cmp);
}

static void _clist_sort_test_full_sorted(void)
{
    clist_node_t *list = &clist;

    unsigned value = 0;
    clist_foreach(list, _insert_fully_sorted_data, &value);
    clist_sort(list, _cmp);
}

static void _clist_sort_test_almost_sorted(void)
{
    clist_node_t *list = &clist;

    unsigned value = 0;
    clist_foreach(list, _insert_almost_sorted_data, &value);
    clist_sort(list, _cmp);
}

static bool _is_list_sorted(clist_node_t *list)
{
    /* empty list is always considered as sorted */
    if (!list->next) {
        return true;
    }

    clist_node_t *end_of_clist = list->next;
    clist_node_t *prev = list->next->next;
    clist_node_t *cur = prev->next;

    while (prev != end_of_clist) {
        struct test_node *lhs = container_of(prev, struct test_node, list);
        struct test_node *rhs = container_of(cur, struct test_node, list);

        if (lhs->value > rhs->value) {
            return false;
        }

        prev = cur;
        cur = cur->next;
    }

    return true;
}

static void _bench_clist_sort(unsigned size)
{
    char name[48] = {};
    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, rev)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_reversed());
    expect(_is_list_sorted(&clist));

    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, prng)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_prng());
    expect(_is_list_sorted(&clist));

    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, sort)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_full_sorted());
    expect(_is_list_sorted(&clist));

    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, ≈sort)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_almost_sorted());
    expect(_is_list_sorted(&clist));
}

int main(void)
{
    puts("Runtime of Selected Core API functions\n");

    t = thread_get_active();

    BENCHMARK_FUNC("nop loop", BENCH_RUNS, __asm__ volatile ("nop"));
    puts("");
    BENCHMARK_FUNC("mutex_init()", BENCH_RUNS, mutex_init(&_lock));
    BENCHMARK_FUNC("mutex lock/unlock", BENCH_RUNS, _mutex_lockunlock());
    puts("");
    BENCHMARK_FUNC("thread_flags_set()", BENCH_RUNS, thread_flags_set(t, _flag));
    BENCHMARK_FUNC("thread_flags_clear()", BENCH_RUNS, thread_flags_clear(_flag));
    BENCHMARK_FUNC("thread flags set/wait any", BENCH_RUNS, _flag_waitany());
    BENCHMARK_FUNC("thread flags set/wait all", BENCH_RUNS, _flag_waitall());
    BENCHMARK_FUNC("thread flags set/wait one", BENCH_RUNS, _flag_waitone());
    puts("");
    BENCHMARK_FUNC("msg_try_receive()", BENCH_RUNS, msg_try_receive(&_msg));
    BENCHMARK_FUNC("msg_avail()", BENCH_RUNS, msg_avail());
    puts("");

    for (unsigned len = 4; len <= BENCH_CLIST_SORT_TEST_NODES; len <<= 1) {
       _bench_clist_sort(len);
    }

   puts("\n[SUCCESS]");
}
