/*
 * Copyright (C) 2017 Technische Universität Berlin
 *               2021 Otto-von-Guericke-Universität Magdeburg
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
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <string.h>

#include "board.h"
#include "fmt.h"
#include "irq.h"
#include "mutex.h"
#include "periph_conf.h"
#include "thread.h"
#include "macros/units.h"

#ifndef FANCY
#define FANCY 0
#endif

/* Fallback in case core clock isn't define - should only be the case on native.
 * We don't need a correct value here any, it is only used for the fancy busy delay to make the
 * problem more approachable to human beings. */
#ifndef CLOCK_CORECLOCK
#define CLOCK_CORECLOCK GHZ(1)
#endif

static mutex_t mtx_res = MUTEX_INIT;
static mutex_t mtx_start_low = MUTEX_INIT_LOCKED;
static mutex_t mtx_start_mid = MUTEX_INIT_LOCKED;
static mutex_t mtx_start_high = MUTEX_INIT_LOCKED;

static char stack_high[THREAD_STACKSIZE_SMALL];
static char stack_mid[THREAD_STACKSIZE_SMALL];
static char stack_low[THREAD_STACKSIZE_SMALL];

static char run_order[16] = "";
static size_t run_order_pos = 0;

static void busy_delay(void)
{
    /* About 1 second if assuming one loop iteration takes ~20 CPU cycles. Actual duration doesn't
     * matter at all, but this delay has to be busy (not yielding during wait). */
    for (volatile uint32_t i = 0; i < CLOCK_CORECLOCK / 20; i++) { }
}

static void record_thread_started(const char *priority)
{
    print_str(priority);
    print_str(" priority thread (pid = ");
    print_u32_dec(thread_getpid());
    print_str(", prio = ");
    print_u32_dec(thread_get_active()->priority);
    print_str(") has started\n");
}

static void record_thread_working(const char *priority)
{
    print_str(priority);
    print_str(" priority thread started to work on its task\n");

    /* make recording of running thread atomic by disabling IRQs. We could have used a mutex here
     * as well, but for something so short just disabling IRQs is more sensible */
    unsigned irq_state = irq_disable();
    run_order[run_order_pos++] = priority[0];
    irq_restore(irq_state);
}

static void record_thread_done(const char *priority)
{
    print_str(priority);
    print_str(" priority thread is done\n");
}

static void *low_handler(void *arg)
{
    (void)arg;
    mutex_lock(&mtx_start_low);
    record_thread_started("low");

    mutex_lock(&mtx_res);
    record_thread_working("low");

    /* launch mid and high priority thread now */
    mutex_unlock(&mtx_start_high);
    mutex_unlock(&mtx_start_mid);

    record_thread_done("low");
    mutex_unlock(&mtx_res);

    return NULL;
}

static void *mid_handler(void *arg)
{
    (void)arg;
    mutex_lock(&mtx_start_mid);
    record_thread_started("mid");

    record_thread_working("mid");
    if (FANCY) {
        busy_delay();
        print_str("    ... this ...\n");
        busy_delay();
        print_str("    ... takes ...\n");
        busy_delay();
        print_str("    ... bloody ...\n");
        busy_delay();
        print_str("    ... ages ...\n");
        busy_delay();
        print_str("    ... to ...\n");
        busy_delay();
        print_str("    ... complete ...\n");
    }

    record_thread_done("mid");
    return NULL;
}

static void *high_handler(void *arg)
{
    (void)arg;
    mutex_lock(&mtx_start_high);
    record_thread_started("high");

    mutex_lock(&mtx_res);
    record_thread_working("high");
    record_thread_done("high");
    mutex_unlock(&mtx_res);

    return NULL;
}

int main(void)
{
    thread_create(stack_low, sizeof(stack_low),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  low_handler, NULL, "low");

    thread_create(stack_mid, sizeof(stack_mid),
                  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
                  mid_handler, NULL, "mid");

    thread_create(stack_high, sizeof(stack_high),
                  THREAD_PRIORITY_MAIN - 3, THREAD_CREATE_STACKTEST,
                  high_handler, NULL, "high");

    /* Start low priority thread first, which will start high and mid priority ones after the
     * shared resource is obtained */
    mutex_unlock(&mtx_start_low);

    if (strcmp("lhm", run_order) == 0) {
        print_str("TEST PASSED\n");
        return 0;
    }
    else if (strcmp("lmh", run_order) == 0) {
        print_str("==> Priority inversion occurred\n");
    }
    else {
        /* This should occur neither with nor without priority inversion! */
        print_str("BUG: \"");
        print_str(run_order);
        print_str("\"\n");
    }

    print_str("TEST FAILED\n");
    return 0;
}
