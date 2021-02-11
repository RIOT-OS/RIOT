/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Peripheral PTP clock test application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "fmt.h"
#include "kernel_defines.h"
#include "macros/units.h"
#include "mutex.h"
#include "periph/ptp.h"
#include "periph/timer.h"

#define TEST_TIME_US        (US_PER_SEC / 2)
#define TEST_ROUNDS         (50)
#define PERIOD_US           (TEST_TIME_US / TEST_ROUNDS)

#ifndef TIM
#define TIM                 TIMER_DEV(0)
#endif
#ifndef TIM_FREQ
#define TIM_FREQ            MHZ(1)
#endif

#define TIM_PERIOD          (1ULL * PERIOD_US * TIM_FREQ / US_PER_SEC - 1)
#define TIM_TIME            (1ULL * TEST_TIME_US * TIM_FREQ / US_PER_SEC)

static mutex_t sync_mutex = MUTEX_INIT_LOCKED;
static atomic_uint_least64_t timestamp;

static void speed_adj_cb(void *arg, int chan)
{
    (void)arg;
    (void)chan;
    mutex_unlock(&sync_mutex);
}

static int test_speed_adjustment(const int32_t speed)
{

    uint32_t expected_ns = TEST_TIME_US * 1000;
    expected_ns += ((int64_t)expected_ns * speed) >> 32;
    const uint32_t expected_ns_lower = expected_ns *  9999ULL / 10000ULL;
    const uint32_t expected_ns_upper = expected_ns * 10001ULL / 10000ULL;

    ptp_clock_adjust_speed(speed);
    print_str("Testing clock at speed ");
    print_s32_dec(speed);
    print_str(" (~");
    {
        int64_t tmp = speed * 100000ULL;
        tmp /= UINT32_MAX;
        tmp += 100000ULL;
        char output[16];
        print(output, fmt_s32_dfp(output, (int32_t)tmp, -3));
    }
    print_str(" % of nominal speed): ");

    if (timer_init(TIM, TIM_FREQ, speed_adj_cb, NULL)) {
        print_str("FAILED.\nCouldn't set up periph_timer for comparison\n");
        return 1;
    }

    /* be double sure mutex is indeed locked */
    mutex_trylock(&sync_mutex);

    timer_stop(TIM);

    if (timer_set(TIM, 0, TIM_TIME)) {
        print_str("FAILED.\nCouldn't set periph_timer for comparison\n");
        return 1;
    }

    uint64_t start_ns = ptp_clock_read_u64();
    timer_start(TIM);

    mutex_lock(&sync_mutex);
    uint64_t got_ns = ptp_clock_read_u64() - start_ns;
    timer_stop(TIM);
    uint64_t diff_ns = 0;

    int failed = 0;
    if (got_ns > expected_ns_upper) {
        failed = 1;
        diff_ns = got_ns - expected_ns;
    }
    else if (got_ns < expected_ns_lower) {
        failed = 1;
        diff_ns = expected_ns - got_ns;
    }

    if (failed) {
        int64_t tmp = (int64_t)got_ns - (int64_t)expected_ns;
        tmp = (tmp * 100000LL + expected_ns / 2) / expected_ns;
        tmp += 100000LL;
        char percentage[16];
        print_str("FAILED\n");
        print_str("expected: ");
        print_u64_dec(expected_ns);
        print_str(", got: ");
        print_u64_dec(got_ns);
        print_str(", diff: ");
        print_u64_dec(diff_ns);
        print_str(" (");
        print(percentage, fmt_s32_dfp(percentage, (int32_t)tmp, -3));
        print_str("% offset)\n");
    }
    else {
        print_str("SUCCEEDED\n");
    }

    return failed;
}

static void clock_adj_cb(void *arg, int chan)
{
    int32_t offset = (uintptr_t)arg;
    (void)chan;
    uint64_t now = ptp_clock_read_u64();
    ptp_clock_adjust(offset);
    atomic_store(&timestamp, now);
    mutex_unlock(&sync_mutex);
}


