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
#  define BENCH_CLIST_RUNS (BENCH_RUNS / 1000UL)
#endif

static mutex_t _lock;
static thread_t *t;
static thread_flags_t _flag = 0x0001;
static msg_t _msg;

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

struct test_node {
    clist_node_t list;
    int value;
};

static clist_node_t clist;

static void _build_test_clist(size_t len)
{
    static struct test_node nodes[128];

    assert(len <= ARRAY_SIZE(nodes));

    memset(nodes, 0, sizeof(nodes));
    clist.next = NULL;

    for (size_t i = 0; i < len; i++) {
        clist_rpush(&clist, &nodes[i].list);
    }
}

static void _insert_unsorted_data(clist_node_t *list)
{
    clist_node_t *last = list->next;
    if (!last) {
        return;
    }
    unsigned val = 128;

    clist_node_t *cur = last->next;
    while (cur != last) {
        struct test_node *n = container_of(cur, struct test_node, list);

        /* This is the worst case for most sorting algorithms: having the
         * list perfectly sorted but in inverse direction. For real time, we
         * only care about worst case and not about best case / average case */
        n->value = val--;
        cur = cur->next;
    }
}

static int _cmp(clist_node_t *_lhs, clist_node_t *_rhs)
{
    struct test_node *lhs = container_of(_lhs, struct test_node, list);
    struct test_node *rhs = container_of(_rhs, struct test_node, list);

    return lhs->value - rhs->value;
}

static void _clist_sort_test(void)
{
    clist_node_t *list = &clist;

    _insert_unsorted_data(list);
    clist_sort(list, _cmp);
}

static void _bench_clist_sort(unsigned size)
{
    char name[32] = {};
    snprintf(name, sizeof(name) - 1, "clist_sort() (%u nodes)", size);
    _build_test_clist(size);
    BENCHMARK_FUNC(name, BENCH_CLIST_RUNS, _clist_sort_test());
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
    _bench_clist_sort(8);
    _bench_clist_sort(32);
    _bench_clist_sort(128);

    puts("\n[SUCCESS]");
    return 0;
}
