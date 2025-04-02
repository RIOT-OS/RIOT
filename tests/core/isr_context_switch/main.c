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
#include <stdio.h>

#include "macros/units.h"
#include "mutex.h"
#include "periph/timer.h"
#include "thread.h"
#include "clk.h"

#ifndef CHANNEL
#  define CHANNEL           0
#endif
#ifndef TEST_REPETITIONS
#  define TEST_REPETITIONS  500
#endif
#ifndef TEST_TIMEOUT_MIN
#  define TEST_TIMEOUT_MIN  10
#endif

static mutex_t sig_main = MUTEX_INIT_LOCKED;
static mutex_t sig_t1 = MUTEX_INIT_LOCKED;
static mutex_t sig_t2 = MUTEX_INIT_LOCKED;
static char t1_stack[THREAD_STACKSIZE_TINY];
static char t2_stack[THREAD_STACKSIZE_TINY];
static tim_t timer;
static uint32_t timeout_short;
static uint32_t timeout_long;

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
        timer_set(timer, CHANNEL, timeout_long);
    }
    else {
        timer_set(timer, CHANNEL, timeout_short);
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

static bool _try_setup_with_freq(uint32_t freq)
{
    if (timer_init(timer, freq, _cb, NULL) == 0) {
        timeout_short = 200ULL * freq / (uint64_t)coreclk();
        if (timeout_short < TEST_TIMEOUT_MIN) {
            timeout_short = TEST_TIMEOUT_MIN;
        }
        timeout_long = timeout_short * 100;

        printf("INFO: timer running at %" PRIu32 " Hz\n"
               "timeout short: %" PRIu32", timeout long: %" PRIu32 "\n",
               freq, timeout_short, timeout_long);
        return true;
    }

    return false;
}

static int _init_timer(void)
{
    if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
        for (unsigned timer_num = 0; timer_num < TIMER_NUMOF; timer_num++) {
            timer = TIMER_DEV(timer_num);
            uint32_t freq = timer_get_closest_freq(timer, MHZ(1));
            if (_try_setup_with_freq(freq)) {
                return 0;
            }
            printf("D'oh! timer_init(%u, %" PRIu32 ", ...) failed!\n"
                   "(Bug in used periph_timer_query_freqs implementation?)\n",
                   timer_num, freq);
        }
    }
    else {
        /* For MCUs not supporting `periph_timer_query_freqs`, we just iterate
        * over a few commonly supported freqs and hope for the best */
        static const uint32_t timer_freqs[] = { MHZ(1), KHZ(500), KHZ(250), 32768 };

        for (unsigned timer_num = 0; timer_num < TIMER_NUMOF; timer_num++) {
            timer = TIMER_DEV(timer_num);
            for (unsigned i = 0; i < ARRAY_SIZE(timer_freqs); i++) {
                if (timer_init(timer, timer_freqs[i], _cb, NULL) == 0) {
                    return 0;
                }
            }
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

    timer_set(timer, CHANNEL, timeout_short);
    mutex_lock(&sig_main);
    puts("TEST PASSED");

    return 0;
}
