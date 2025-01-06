/*
 * Copyright (C) 2025 Mihai Renea
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
 * @brief       Test application for wait queues
 *
 * @author      Mihai Renea <mihairenea@gmail.com>
 * @}
 */

#include "wait_queue.h"

#include <errno.h>
#include <stdio.h>

#include "ztimer.h"
#include "wait_queue.h"
#include "atomic_utils.h"
#include "sema.h"

#define WAITERS_CNT 5

static wait_queue_t wq = WAIT_QUEUE_INIT;

static uint64_t rise_and_shine_mr_freeman = 0;
static sema_t woken_cnt = SEMA_CREATE(0);

static void timeout_cb(void *arg)
{
    bool broadcast = arg;

    rise_and_shine_mr_freeman = UINT32_MAX + 1ULL;
    if (broadcast) {
        queue_wake(&wq);
    } else {
        queue_wake_exclusive(&wq);
    }
}

static mutex_t cond_mutex = MUTEX_INIT;

static bool cond_fn(void)
{
    mutex_lock(&cond_mutex);
    bool ret = atomic_load_u64(&rise_and_shine_mr_freeman) > UINT32_MAX;
    mutex_unlock(&cond_mutex);

    return ret;
}

static void *waiter(void *arg)
{
    (void)arg;
    // QUEUE_WAIT(&wq, cond_fn());

    do {
      (void)(0);
      wait_queue_entry_t me;
      _prepare_to_wait(&wq, &me);
      while (!(cond_fn())) {
        _maybe_yield_and_enqueue(&wq, &me);
      }
      _wait_dequeue(&wq, &me);
    } while (0);

    sema_post(&woken_cnt);
    puts("woken!");
    return NULL;
}

static char stacks[WAITERS_CNT][THREAD_STACKSIZE_MAIN];

int main(void)
{
    printf("hello\n");

    mutex_lock(&cond_mutex);

    int thread_ids[WAITERS_CNT];
    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        thread_ids[i] = thread_create(stacks[i], sizeof(stacks[0]), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, waiter, NULL, "waiter");
        assert(thread_ids[i] >= 0);
    }


    ztimer_t timer = { .callback = timeout_cb, .arg = (void *)false };
    ztimer_set(ZTIMER_MSEC, &timer, 100);

    ztimer_sleep(ZTIMER_MSEC, 50);

    int ret = sema_try_wait(&woken_cnt);
    assert(ret == -EAGAIN);

    // return 0;

    queue_wake(&wq);

    printf("cond mutex unlocked\n");
    mutex_unlock(&cond_mutex);

    ret = sema_wait_timed_ztimer(&woken_cnt, ZTIMER_MSEC, 51);
    assert(ret == 0);
    ztimer_sleep(ZTIMER_MSEC, 10);

    ret = sema_try_wait(&woken_cnt);
    assert(ret == -EAGAIN);

    rise_and_shine_mr_freeman = 0;

    printf("main wake\n");
    queue_wake(&wq);
    ztimer_sleep(ZTIMER_MSEC, 50);

    ret = sema_try_wait(&woken_cnt);
    assert(ret == -EAGAIN);

    timer.arg = (void *)true;
    ztimer_set(ZTIMER_MSEC, &timer, 10);

    for (unsigned i = 0; i < (WAITERS_CNT - 1); i++) {
        ret = sema_wait_timed_ztimer(&woken_cnt, ZTIMER_MSEC, 11);
        assert(ret == 0);
    }

    puts("Test successful!\n");

    return 0;
}
