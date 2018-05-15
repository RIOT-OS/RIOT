/*
 * Copyright (C) 2018 Josua Arndt
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
 * @brief       xtimer_configuration test application
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachend.de>

 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "xtimer.h"
#include "timex.h"
#include "thread.h"

#include "periph/gpio.h"

#define RUNS (5U)
#define SLEEP_TIMES_NUMOF (sizeof(sleep_times) / sizeof(sleep_times[0]))

static const uint32_t sleep_times[] = {
    XTIMER_BACKOFF_OVERHEAD,/* spin and shoot */
    XTIMER_BACKOFF - 1,     /* spin adjust XTIMER_BACKOFF_OVERHEAD*/
    XTIMER_BACKOFF + 1,     /* no backoff test */
    XTIMER_PERIODIC_SPIN,   /* for sure no backoff */
    1000,
    2345,
    50000,
    100000,
    123456,
};

xtimer_ticks32_t start_kept[5 * SLEEP_TIMES_NUMOF];
xtimer_ticks32_t test_times[5 * SLEEP_TIMES_NUMOF];
volatile xtimer_ticks32_t end_times[SLEEP_TIMES_NUMOF];
volatile uint8_t thread_sleeps;

static char stack[THREAD_STACKSIZE_MAIN];
static void *_thread(void *arg)
{
    (void)arg;

    puts("thread(): waiting going to sleep!");

    for ( thread_sleeps = 0; thread_sleeps < SLEEP_TIMES_NUMOF; thread_sleeps++) {
        thread_sleep();
        end_times[thread_sleeps] = xtimer_now();
    }
    return NULL;
}

