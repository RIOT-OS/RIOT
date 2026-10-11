/*
 * SPDX-FileCopyrightText: 2025 Mihai Renea <mihai.renea@ml-pa.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   wait queue test application
 *
 * @author  Mihai Renea <mihai.renea@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "atomic_utils.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "wait_queue.h"

#define WAITERS_CNT 2

static char stacks[WAITERS_CNT][THREAD_STACKSIZE_MAIN];
static uint8_t woken_up = 0;
static wait_queue_t queue = WAIT_QUEUE_INIT;

#define THRESHOLD ((uint64_t)UINT32_MAX + 1)
static uint64_t condition = 0;

#define DEBUG_COND_EXEC(waiter, cond) (printf("waiter %d cond check\n", waiter), cond)

static void *waiter(void *arg)
{
    unsigned waiter = (unsigned)(uintptr_t)arg;

    int val = QUEUE_WAIT_ZTIMER(ZTIMER_MSEC, 10, &queue,
                                DEBUG_COND_EXEC(waiter, atomic_load_u64(&condition) >= THRESHOLD));
    expect(val == 0);
    printf("waiter %u continues (timeout)\n", waiter);
    atomic_fetch_add_u8(&woken_up, 1);

    val = QUEUE_WAIT_ZTIMER(ZTIMER_MSEC, 500, &queue,
                            DEBUG_COND_EXEC(waiter, atomic_load_u64(&condition) >= THRESHOLD));
    expect(val == 1);
    printf("waiter %u continues (cond met)\n", waiter);
    atomic_fetch_add_u8(&woken_up, 1);

    QUEUE_WAIT(&queue, DEBUG_COND_EXEC(waiter, atomic_load_u64(&condition) >= THRESHOLD + 1));
    printf("waiter %u continues (cond met)\n", waiter);
    atomic_fetch_add_u8(&woken_up, 1);

    return NULL;
}

int main(void)
{
    puts("START");
    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        int res = thread_create(stacks[i], THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN - i - 1,
                                THREAD_CREATE_STACKTEST, waiter,
                                (void *)(uintptr_t)i,
                                "waiter");
        expect(res >= 0);
    }
    expect(atomic_load_u8(&woken_up) == 0);

    /* This will wake up the waiters but they won't continue since the condition
     * is not met. */
    printf("waking up, condition=%" PRIu64 "\n", condition);
    queue_wake_all(&queue);
    expect(atomic_load_u8(&woken_up) == 0);

    /* trigger timeout */
    ztimer_sleep(ZTIMER_MSEC, 15);
    expect(atomic_load_u8(&woken_up) == WAITERS_CNT);

    atomic_store_u64(&condition, THRESHOLD);
    printf("waking up, condition=%" PRIu64 "\n", condition);
    queue_wake_all(&queue);
    expect(atomic_load_u8(&woken_up) == WAITERS_CNT * 2);

    atomic_store_u64(&condition, THRESHOLD + 1);
    printf("waking up, condition=%" PRIu64 "\n", condition);
    queue_wake_all(&queue);

    expect(atomic_load_u8(&woken_up) == WAITERS_CNT * 3);

    puts("SUCCESS");

    return 0;
}
