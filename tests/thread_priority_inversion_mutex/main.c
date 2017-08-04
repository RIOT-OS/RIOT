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
 * @brief       Test application for testing priority inheritance for mutexes
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

#define TICK_LEN            (50UL * US_PER_MS)
#define EXPECTED_RESULT     (4)

static mutex_t _res_mtx = MUTEX_INIT;

static char _stack_high[THREAD_STACKSIZE_MAIN];
static char _stack_mid[THREAD_STACKSIZE_MAIN];
static char _stack_low[THREAD_STACKSIZE_MAIN];

static kernel_pid_t _pid_low;
static kernel_pid_t _pid_mid;
static kernel_pid_t _pid_high;

static int _result = 0;
static int _res_addsub = 1;

static inline void delay(unsigned ticks)
{
    xtimer_usleep(ticks * TICK_LEN);
}

static inline void event(int num, const char *t_name, const char *msg)
{
    /* record event */
    _result += (_res_addsub * num);
    _res_addsub *= -1;

    printf("Event %2i: %7s - %s\n", num, t_name, msg);
}

static void *t_low_handler(void *arg)
{
    (void)arg;

    /* starting working loop immediately */
    event(1, "t_low", "locking mutex");
    mutex_lock(&_res_mtx);
    event(2, "t_low", "holding mutex");

    delay(3);

    event(5, "t_low", "unlocking mutex");
    mutex_unlock(&_res_mtx);

    return NULL;
}

static void *t_mid_handler(void *arg)
{
    (void)arg;

    delay(2);

    event(4, "t_mid", "starting infinite loop, potentially starving others");
    while (1) {
        thread_yield_higher();
    }

    return NULL;
}

static void *t_high_handler(void *arg)
{
    (void)arg;

    /* starting working loop after 500 ms */
    delay(1);

    event(3, "t_high", "locking mutex");
    mutex_lock(&_res_mtx);
    event(6, "t_high", "holding mutex");

    delay(1);

    event(7, "t_high", "unlocking mutex");
    mutex_unlock(&_res_mtx);

    return NULL;
}

int main(void)
{
    puts("Simple test for showing the effect of priority inversion\n");
    puts("If this tests succeeds, you should see 7 events appearing in order.\n"
         "The expected output should look like this:\n"
         "Event  1:   t_low - locking mutex\n"
         "Event  2:   t_low - holding mutex\n"
         "Event  3:  t_high - locking mutex\n"
         "Event  4:   t_mid - starting infinite loop, potentially starving others\n"
         "Event  5:   t_low - unlocking mutex\n"
         "Event  6:  t_high - holding mutex\n"
         "Event  7:  t_high - unlocking mutex\n");
    puts("TEST OUTPUT:");

    _pid_low  = thread_create(_stack_low, sizeof(_stack_low),
                              (THREAD_PRIORITY_MAIN + 3),
                              THREAD_CREATE_WOUT_YIELD,
                              t_low_handler, NULL,
                              "t_low");

    _pid_mid  = thread_create(_stack_mid, sizeof(_stack_mid),
                              (THREAD_PRIORITY_MAIN + 2),
                              THREAD_CREATE_WOUT_YIELD,
                              t_mid_handler, NULL,
                              "t_mid");

    _pid_high = thread_create(_stack_high, sizeof(_stack_high),
                              (THREAD_PRIORITY_MAIN + 1),
                              THREAD_CREATE_WOUT_YIELD,
                              t_high_handler, NULL,
                              "t_high");

    delay(5);

    if (_result == EXPECTED_RESULT) {
        puts("\n[SUCCESS]");
    }
    else {
        puts("\n[FAILED]");
    }

    return 0;
}
