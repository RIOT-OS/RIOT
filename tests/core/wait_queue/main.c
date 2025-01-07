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
#include "test_utils/expect.h"

#define WAITERS_CNT 5

static char stacks[WAITERS_CNT][THREAD_STACKSIZE_MAIN];

static wait_queue_t wq = WAIT_QUEUE_INIT;

static uint64_t cond_val = 0;
static sema_t woken_cnt = SEMA_CREATE(0);

#define COND_ITER_CNT_INIT (CONFIG_QUEUE_WAIT_EARLY_EXIT ? (uint32_t)-WAITERS_CNT : 0)
static uint32_t cond_iter_cnt = COND_ITER_CNT_INIT;

/* increment a counter before the condition expression is evaluated */
#define COUNTING_COND(cnt, expr) (atomic_fetch_add_u32(&(cnt), 1), expr)

/* assert interrupts enabled before evaluating the expression */
#define IRQ_ON(expr) (expect(irq_is_enabled()), expr)

/* assert interrupts disabled before evaluating the expression */
#define IRQ_OFF(expr) (expect(!irq_is_enabled()), expr)

#define COND_VAL_THRESHOLD (UINT32_MAX + 1ULL)

static void timeout_cb(void *arg)
{
    bool broadcast = arg;

    cond_val = COND_VAL_THRESHOLD;
    if (broadcast) {
        queue_wake(&wq);
    } else {
        queue_wake_exclusive(&wq);
    }
}

static void *waiter_nonblocking(void *arg)
{
    QUEUE_WAIT(&wq, COUNTING_COND(cond_iter_cnt,
                                  IRQ_ON(atomic_load_u64(&cond_val) >= COND_VAL_THRESHOLD)));

    sema_post(&woken_cnt);
    printf("waiter %u awake!\n", (unsigned)arg);

    return NULL;
}

static void *waiter_nonblocking_irqdisabled(void *arg)
{
    irq_disable();
    QUEUE_WAIT(&wq, COUNTING_COND(cond_iter_cnt, IRQ_OFF(cond_val >= COND_VAL_THRESHOLD)));
    expect(!irq_is_enabled());
    irq_enable();

    sema_post(&woken_cnt);
    printf("waiter %u awake!\n", (unsigned)arg);

    return NULL;
}

void test_waiters_nonblocking(thread_task_func_t waiter_func)
{
    cond_val = 0;
    cond_iter_cnt = COND_ITER_CNT_INIT;

    int thread_ids[WAITERS_CNT];
    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        thread_ids[i] = thread_create(stacks[i], sizeof(stacks[0]), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, waiter_func, (void *)i, "waiter");
        expect(thread_ids[i] >= 0);
    }

    /* waiters have higher prio, they should all be enqueued now */
    expect(wq.list != WAIT_QUEUE_TAIL);
    uint32_t cond_iter_cnt_expected = WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    ztimer_t timer = {
        .callback = timeout_cb,
        .arg = (void *)false,  // wake exclusive
    };
    ztimer_set(ZTIMER_MSEC, &timer, 100);

    ztimer_sleep(ZTIMER_MSEC, 50);

    int ret = sema_try_wait(&woken_cnt);
    expect(ret == -EAGAIN);

    /* Wake all, but they should go back to sleep because the condition is not
     * met yet */
    queue_wake(&wq);

    sema_try_wait(&woken_cnt);
    expect(ret == -EAGAIN);

    cond_iter_cnt_expected += WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    ret = sema_wait_timed_ztimer(&woken_cnt, ZTIMER_MSEC, 51);
    expect(ret == 0);

    cond_iter_cnt_expected += 1;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    ztimer_sleep(ZTIMER_MSEC, 10);

    ret = sema_try_wait(&woken_cnt);
    expect(ret == -EAGAIN);
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    cond_val = 0;

    /* Wake all, but they should go back to sleep because the condition is not
     * met yet */
    queue_wake(&wq);

    cond_iter_cnt_expected += WAITERS_CNT - 1;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    ret = sema_try_wait(&woken_cnt);
    expect(ret == -EAGAIN);

    timer.arg = (void *)true; // wake all
    ztimer_set(ZTIMER_MSEC, &timer, 10);

    for (unsigned i = 0; i < (WAITERS_CNT - 1); i++) {
        ret = sema_wait_timed_ztimer(&woken_cnt, ZTIMER_MSEC, 11);
        expect(ret == 0);
    }

    cond_iter_cnt_expected += WAITERS_CNT - 1;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    /* just to make sure all threads finish */
    ztimer_sleep(ZTIMER_MSEC, 2);
}

