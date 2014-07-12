/*
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief IRQ test application
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "hwtimer.h"
#include "thread.h"

char busy_stack[KERNEL_CONF_STACKSIZE_MAIN];
volatile int busy, i, k;

void *busy_thread(void *arg)
{
    (void) arg;

    int j = 0;
    puts("busy_thread starting");

    i = 0;

    while (busy) {
        k = j = ++i;
    }

    printf("i: %i\n", i);
    printf("j: %i\n", j);
    printf("k: %i\n", k);

    puts("success");

    return NULL;
}


int main(void)
{
    busy = 1;
    k = 23;
    thread_create(busy_stack, sizeof(busy_stack),
                  PRIORITY_MAIN + 1, CREATE_WOUT_YIELD,
                  busy_thread, NULL, "busy_thread");
    puts("busy_thread created");

    puts("hwtimer_wait()");
    hwtimer_wait(HWTIMER_TICKS(100000));
    busy = 0;

    puts("main: return");

    return 0;
}
