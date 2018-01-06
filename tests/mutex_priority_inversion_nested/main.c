/*
 * Copyright (C) 2017 Technische Universität Berlin
 *               2017 Freie Universität Berlin
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
 * @brief       Test application for testing priority inheritance for nested
 *              mutexes
 *
 * @author      Thomas Geithner <thomas.geithner@dai-labor.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "mutex.h"
#include "xtimer.h"

#define TICK_LEN            (50 * US_PER_MS)
#define EXSPECTED_RESULT    (8)

#define T_NUMOF             (5U)

static mutex_t a = MUTEX_INIT;
static mutex_t b = MUTEX_INIT;

static char stacks[T_NUMOF][THREAD_STACKSIZE_MAIN];
static kernel_pid_t pids[T_NUMOF];
static const char *names[] = { "t1", "t2", "t3", "t4", "t4" };

static int result = 0;
static int res_addsub = 1;

static inline void delay(unsigned ticks)
{
    xtimer_usleep(ticks * TICK_LEN);
}

static inline void event(int num, const char *t_name, const char *msg)
{
    /* record event */
    result += (res_addsub * num);
    res_addsub *= -1;

    printf("Event %2i: %7s - %s\n", num, t_name, msg);
}

static void *t1(void *arg)
{
    (void)arg;

    delay(2);
    event(6, "t1", "locking mutex B");
    mutex_lock(&b);
    event(9, "t1", "holding mutex B");

    delay(1);
    event(10, "t1", "unlocking mutex B");
    mutex_unlock(&b);

    return NULL;
}

static void *t2(void *arg)
{
    (void)arg;

    delay(3);
    event(7, "t2", "starting extensive loop, potentially starving others");

    uint32_t time = xtimer_now_usec();
    while((xtimer_now_usec() - time) < (4 * TICK_LEN)) {
        thread_yield_higher();
    }

    event(11, "t2", "finished extensive loop");

    return NULL;
}

static void *t3(void *arg)
{
    (void)arg;

    delay(1);
    event(5, "t3", "locking mutex A");
    mutex_lock(&a);
    event(14, "t3", "holding mutex A");

    delay(1);
    event(15, "t3", "unlocking mutex A");
    mutex_unlock(&a);

    return NULL;
}

static void *t4(void *arg)
{
    (void)arg;

    delay(6);
    event(12, "t4", "starting infinite loop, potentially starving others");
    while (1) {
        thread_yield_higher();
    }

    return NULL;
}

static void *t5(void *arg)
{
    (void)arg;

    event(1, "t5", "locking mutex A");
    mutex_lock(&a);
    event(2, "t5", "holding mutex A");
    event(3, "t5", "locking mutex B");
    mutex_lock(&b);
    event(4, "t5", "holding mutex B");

    delay(4);
    event(8, "t5", "unlocking mutex B");
    mutex_unlock(&b);

    delay(3);
    event(13, "t5", "unlocking mutex A");
    mutex_unlock(&a);

    return NULL;
}

static thread_task_func_t handlers[] = { t1, t2, t3, t4, t5 };

int main(void)
{
    puts("Test for showing the effect of priority inversion when nesting mutexes\n");
    puts("If this tests succeeds, you should see 15 events appearing in order.\n"
         "The expected output should look like this:\n"
         "Event  1:      t5 - locking mutex A\n"
         "Event  2:      t5 - holding mutex A\n"
         "Event  3:      t5 - locking mutex B\n"
         "Event  4:      t5 - holding mutex B\n"
         "Event  5:      t3 - locking mutex A\n"
         "Event  6:      t1 - locking mutex B\n"
         "Event  7:      t2 - starting extensive loop, potentially starving others\n"
         "Event  8:      t5 - unlocking mutex B\n"
         "Event  9:      t1 - holding mutex B\n"
         "Event 10:      t1 - unlocking mutex B\n"
         "Event 11:      t2 - finished extensive loop\n"
         "Event 12:      t4 - starting infinite loop, potentially starving others\n"
         "Event 13:      t5 - unlocking mutex A\n"
         "Event 14:      t3 - holding mutex A\n"
         "Event 15:      t3 - unlocking mutex A\n");
    puts("TEST OUTPUT:");

    /* create threads */
    for (unsigned i = 0; i < T_NUMOF; i++) {
        pids[i] = thread_create(stacks[i], sizeof(stacks[i]),
                                (THREAD_PRIORITY_MAIN + 1 + i),
                                THREAD_CREATE_WOUT_YIELD,
                                handlers[i], NULL,
                                names[i]);
    }

    delay(13);

    if (result == EXSPECTED_RESULT) {
        puts("\n   *** result: SUCCESS ***");
    }
    else {
        puts("\n   *** result: FAILED ***");
    }

    return 0;
}
