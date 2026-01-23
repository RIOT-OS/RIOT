/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   thread flags group test application
 *
 * @author  Mihai Renea <mihai.renea@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "atomic_utils.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "thread_flags_group.h"

#ifndef WAITER_THREADS_CNT
#  define WAITER_THREADS_CNT 3
#endif

#ifdef CPU_NATIVE
#  define WAITER_STACKSIZE THREAD_STACKSIZE_MAIN
#else
#  define WAITER_STACKSIZE THREAD_STACKSIZE_SMALL
#endif

static char stacks[WAITER_THREADS_CNT][WAITER_STACKSIZE];

#define GOOD_FLAG 0x2
#define BAD_FLAG  0x4

static uint8_t woken_up = 0;
static uint8_t last_prio = 0;
static thread_flags_group_t group = THREAD_FLAGS_GROUP_INIT;

static void _print_waiting(char const *what_who, kernel_pid_t pid)
{
#ifdef CPU_NATIVE
    printf("%s %d\n", what_who, pid);
#else
    puts(what_who);
    (void)pid;
#endif
}

static void *forever_waiter(void *arg)
{
    _print_waiting("waiting forever-waiter", (kernel_pid_t)(uintptr_t)arg);
    thread_flags_wait_any(GOOD_FLAG | BAD_FLAG);
    expect(false);
}

static void *waiter(void *arg)
{
    thread_flags_group_join(&group);

    _print_waiting("waiting waiter", (kernel_pid_t)(uintptr_t)arg);
    thread_flags_wait_any(GOOD_FLAG);

    _print_waiting("woken up waiter", (kernel_pid_t)(uintptr_t)arg);

    expect(atomic_load_u8(&last_prio) <= thread_get_active()->priority);

    atomic_store_u8(&last_prio, thread_get_active()->priority);
    atomic_fetch_add_u8(&woken_up, 1);

    return NULL;
}

int main(void)
{
    puts("START");
    unsigned waiters_cnt = 0;
    for (unsigned i = 0; i < WAITER_THREADS_CNT; i++) {
        int prio = (int)THREAD_PRIORITY_MAIN - i - 1;
        if (prio < 0) {
            prio = 0;
        }

        thread_task_func_t handler = i % 3 ? (waiters_cnt++, waiter) : forever_waiter;
        int res = thread_create(stacks[i], sizeof(stacks[0]),
                                prio, THREAD_CREATE_STACKTEST, handler,
                                (void *)(uintptr_t)i, "waiter");
        expect(res >= 0);
    }

    /* this shouldn't wake up */
    thread_flags_group_set(&group, BAD_FLAG);
    expect(atomic_load_u8(&woken_up) == 0);

    puts("waking up!");

    thread_flags_group_set(&group, GOOD_FLAG);

    /* waiters have higher prio, so they must have finished */
    expect(atomic_load_u8(&woken_up) == waiters_cnt);

    puts("SUCCESS");

    return 0;
}
