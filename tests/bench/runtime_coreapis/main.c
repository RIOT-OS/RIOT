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

#include "mutex.h"
#include "benchmark.h"
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

static void _insert_reserved_data(clist_node_t *list)
{
    clist_node_t *last = list->next;
    if (!last) {
        return;
    }
    unsigned val = BENCH_CLIST_SORT_TEST_NODES - 1;

    clist_node_t *cur = last->next;
    while (cur != last) {
        struct test_node *n = container_of(cur, struct test_node, list);

        n->value = val--;
        cur = cur->next;
    }
    struct test_node *n = container_of(last, struct test_node, list);
    n->value = BENCH_CLIST_SORT_TEST_NODES;
}

static void _insert_fully_sorted_data(clist_node_t *list)
{
    clist_node_t *last = list->next;
    if (!last) {
        return;
    }
    unsigned val = 0;

    clist_node_t *cur = last->next;
    while (cur != last) {
        struct test_node *n = container_of(cur, struct test_node, list);

        n->value = val++;
        cur = cur->next;
    }

    struct test_node *n = container_of(last, struct test_node, list);
    n->value = val;
}

static void _insert_almost_sorted_data(clist_node_t *list)
{
    clist_node_t *last = list->next;
    if (!last) {
        return;
    }
    unsigned val = 0;

    clist_node_t *cur = last->next;
    while (cur != last) {
        struct test_node *n = container_of(cur, struct test_node, list);

        if (val == 3) {
            n->value = BENCH_CLIST_SORT_TEST_NODES;
        }
        else {
            n->value = val;
        }
        val++;
        cur = cur->next;
    }

    struct test_node *n = container_of(last, struct test_node, list);
    n->value = 3;
}

static void _insert_prng_data(clist_node_t *list)
{
    clist_node_t *last = list->next;
    if (!last) {
        return;
    }

    uint32_t prng = 1337; /* what else as seed? */
    clist_node_t *cur = last->next;

    while (cur != last) {
        struct test_node *n = container_of(cur, struct test_node, list);

        prng ^= prng << 13;
        prng ^= prng >> 17;
        prng ^= prng << 5;
        n->value = prng;
        cur = cur->next;
    }
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

    /* shuffle the elements a bit by adding random data and sorting it */
    _insert_prng_data(&clist);
    clist_sort(&clist, _cmp);
}

static void _clist_sort_test_reversed(void)
{
    clist_node_t *list = &clist;

    _insert_reserved_data(list);
    clist_sort(list, _cmp);
}

static void _clist_sort_test_prng(void)
{
    clist_node_t *list = &clist;

    _insert_prng_data(list);
    clist_sort(list, _cmp);
}

static void _clist_sort_test_full_sorted(void)
{
    clist_node_t *list = &clist;

    _insert_fully_sorted_data(list);
    clist_sort(list, _cmp);
}

static void _clist_sort_test_almost_sorted(void)
{
    clist_node_t *list = &clist;

    _insert_almost_sorted_data(list);
    clist_sort(list, _cmp);
}

static void _bench_clist_sort(unsigned size)
{
    char name[48] = {};
    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, rev)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_reversed());

    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, prng)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_prng());

    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, sort)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_full_sorted());

    snprintf(name, sizeof(name) - 1, "clist_sort (#%u, ≈sort)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test_almost_sorted());
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
    return 0;
}
