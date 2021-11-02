/*
 * Copyright (C) 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
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
 * @brief       Countdown timer test application
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include "benchmark.h"
#include "embUnit.h"
#include "periph/countdown.h"
#include "mutex.h"
#include "sema_inv.h"
#include "ztimer.h"

static void _unlock_mutex(void *ctx)
{
    mutex_unlock(ctx);
}

static void _post_sema(void *ctx)
{
    sema_inv_post(ctx);
}

static void _print(void *ctx)
{
    puts(ctx);
}

static void test_countdown_25ms(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    const unsigned timeout = 25;

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        TEST_ASSERT(countdown_init(i, timeout, COUNTDOWN_FLAG_UNIT_MS, _unlock_mutex, &lock));
        countdown_set(i, timeout, COUNTDOWN_FLAG_UNIT_MS);

        ztimer_now_t before = ztimer_now(ZTIMER_MSEC);
        countdown_start(i);
        TEST_ASSERT_EQUAL_INT(0, ztimer_mutex_lock_timeout(ZTIMER_MSEC, &lock, timeout + 5));
        TEST_ASSERT(timeout - 5 < ztimer_now(ZTIMER_MSEC) - before);
    }
}

static void test_countdown_25000ns(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    const unsigned timeout = 25;

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        TEST_ASSERT(countdown_init(i, NS_PER_US * timeout, COUNTDOWN_FLAG_UNIT_NS,
                   _unlock_mutex, &lock));
        countdown_set(i, timeout * NS_PER_US, COUNTDOWN_FLAG_UNIT_NS);

        ztimer_now_t before = ztimer_now(ZTIMER_USEC);
        countdown_start(i);
        TEST_ASSERT_EQUAL_INT(0, ztimer_mutex_lock_timeout(ZTIMER_USEC, &lock, timeout + 5));
        TEST_ASSERT(timeout - 5 < ztimer_now(ZTIMER_USEC) - before);
    }
}

static void test_countdown_long_1s(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    const unsigned timeout = 1000;

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        countdown_init(i, timeout / 4, COUNTDOWN_FLAG_UNIT_MS, _unlock_mutex, &lock);
        countdown_set_long(i, timeout, COUNTDOWN_FLAG_UNIT_MS);

        ztimer_now_t before = ztimer_now(ZTIMER_MSEC);
        countdown_start(i);
        TEST_ASSERT_EQUAL_INT(0, ztimer_mutex_lock_timeout(ZTIMER_MSEC, &lock, timeout + 5));
        TEST_ASSERT(timeout - 5 < ztimer_now(ZTIMER_MSEC) - before);
    }
}

static void test_countdown_long_cancel(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    const unsigned timeout = 1000;

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        countdown_init(i, timeout / 4, COUNTDOWN_FLAG_UNIT_MS, _unlock_mutex, &lock);
        countdown_set_long(i, timeout, COUNTDOWN_FLAG_UNIT_MS);

        countdown_start(i);
        ztimer_sleep(ZTIMER_MSEC, timeout / 2);
        countdown_cancel(i);

        TEST_ASSERT_EQUAL_INT(-ECANCELED, ztimer_mutex_lock_timeout(ZTIMER_MSEC, &lock, timeout + 5));
    }
}

static void test_countdown_long_set_cancel(void)
{
    mutex_t lock = MUTEX_INIT_LOCKED;
    const unsigned timeout = 1000;

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        countdown_init(i, timeout / 4, COUNTDOWN_FLAG_UNIT_MS, _unlock_mutex, &lock);
        countdown_set_long(i, timeout, COUNTDOWN_FLAG_UNIT_MS);

        countdown_start(i);
        ztimer_sleep(ZTIMER_MSEC, timeout / 2);

        /* calling set must cancel the current timeout */
        countdown_set_long(i, timeout, COUNTDOWN_FLAG_UNIT_MS);

        TEST_ASSERT_EQUAL_INT(-ECANCELED, ztimer_mutex_lock_timeout(ZTIMER_MSEC, &lock, timeout + 5));
    }
}

static void test_countdown_multi_25ms(void)
{
    sema_inv_t sema;
    const unsigned timeout = 25;

    sema_inv_init(&sema, COUNTDOWN_NUMOF);

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        TEST_ASSERT(countdown_init(i, timeout, COUNTDOWN_FLAG_UNIT_MS, _post_sema, &sema));
        countdown_set(i, timeout, COUNTDOWN_FLAG_UNIT_MS);
    }

    ztimer_now_t before = ztimer_now(ZTIMER_MSEC);

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        countdown_start(i);
    }

    TEST_ASSERT_EQUAL_INT(0, ztimer_mutex_lock_timeout(ZTIMER_MSEC, &sema.lock, timeout + 5));
    TEST_ASSERT(timeout - 5 < ztimer_now(ZTIMER_MSEC) - before);
}

static void test_countdown_multi_long_1s(void)
{
    sema_inv_t sema;
    const unsigned timeout = 1000;

    sema_inv_init(&sema, COUNTDOWN_NUMOF);

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        TEST_ASSERT(countdown_init(i, timeout / 4, COUNTDOWN_FLAG_UNIT_MS, _post_sema, &sema));
        countdown_set_long(i, timeout, COUNTDOWN_FLAG_UNIT_MS);
    }

    ztimer_now_t before = ztimer_now(ZTIMER_MSEC);

    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        countdown_start(i);
    }

    TEST_ASSERT_EQUAL_INT(0, ztimer_mutex_lock_timeout(ZTIMER_MSEC, &sema.lock, timeout + 5));
    TEST_ASSERT(timeout - 5 < ztimer_now(ZTIMER_MSEC) - before);
}

static Test *tests_periph_countdown(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_countdown_25ms),
        new_TestFixture(test_countdown_25000ns),
        new_TestFixture(test_countdown_long_1s),
        new_TestFixture(test_countdown_long_cancel),
        new_TestFixture(test_countdown_long_set_cancel),
        new_TestFixture(test_countdown_multi_25ms),
        new_TestFixture(test_countdown_multi_long_1s),
    };

    EMB_UNIT_TESTCALLER(periph_countdown_tests, NULL, NULL, fixtures);

    return (Test *)&periph_countdown_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_periph_countdown());
    TESTS_END();

    puts("\n--- Benchmark ---");
    for (unsigned i = 0; i < COUNTDOWN_NUMOF; ++i) {
        ztimer_now_t diff;

        printf("[timer %u]\n", i);
        countdown_init(i, 50, COUNTDOWN_FLAG_UNIT_MS, _print, "!!TOO SLOW!!");

        /* test set() */
        diff = ztimer_now(ZTIMER_USEC);
        for (unsigned j = 0; j < 1000; ++j) {
            countdown_set(i, 25, COUNTDOWN_FLAG_UNIT_MS);
        }
        diff = ztimer_now(ZTIMER_USEC) - diff;
        benchmark_print_time(diff, 1000, ".set()");

        /* test start() */
        diff = ztimer_now(ZTIMER_USEC);
        for (unsigned j = 0; j < 1000; ++j) {
            countdown_start(i);
        }
        diff = ztimer_now(ZTIMER_USEC) - diff;
        benchmark_print_time(diff, 1000, ".start()");

        /* test cancel() */
        diff = ztimer_now(ZTIMER_USEC);
        for (unsigned j = 0; j < 1000; ++j) {
            countdown_cancel(i);
        }
        diff = ztimer_now(ZTIMER_USEC) - diff;
        benchmark_print_time(diff, 1000, ".cancel()");
    }

    return 0;
}
