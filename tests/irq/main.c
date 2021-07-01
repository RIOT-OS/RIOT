/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *               2019 Freie Universität Berlin
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
 * @brief IRQ test application
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "irq.h"
#include "xtimer.h"
#include "thread.h"

static char busy_stack[THREAD_STACKSIZE_MAIN];
static volatile int busy, i, k;


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

    puts("SUCCESS");

    return NULL;
}


int main(void)
{
    puts("START");

    /* check if we can control interrupt enabling properly */

    /* initially, interrupts should be enabled */
    if (!irq_is_enabled()) {
        goto err;
    }

    /* test a 'brute-force' disable/enable sequence */
    irq_disable();
    if (irq_is_enabled()) {
        goto err;
    }
    irq_enable();
    if (!irq_is_enabled()) {
        goto err;
    }

    /* test a simple disable/restore sequence */
    unsigned state = irq_disable();
    if (irq_is_enabled()) {
        goto err;
    }
    unsigned state_inner = irq_disable();
    if (irq_is_enabled()) {
        goto err;
    }
    irq_restore(state_inner);
    if (irq_is_enabled()) {
        goto err;
    }
    irq_restore(state);
    if (!irq_is_enabled()) {
        goto err;
    }

    busy = 1;
    k = 23;
    thread_create(busy_stack, sizeof(busy_stack),
                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_WOUT_YIELD,
                  busy_thread, NULL, "busy_thread");
    puts("busy_thread created");

    puts("xtimer_wait()");
    xtimer_usleep(100000);
    busy = 0;

    puts("main: return");

    return 0;

err:
    puts("FAILED");
    return 0;
}