void test_waiters_lowprio(void)
{
    cond_val = 0;
    cond_iter_cnt = COND_ITER_CNT_INIT;

    int thread_ids[WAITERS_CNT];
    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        thread_ids[i] = thread_create(stacks[i], sizeof(stacks[0]), THREAD_PRIORITY_MAIN + 1,
                      THREAD_CREATE_STACKTEST, waiter_nonblocking, (void *)i, "waiter");
        expect(thread_ids[i] >= 0);
    }

    ztimer_sleep(ZTIMER_MSEC, 2);

    /* they should all be enqueued now */
    expect(wq.list != WAIT_QUEUE_TAIL);
    uint32_t cond_iter_cnt_expected = WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    cond_val = COND_VAL_THRESHOLD;
    queue_wake(&wq);

    expect(wq.list == WAIT_QUEUE_TAIL);

    /* busy-wait for a while */
    for (unsigned i = 0; i < 1000000; i++) {
        expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);
    }

    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        int ret = sema_wait_timed_ztimer(&woken_cnt, ZTIMER_MSEC, 11);
        expect(ret == 0);
    }

    cond_iter_cnt_expected += WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    /* just to make sure all threads finish */
    ztimer_sleep(ZTIMER_MSEC, 2);
}

#if !CONFIG_QUEUE_WAIT_EARLY_EXIT

static mutex_t cond_mutex = MUTEX_INIT;

static bool cond_fn_mutex(void)
{
    mutex_lock(&cond_mutex);
    bool ret = atomic_load_u64(&cond_val) >= COND_VAL_THRESHOLD;
    mutex_unlock(&cond_mutex);

    return ret;
}

static wait_queue_t nested_wq = WAIT_QUEUE_INIT;
static uint32_t nested_cond_iter_cnt = 0;
static uint64_t nested_cond_val = 0;

static bool cond_fn_wq(unsigned waiter_no)
{
    QUEUE_WAIT(&nested_wq, COUNTING_COND(nested_cond_iter_cnt,
                                         atomic_load_u64(&nested_cond_val) >= COND_VAL_THRESHOLD));
    printf("waiter %u: past inner wq\n", waiter_no);
    return atomic_load_u64(&cond_val) >= COND_VAL_THRESHOLD;
}

static void *waiter_blocking_queue(void *arg)
{
    QUEUE_WAIT(&wq, COUNTING_COND(cond_iter_cnt, cond_fn_wq((unsigned)arg)));

    sema_post(&woken_cnt);
    printf("waiter %u awake!\n", (unsigned)arg);

    return NULL;
}

static void *waiter_blocking_mutex(void *arg)
{
    QUEUE_WAIT(&wq, COUNTING_COND(cond_iter_cnt, cond_fn_mutex()));

    sema_post(&woken_cnt);
    printf("waiter %u awake!\n", (unsigned)arg);

    return NULL;
}

void test_waiters_blocking_mutex(void)
{
    cond_val = 0;
    cond_iter_cnt = 0;

    mutex_lock(&cond_mutex);

    int thread_ids[WAITERS_CNT];
    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        thread_ids[i] = thread_create(stacks[i], sizeof(stacks[0]), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, waiter_blocking_mutex, (void *)i, "waiter");
        expect(thread_ids[i] >= 0);
    }

    /* waiters have higher prio, they should all be enqueued now */
    expect(wq.list != WAIT_QUEUE_TAIL);
    uint32_t cond_iter_cnt_expected = WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    /* wake up all, but since they block on the mutex they should stay asleep */
    cond_val = COND_VAL_THRESHOLD;
    queue_wake(&wq);
    expect(wq.list == WAIT_QUEUE_TAIL);
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    /* they should all wake up now and do another condition check, then wait
     * once again */
    cond_val = 0;
    mutex_unlock(&cond_mutex);

    cond_iter_cnt_expected += WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    cond_val = COND_VAL_THRESHOLD;
    queue_wake(&wq);

    cond_iter_cnt_expected += WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        int ret = sema_try_wait(&woken_cnt);
        expect(ret == 0);
    }

    /* just to make sure all threads finish */
    ztimer_sleep(ZTIMER_MSEC, 2);
}