static int test_clock_adjustment(int32_t offset)
{
    /* Record one extra sample, to throw away the first measurement */
    static int64_t diffs[TEST_ROUNDS + 1];
    int64_t period_ns = PERIOD_US * 1000ULL + offset;
    uint64_t last_ns;

    print_str("Testing clock adjustments for offset ");
    print_s32_dec(offset);
    print_str(": ");

    /* be double sure mutex is indeed locked */
    mutex_trylock(&sync_mutex);

    if (timer_init(TIM, TIM_FREQ, clock_adj_cb, (void *)offset) ||
        timer_set_periodic(TIM, 0, TIM_PERIOD, TIM_FLAG_RESET_ON_MATCH)) {
        print_str("FAILED.\nCouldn't set up periph_timer for comparison\n");
        return 1;
    }

    /* wait for periodic timer IRQ */
    mutex_lock(&sync_mutex);
    last_ns = atomic_load(&timestamp);

    for (unsigned i = 0; i < TEST_ROUNDS + 1; i++) {
        /* wait for periodic timer IRQ */
        mutex_lock(&sync_mutex);
        uint64_t now_ns = atomic_load(&timestamp);
        diffs[i] = (int64_t)(now_ns - last_ns) - period_ns;
        last_ns = now_ns;
    }

    timer_stop(TIM);

    int64_t avg = 0, var = 0, min = INT64_MAX, max = INT64_MIN;
    for (unsigned i = 1; i < TEST_ROUNDS + 1; i++) {
        avg += diffs[i];
    }
    avg = (avg + TEST_ROUNDS / 2) / TEST_ROUNDS;
    for (unsigned i = 1; i < TEST_ROUNDS + 1; i++) {
        int64_t tmp = diffs[i] - avg;
        var += tmp * tmp;
        if (diffs[i] < min) {
            min = diffs[i];
        }
        if (diffs[i] > max) {
            max = diffs[i];
        }
    }

    var = (var + TEST_ROUNDS / 2) / TEST_ROUNDS;

    int failed = 0;
    if ((max < 500) && (min > -500) && (avg < 250) && (avg > -250)) {
        print_str("SUCCEEDED\n");
    }
    else {
        failed = 1;
        print_str("FAILED\nDiffs:\n");
        for (unsigned i = 1; i < TEST_ROUNDS + 1; i++) {
            print_s64_dec(diffs[i]);
            print_str("\n");
        }
    }
    print_str("Statistics: avg = ");
    print_s64_dec(avg);
    print_str(", min = ");
    print_s64_dec(min);
    print_str(", max = ");
    print_s64_dec(max);
    print_str(", σ² = ");
    print_s64_dec(var);
    print_str("\n");

    return failed;
}

int main(void)
{
    static const int32_t speeds[] = {
        /* 0%, +50%, -50%, +1%, -1%, +0.1%, -0.1%, +42/(2^32), -42/(2^32) */
        0, INT32_MAX, INT32_MIN, 42949673, -42949673, 4294967, -4294967, 42, -42
    };
    static const int32_t offsets[] = {
        0, -1337, +1337, INT32_MAX
    };
    int failed = 0;

    if (IS_USED(MODULE_PERIPH_PTP_SPEED_ADJUSTMENT)) {
        for (size_t i = 0; i < ARRAY_SIZE(speeds); i++) {
            failed |= test_speed_adjustment(speeds[i]);
        }

        /* Restore nominal clock speed */
        ptp_clock_adjust_speed(0);
    }

    for (size_t i = 0; i < ARRAY_SIZE(offsets); i++) {
        failed |= test_clock_adjustment(offsets[i]);
    }

    if (failed) {
        print_str("TEST FAILED!\n");
    }
    else {
        print_str("TEST SUCCEEDED!\n");
    }
    return 0;
}