int main(void)
{
#if defined(SLEEP_PIN_1)
    gpio_t sleep_pin_1 = SLEEP_PIN_1;
    gpio_init(sleep_pin_1, GPIO_OUT);
#endif
#if defined(SLEEP_PIN_2)
    gpio_t sleep_pin_2 = SLEEP_PIN_2;
    gpio_init(sleep_pin_2, GPIO_OUT);
#endif
#if defined(SLEEP_PIN_3)
    gpio_t sleep_pin_3 = SLEEP_PIN_3;
    gpio_init(sleep_pin_3, GPIO_OUT);
#endif

    uint32_t start_test, testtime;

    printf("Running test %u times with %u distinct sleep times\n", RUNS,
           (unsigned)SLEEP_TIMES_NUMOF);
    puts("Please hit any key and then ENTER to continue");
    getchar();
    start_test = xtimer_now_usec();

    puts("Testing Periodic xtimer with absolute timestamp.");
    /* Testing Periodic with fixed time
     * Periodic wakeup has the fastest return time as the absolute value is set
     * as timestamp.
     */
    xtimer_ticks32_t periode_ticks, start_periode;
    uint32_t periode_us;

    /* Testing xtimer_periodic_wakeup usage absolute timestamp*/
    periode_ticks.ticks32 = 5 * XTIMER_PERIODIC_RELATIVE;
    periode_us = xtimer_usec_from_ticks(periode_ticks);

    start_periode = xtimer_now();
    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        start_kept[n] = start_periode;
        xtimer_periodic_wakeup(&start_periode, periode_us);
        test_times[n] = xtimer_now();
    }
    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        xtimer_ticks32_t diff = xtimer_diff(test_times[n], start_kept[n]);
        printf("%" PRIu32 " %" PRIu32 " slept for %6" PRIu32 " ticks ( %6" PRIu32
               " us) expected %6" PRIu32 " ticks, diff %3" PRIu32 " ticks\n",
               start_kept[n].ticks32, test_times[n].ticks32, diff.ticks32,
               xtimer_usec_from_ticks(diff), periode_ticks.ticks32,
               xtimer_diff(diff, periode_ticks).ticks32);
    }

    puts("\nTesting Periodic xtimer with relative timestamp.");
    /* Testing xtimer_periodic_wakeup usage relativ timestamp*/
    periode_ticks.ticks32 = XTIMER_PERIODIC_RELATIVE - 1;
    periode_us = xtimer_usec_from_ticks(periode_ticks);

    start_periode = xtimer_now();
    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        start_kept[n] = start_periode;
        xtimer_periodic_wakeup(&start_periode, periode_us);
        test_times[n] = xtimer_now();
    }
    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        xtimer_ticks32_t diff = xtimer_diff(test_times[n], start_kept[n]);
        printf("%" PRIu32 " %" PRIu32 " slept for %6" PRIu32 " ticks ( %6" PRIu32
               " us) expected %6" PRIu32 " ticks, diff %3" PRIu32 " ticks\n",
               start_kept[n].ticks32, test_times[n].ticks32, diff.ticks32,
               xtimer_usec_from_ticks(diff), periode_ticks.ticks32,
               xtimer_diff(diff, periode_ticks).ticks32);
    }

    puts("\nTesting Periodic xtimer spin.");
    /* Testing xtimer_periodic_wakeup usage spin*/
    periode_ticks.ticks32 = XTIMER_PERIODIC_SPIN - 1;
    periode_us = xtimer_usec_from_ticks(periode_ticks);

    start_periode = xtimer_now();
    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        start_kept[n] = start_periode;
        xtimer_periodic_wakeup(&start_periode, periode_us);
        test_times[n] = xtimer_now();
    }
    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        xtimer_ticks32_t diff = xtimer_diff(test_times[n], start_kept[n]);
        printf("%" PRIu32 " %" PRIu32 " slept for %6" PRIu32 " ticks ( %6" PRIu32
               " us) expected %6" PRIu32 " ticks, diff %3" PRIu32 " ticks\n",
               start_kept[n].ticks32, test_times[n].ticks32, diff.ticks32,
               xtimer_usec_from_ticks(diff), periode_ticks.ticks32,
               xtimer_diff(diff, periode_ticks).ticks32);
    }

    puts("\nTesting xtimer sleep 32 spin. Bigger diff values expected.");
    /* Testing ticks */
    /* will always have some ticks delay as absolute timestamp is calculated
     * after calling multiple functions.
     * _xtimer_tsleep32
     * _xtimer_tsleep
     * _xtimer_set64
     * _xtimer_set
     * */
    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        xtimer_ticks32_t start, end, diff, sleep;
        sleep.ticks32 = sleep_times[n];

        start = xtimer_now();
PORTF |= (1 << PINF7);
        xtimer_tsleep32(sleep);
