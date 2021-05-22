/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief   Application for testing context switching triggered from IRQ
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#include "macros/units.h"
#include "mutex.h"
#include "periph/timer.h"
#include "random.h"
#include "thread.h"

#ifndef TIMER
#define TIMER               0
#endif
#ifndef CHANNEL
#define CHANNEL               0
#endif
#ifndef TEST_REPETITIONS
#define TEST_REPETITIONS    500
#endif
#ifndef TEST_TIMEOUT_SHORT
#define TEST_TIMEOUT_SHORT  8
#endif
#ifndef TEST_TIMEOUT_LONG
#define TEST_TIMEOUT_LONG   1000
#endif

static mutex_t sig_main = MUTEX_INIT_LOCKED;
static mutex_t sig_t1 = MUTEX_INIT_LOCKED;
static mutex_t sig_t2 = MUTEX_INIT_LOCKED;
static char t1_stack[THREAD_STACKSIZE_TINY];
static char t2_stack[THREAD_STACKSIZE_TINY];

static void _cb(void *unused1, int unused2)
{
    (void)unused1;
    (void)unused2;
    static uint8_t counter = 0;

    if (counter++ & 0x01) {
        mutex_unlock(&sig_t1);
    }
    else {
        mutex_unlock(&sig_t2);
    }

    if (counter <= UINT8_MAX / 2) {
        timer_set(TIMER_DEV(TIMER), CHANNEL, TEST_TIMEOUT_LONG);
    }
    else {
        timer_set(TIMER_DEV(TIMER), CHANNEL, TEST_TIMEOUT_SHORT);
    }
}

static void *t1_impl(void *unused)
{
    (void)unused;
    for (unsigned repetitions = 0; repetitions < TEST_REPETITIONS; repetitions++) {
        mutex_lock(&sig_t1);
    }
    return NULL;
}

static void *t2_impl(void *unused)
{
    (void)unused;
    for (unsigned repetitions = 0; repetitions < TEST_REPETITIONS; repetitions++) {
        mutex_lock(&sig_t2);
    }
    mutex_unlock(&sig_main);
    return NULL;
}

static int _init_timer(void)
{
    /* Rather than manually having to maintain a mapping of which boards support which frequency,
     * just iterate over a set of frequencies until one is supported.
     */
    static const uint32_t timer_freqs[] = { MHZ(1), KHZ(500), KHZ(250), 32768, CLOCK_CORECLOCK };

    for (unsigned i = 0; i < ARRAY_SIZE(timer_freqs); i++) {
        if (timer_init(TIMER_DEV(TIMER), timer_freqs[i], _cb, NULL) == 0) {
            printf("INFO: timer running at %" PRIu32 " Hz\n", timer_freqs[i]);
            return 0;
        }
    }

    return -1;
}

int main(void)
{
    printf("Testing %u context switches triggered from ISR\n", (unsigned)TEST_REPETITIONS);
    thread_create(t1_stack, sizeof(t1_stack),
                  THREAD_PRIORITY_MAIN + 1,
                  THREAD_CREATE_STACKTEST,
                  t1_impl, NULL, "t1");
    thread_create(t2_stack, sizeof(t2_stack),
                  THREAD_PRIORITY_MAIN + 1,
                  THREAD_CREATE_STACKTEST,
                  t2_impl, NULL, "t2");

    if (_init_timer()) {
        puts("Failed to initialize timer.\n==>TEST FAILED");
        return 1;
    }

    timer_set(TIMER_DEV(TIMER), CHANNEL, TEST_TIMEOUT_LONG);
    mutex_lock(&sig_main);
    puts("TEST PASSED");

    return 0;
}