void test_waiters_blocking_wq(void)
{
    /* condition is already met, but the expression will block on the inner
     * wait queue */
    cond_val = COND_VAL_THRESHOLD;
    cond_iter_cnt = 0;
    nested_cond_val = 0;

    int thread_ids[WAITERS_CNT];
    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        thread_ids[i] = thread_create(stacks[i], sizeof(stacks[0]), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, waiter_blocking_queue, (void *)i, "waiter");
        expect(thread_ids[i] >= 0);
    }

    nested_cond_val = COND_VAL_THRESHOLD;
    /* waiters have higher prio, they should all be enqueued now */
    expect(wq.list != WAIT_QUEUE_TAIL);
    uint32_t cond_iter_cnt_expected = WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    expect(nested_wq.list != WAIT_QUEUE_TAIL);
    uint32_t nested_cond_iter_cnt_expected = WAITERS_CNT;
    expect(atomic_load_u32(&nested_cond_iter_cnt) == nested_cond_iter_cnt_expected);

    /* wake up all, but since they block on the inner wq they should stay asleep */
    cond_val = COND_VAL_THRESHOLD;
    queue_wake(&wq);
    expect(wq.list == WAIT_QUEUE_TAIL);
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    /* nothing should have changed on the nested wait queue */
    expect(nested_wq.list != WAIT_QUEUE_TAIL);
    expect(atomic_load_u32(&nested_cond_iter_cnt) == nested_cond_iter_cnt_expected);

    /* they should all wake up now and do another NESTED condition check, then wait
     * once again */
    printf("wake up nested 1\n");
    nested_cond_val = 0;
    queue_wake(&nested_wq);


    nested_cond_iter_cnt_expected += WAITERS_CNT;
    expect(nested_wq.list != WAIT_QUEUE_TAIL);
    expect(atomic_load_u32(&nested_cond_iter_cnt) == nested_cond_iter_cnt_expected);

    /* nothing should have changed on the OUTER wait queue */
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);
    expect(wq.list == WAIT_QUEUE_TAIL);

    /* The NESTED wait loop should finish, but the condition for the OUTER
     * wait queue is not met */
    printf("wake up nested 2\n");
    cond_val = 0;
    nested_cond_val = COND_VAL_THRESHOLD;
    queue_wake(&nested_wq);

    cond_iter_cnt_expected += WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    expect(nested_wq.list == WAIT_QUEUE_TAIL);
    expect(wq.list != WAIT_QUEUE_TAIL);

    cond_val = COND_VAL_THRESHOLD;
    queue_wake(&wq);

    cond_iter_cnt_expected += WAITERS_CNT;
    expect(atomic_load_u32(&cond_iter_cnt) == cond_iter_cnt_expected);

    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        int ret = sema_try_wait(&woken_cnt);
        expect(ret == 0);
    }

    /* just to make sure all threads finish */
    ztimer_sleep(ZTIMER_MSEC, 2);
}

#endif

int main(void)
{
    test_waiters_nonblocking(waiter_nonblocking);
    puts("tested non-blocking condition expression");

    test_waiters_nonblocking(waiter_nonblocking_irqdisabled);
    puts("tested non-blocking condition expression (IRQ disabled)");

    test_waiters_lowprio();
    puts("tested non-blocking condition expression (waiters have low prio)");

#if !CONFIG_QUEUE_WAIT_EARLY_EXIT
    test_waiters_blocking_mutex();
    puts("tested condition expression blocking on mutex");

    test_waiters_blocking_wq();
    puts("tested condition expression blocking on a nested wait queue");
#endif

    puts("-----\nTest successful!");

    return 0;
}