PORTF &= ~(1 << PINF7);
        end = xtimer_now();

        diff = xtimer_diff(end, start);
        printf("%" PRIu32 " %" PRIu32 " slept for %6" PRIu32 " ticks ( %6" PRIu32
               " us) expected %6" PRIu32 " ticks, diff %3" PRIu32 " ticks\n",
               start.ticks32, end.ticks32, diff.ticks32, xtimer_usec_from_ticks(diff),
               sleep.ticks32, xtimer_diff(diff, sleep).ticks32);
    }

    puts("\nTesting XTIMER_ISR_BACKOFF.");
    /* Testing XTIMER_ISR_BACKOFF */
    /* The delay is not predictabel as multiple xtimer gets fired in one ISR.
     * When setting xtimer with xtimer_set_wakeup the delay is smaller
     * as less functions are called untill absolute timestamp is calculated
     * then for _xtimer_tsleep32.
     * _xtimer_set_wakeup
     * _xtimer_set
     */
    xtimer_t xtimer[SLEEP_TIMES_NUMOF];
    xtimer_ticks32_t start[SLEEP_TIMES_NUMOF];
    uint32_t interval[SLEEP_TIMES_NUMOF];

    kernel_pid_t pid_main = thread_getpid();

    kernel_pid_t pid_1 = thread_create(stack,
                                       sizeof(stack),
                                       THREAD_PRIORITY_MAIN - 1,
                                       THREAD_CREATE_STACKTEST,
                                       _thread,
                                       NULL,
                                       "second_thread");

    /* timer to colide with first timer from other thread */
    periode_ticks.ticks32 = 40000 + XTIMER_ISR_BACKOFF / 2;
    interval[SLEEP_TIMES_NUMOF - 2] = xtimer_usec_from_ticks(periode_ticks);

    /* timer to wake up main after all threads are fired */
    periode_ticks.ticks32 = 40000 + (XTIMER_BACKOFF * SLEEP_TIMES_NUMOF * 2);
    interval[SLEEP_TIMES_NUMOF - 1] = xtimer_usec_from_ticks(periode_ticks);

    for (unsigned n = 0; n < (SLEEP_TIMES_NUMOF - 2); n++) {
        periode_ticks.ticks32 = 40000 + (XTIMER_BACKOFF * n * 2);
        interval[n] = xtimer_usec_from_ticks(periode_ticks);
    }

    /* wait for a nice ticks number */
    xtimer_ticks32_t sleep = xtimer_now();
    xtimer_ticks32_t next;
    next.ticks32 = 1;

    while (sleep.ticks32) {
        sleep.ticks32 = sleep.ticks32 / 10;
        next.ticks32 *= 10;
    }

    sleep = xtimer_diff(next, xtimer_now());
    xtimer_tsleep32(sleep);

    start[SLEEP_TIMES_NUMOF - 2] = xtimer_now();
    xtimer_set_wakeup(&xtimer[SLEEP_TIMES_NUMOF - 2],
                      interval[SLEEP_TIMES_NUMOF - 2], pid_main);

    start[SLEEP_TIMES_NUMOF - 1] = xtimer_now();
    xtimer_set_wakeup(&xtimer[SLEEP_TIMES_NUMOF - 1],
                      interval[SLEEP_TIMES_NUMOF - 1], pid_main);

    for (unsigned n = 0; n < (SLEEP_TIMES_NUMOF - 2); n++) {
        start[n] = xtimer_now();
        xtimer_set_wakeup(&xtimer[n], interval[n], pid_1);
    }

    /* timer colided with first from other thread */
    thread_sleep();
    end_times[SLEEP_TIMES_NUMOF - 2] = xtimer_now();

    /* wait for all timers to fire */
    thread_sleep();
    end_times[SLEEP_TIMES_NUMOF - 1] = xtimer_now();

    if ( thread_sleeps != (SLEEP_TIMES_NUMOF - 2)) {
        printf("Error: expected %u thread wake ups but only %u happend in time.\n",
            (uint8_t)(SLEEP_TIMES_NUMOF-2), thread_sleeps);
        puts("Perhaps increasing XTIMER_BACKOFF helps.\n\n");
    }

    for (unsigned n = 0; n < SLEEP_TIMES_NUMOF; n++) {
        if ((n == 0) || (n == SLEEP_TIMES_NUMOF - 2)) {
            puts("Two timer fired in same ISR, different diff expected");
        }

        xtimer_ticks32_t diff = xtimer_diff(end_times[n], start[n]);
        xtimer_ticks32_t interval_ticks = xtimer_ticks_from_usec(interval[n]);

        printf("%" PRIu32 " %" PRIu32 " slept for %6" PRIu32 " ticks ( %6" PRIu32
               " us) expected %6" PRIu32 " ticks, diff %3" PRIu32 " ticks\n",
               start[n].ticks32, end_times[n].ticks32, diff.ticks32,
               xtimer_usec_from_ticks(diff), interval_ticks.ticks32,
               xtimer_diff(diff, interval_ticks).ticks32);
    }

    testtime = xtimer_now_usec() - start_test;
    printf("Test ran for %" PRIu32 " us\n", testtime);

    return 0;
}
