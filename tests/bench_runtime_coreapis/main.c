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

#include "mutex.h"
#include "benchmark.h"
#include "thread.h"
#include "thread_flags.h"

#ifndef BENCH_RUNS
#define BENCH_RUNS          (1000UL * 1000UL)
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

    puts("\n[SUCCESS]");
    return 0;
}
