/*
 * Copyright (C) 2017 Technische Universität Berlin
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
 * @brief       Thread test application for priority inversion problem
 *
 * @author      Thomas Geithner <thomas.geithner@dai-labor.de>
 *
 * @}
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "thread.h"
#include "mutex.h"
#include "xtimer.h"

mutex_t res_mtx;

char stack_high[THREAD_STACKSIZE_DEFAULT];
char stack_mid[THREAD_STACKSIZE_DEFAULT];
char stack_low[THREAD_STACKSIZE_DEFAULT];

void *t_low_handler(void *arg)
{
    (void) arg;

    /* starting working loop immediately */
    while (1) {
        puts("t_low: allocating resource...");
        mutex_lock(&res_mtx);
        puts("t_low: got resource.");
        xtimer_sleep(1);

        puts("t_low: freeing resource...");
        mutex_unlock(&res_mtx);
        puts("t_low: freed resource.");
        xtimer_sleep(1);
    }
    return NULL;
}

void *t_mid_handler(void *arg)
{
    (void) arg;

    /* starting working loop after 3s */
    xtimer_sleep(3);

    puts("t_mid: doing some stupid stuff...");
    while (1) {
        thread_yield_higher();
    }
}

void *t_high_handler(void *arg)
{
    (void) arg;

    /* starting working loop after 500 ms */
    xtimer_msleep(500U);
    while (1) {
        puts("t_high: allocating resource...");
        mutex_lock(&res_mtx);
        puts("t_high: got resource.");
        xtimer_sleep(1);

        puts("t_high: freeing resource...");
        mutex_unlock(&res_mtx);
        puts("t_high: freed resource.");
        xtimer_sleep(1);
    }
    return NULL;
}

kernel_pid_t pid_low;
kernel_pid_t pid_mid;
kernel_pid_t pid_high;

int main(void)
{
    xtimer_init();
    mutex_init(&res_mtx);
    puts("This is a scheduling test for Priority Inversion");

    pid_low = thread_create(stack_low, sizeof(stack_low),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_STACKTEST,
        t_low_handler, NULL,
        "t_low");

    pid_mid = thread_create(stack_mid, sizeof(stack_mid),
        THREAD_PRIORITY_MAIN - 2,
        THREAD_CREATE_STACKTEST,
        t_mid_handler, NULL,
        "t_mid");

    pid_high = thread_create(stack_high, sizeof(stack_high),
        THREAD_PRIORITY_MAIN - 3,
        THREAD_CREATE_STACKTEST,
        t_high_handler, NULL,
        "t_high");

    thread_sleep();
    return 0;
}
