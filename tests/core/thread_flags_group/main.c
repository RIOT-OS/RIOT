/*
 * Copyright (C) 2025 Mihai Renea <mihai.renea@ml-pa.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#define WAITERS_CNT 2

static char stacks[WAITERS_CNT][THREAD_STACKSIZE_MAIN];

#define GOOD_FLAG 0x2
#define BAD_FLAG  0x4

static uint8_t woken_up = 0;
static uint8_t last_prio = 0;
static tfg_t group = THREAD_FLAGS_GROUP_INIT;

static void *waiter(void *arg)
{
    // tfg_entry_t entry;

    thread_flags_group_join(&group);

    printf("waiter %u waiting...\n", (unsigned)arg);
    thread_flags_wait_any(GOOD_FLAG);

    printf("waiter %u woken up!\n", (unsigned)arg);

    expect(atomic_load_u8(&last_prio) < thread_get_active()->priority);

    atomic_store_u8(&last_prio, thread_get_active()->priority);
    atomic_fetch_add_u8(&woken_up, 1);

    return NULL;
}

int main(void)
{
    puts("START");
    for (unsigned i = 0; i < WAITERS_CNT; i++) {
        int res = thread_create(stacks[i], THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN - i - 1,
                                THREAD_CREATE_STACKTEST, waiter, (void *)i,
                                "waiter");
        expect(res >= 0);
    }

    /* this shouldn't wake up */
    thread_flags_group_set(&group, BAD_FLAG);
    expect(atomic_load_u8(&woken_up) == 0);

    puts("waking up!");

    thread_flags_group_set(&group, GOOD_FLAG);

    /* waiters have higher prio, so they must have finished */
    expect(atomic_load_u8(&woken_up) == WAITERS_CNT);

    puts("SUCCESS");

    return 0;
}
