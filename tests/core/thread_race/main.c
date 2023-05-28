/*
 * Copyright (C) 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
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
 * @brief       Test for race conditions in context switching
 *
 * May have false negatives.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "irq.h"
#include "sched.h"
#include "thread.h"

char iqr_check_stack[THREAD_STACKSIZE_DEFAULT];

static volatile uint8_t irq_occurred;

static void *_thread_irq_check(void *arg)
{
    (void)arg;

    while (1) {
        irq_occurred = 1;

        thread_sleep();
    }

    return NULL;
}

/* Mostly copied from thread_wakeup() */
static void _thread_wake_wo_yield(kernel_pid_t pid)
{
    unsigned old_state = irq_disable();

    thread_t *other_thread = thread_get(pid);

    sched_set_status(other_thread, STATUS_RUNNING);

    irq_restore(old_state);
}

static void _spin(void)
{
    /* Volatile so it is not messed with by optimizations */
    volatile uint8_t i;

    for (i = 0; i < 255; i++) ;
}

int main(void)
{
    puts("Context swap race condition test application");

    kernel_pid_t pid;

    puts("Starting IRQ check thread");
    pid = thread_create(iqr_check_stack, sizeof(iqr_check_stack),
                        THREAD_PRIORITY_MAIN - 1,
                        THREAD_CREATE_SLEEPING,
                        _thread_irq_check, NULL, "irqchk");

    printf("Checking for working context swap (to detect false positives)... ");
    irq_occurred = 0;
    _thread_wake_wo_yield(pid);

    thread_yield_higher();

    /* Delay so we are not testing for race conditions also */
    _spin();

    if (irq_occurred == 1) {
        puts("[Success]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Checking for reset of swaps (to detect false positives)... ");
    irq_occurred = 0;
    _thread_wake_wo_yield(pid);

    thread_yield_higher();

    /* Delay so we are not testing for race conditions also */
    _spin();

    if (irq_occurred == 1) {
        puts("[Success]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    /* Volatile so it is not messed with by optimizations */
    volatile uint8_t race_test;

    printf("Checking for context swap race condition... ");
    irq_occurred = 0;
    _thread_wake_wo_yield(pid);

    thread_yield_higher();

    /* Race instruction */
    race_test = irq_occurred;

    if (race_test == 1) {
        puts("[Success]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    return 0;
}
