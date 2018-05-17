/*
 * Copyright (C) 2017 Gilles DOFFE <gdoffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief       Example of systick handler
 *
 * This example uses hardware systick to perform preemption
 *
 * @author      Gilles DOFFE <gdoffe@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "thread.h"

/* 1 second */
#define THREAD_PERIOD_INTERVAL (1000U * US_PER_MS)

char test0_thread_stack[THREAD_STACKSIZE_DEFAULT];
char test1_thread_stack[THREAD_STACKSIZE_DEFAULT];
char test2_thread_stack[THREAD_STACKSIZE_DEFAULT];
char test3_thread_stack[THREAD_STACKSIZE_DEFAULT];

/*** Thread functions ***/

/* test_task launched every X seconds */
void* test_task(void* arg) {
    uint32_t seconds = (uint32_t)arg;
    thread_t *active_thread = (thread_t *)sched_active_thread;

    for (;;) {
        xtimer_ticks32_t loop_start_time = xtimer_now();
        printf("%s - %"PRIu32" seconds\n", active_thread->name, seconds);
        xtimer_periodic_wakeup(&loop_start_time, seconds * THREAD_PERIOD_INTERVAL);
    }

    return NULL;
}

/* test3_task launched and blocking forever (that's what he believes ;) */
void* test3_task(void* arg) {
    (void)arg;
    for (;;) {
        puts("test3 - Forever");
        for (uint64_t i = 0; i < (3*XTIMER_HZ_BASE); i++);
    }

    return NULL;
}

/*** Handlers ***/

/* Systick IRQ handler */
void isr_systick(void)  {
    thread_yield();
}

/*** Main ***/

int main(void)  {

/* Tested only on Cortex M */
#if defined(MODULE_CORTEXM_COMMON)
    /* Configure systick clock to 1 ms */
    uint32_t error = SysTick_Config((CLOCK_CORECLOCK / 1000));

    if (error != 0)
        puts("ERROR !");
    else
        puts("SUCCESS !");
#else
#error "Error: arch not supported/tested. If you want to support another architecture, please update this example."
#endif /* __CORTEX_M */

    /* Do not set a priority value lower than THREAD_PRIORITY_MAIN or other threads will never be created... */
    /* Note : Lower is the priority value, higher is the priority */
    /* Note : Setting same priority for two or more threads do garanty scheduling order */
    thread_create(test0_thread_stack, sizeof(test0_thread_stack),
                  THREAD_PRIORITY_MAIN + 1, 0,
                  test_task, (void *) 1, "test0");
    thread_create(test1_thread_stack, sizeof(test1_thread_stack),
                  THREAD_PRIORITY_MAIN + 2, 0,
                  test_task, (void *) 2, "test1");
    thread_create(test2_thread_stack, sizeof(test2_thread_stack),
                  THREAD_PRIORITY_MAIN + 3, 0,
                  test_task, (void *) 3, "test2");
    thread_create(test3_thread_stack, sizeof(test3_thread_stack),
                  THREAD_PRIORITY_MAIN + 4, 0,
                  test3_task, NULL, "test3");

    for (;;) {
        char c = getchar();
        printf("Received: %c\n", c);
        if (c == 'p')
            core_panic(PANIC_UNDEFINED, "Debug panic to display threads");
    }

    return 0;
}
