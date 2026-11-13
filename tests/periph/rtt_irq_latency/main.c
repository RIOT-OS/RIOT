/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test how long the RTT driver blocks interrupts
 *
 * A periph_timer channel is set up to fire while an RTT API function is
 * running. The start of the RTT call is shifted across a window around the
 * timer deadline, so that the deadline eventually falls into every section
 * of the call that runs with interrupts disabled. The maximum delay of the
 * timer ISR (minus the delay measured with an empty call) is the longest
 * time interrupts were blocked by the RTT call.
 *
 * Long interrupt locks cause lost bytes on UARTs without RX FIFO, e.g.
 * one byte at 115200 baud takes only ~87 us.
 *
 * @}
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "embUnit.h"
#include "periph/rtt.h"
#include "periph/timer.h"

#ifndef TEST_TIMER_DEV
#  define TEST_TIMER_DEV        TIMER_DEV(0)
#endif

/**
 * @brief   Desired frequency of the timer used for measuring
 *
 * If `periph_timer_query_freqs` is available, the closest supported frequency
 * is used.
 */
#ifndef TEST_TIMER_FREQ
#  define TEST_TIMER_FREQ       (1000000LU)
#endif

/**
 * @brief   Minimum timer frequency to measure IRQ locks with sufficient
 *          resolution (one tick must be 10 us or less)
 */
#ifndef TEST_TIMER_FREQ_MIN
#  define TEST_TIMER_FREQ_MIN   (100000LU)
#endif

/**
 * @brief   Maximum accepted time in microseconds that an RTT API call may
 *          block interrupts
 */
#ifndef RTT_IRQ_LOCK_MAX_US
#  define RTT_IRQ_LOCK_MAX_US   (50U)
#endif

/* time between arming the timer and its deadline, in microseconds */
#define FIRE_OFFSET_US          (500U)
/* window before the deadline in which the RTT call is started, in microseconds */
#define SWEEP_US                (400U)
#define SWEEP_STEP_US           (2U)
#define REPEAT                  (3U)
/* upper bound for spinning on the timer ISR to avoid hanging forever */
#define SPIN_MAX                (10000000LU)

static uint32_t _freq;
static uint32_t _baseline;
static unsigned _fire_offset;
static volatile bool _fired;
static volatile unsigned _t_isr;

static void _timer_cb(void *arg, int chan)
{
    (void)arg;
    (void)chan;
    _t_isr = timer_read(TEST_TIMER_DEV);
    _fired = true;
}

static void _rtt_cb(void *arg)
{
    (void)arg;
}

static uint32_t _ticks_to_us(uint32_t ticks)
{
    return (uint32_t)(((uint64_t)ticks * 1000000LU) / _freq);
}

static unsigned _us_to_ticks(uint32_t us)
{
    return (unsigned)(((uint64_t)us * _freq) / 1000000LU);
}

static uint32_t _alarm_far_away(void)
{
    /* never reached, as every scenario sets a new alarm relative to now */
    return (rtt_get_counter() + (RTT_MAX_VALUE / 2)) & RTT_MAX_VALUE;
}

static void _empty(void)
{
}

static void _get_counter(void)
{
    (void)rtt_get_counter();
}

static void _set_alarm(void)
{
    rtt_set_alarm(_alarm_far_away(), _rtt_cb, NULL);
}

static void _set_alarm_twice(void)
{
    rtt_set_alarm(_alarm_far_away(), _rtt_cb, NULL);
    rtt_set_alarm(_alarm_far_away(), _rtt_cb, NULL);
}

static void _set_and_clear_alarm(void)
{
    rtt_set_alarm(_alarm_far_away(), _rtt_cb, NULL);
    rtt_clear_alarm();
}

#ifdef MODULE_PERIPH_RTT_OVERFLOW
static void _set_overflow_cb(void)
{
    rtt_set_overflow_cb(_rtt_cb, NULL);
}
#endif

#ifdef MODULE_PERIPH_RTT_SET_COUNTER
static void _set_counter(void)
{
    rtt_set_counter(rtt_get_counter());
}

static void _set_alarm_and_counter(void)
{
    uint32_t now = rtt_get_counter();

    /* write the counter while the alarm write may still be synchronized */
    rtt_set_alarm(_alarm_far_away(), _rtt_cb, NULL);
    rtt_set_counter(now);
}
#endif

/**
 * @brief   Take one sample: the timer deadline is @p pre ticks after the
 *          start of @p fn
 *
 * @return  ISR delay in ticks, or -1 if the sample is not usable
 */
static int32_t _sample(void (*fn)(void), unsigned pre)
{
    _fired = false;
    unsigned t0 = timer_read(TEST_TIMER_DEV);
    timer_set(TEST_TIMER_DEV, 0, _fire_offset);

    unsigned now;
    do {
        now = timer_read(TEST_TIMER_DEV);
        if (now < t0) {
            /* timer wrapped around, discard sample */
            pre = 0;
            break;
        }
    } while ((now - t0) < (_fire_offset - pre));

    if (!_fired && (now >= t0)) {
        fn();
    }

    for (uint32_t i = 0; !_fired; i++) {
        if (i == SPIN_MAX) {
            timer_clear(TEST_TIMER_DEV, 0);
            return -1;
        }
    }

    unsigned deadline = t0 + _fire_offset;
    if ((_t_isr < t0) || (deadline < t0) || (now < t0)) {
        /* timer wrapped around, discard sample */
        return -1;
    }
    if (_t_isr < deadline) {
        return 0;
    }
    return (int32_t)(_t_isr - deadline);
}

static uint32_t _max_latency_us(void (*fn)(void))
{
    uint32_t max = 0;
    unsigned sweep = _us_to_ticks(SWEEP_US);
    unsigned step = _us_to_ticks(SWEEP_STEP_US);

    if (step == 0) {
        step = 1;
    }

    for (unsigned rep = 0; rep < REPEAT; rep++) {
        for (unsigned pre = 0; pre <= sweep; pre += step) {
            int32_t lat = _sample(fn, pre);
            if ((lat > 0) && ((uint32_t)lat > max)) {
                max = lat;
            }
        }
    }

    return _ticks_to_us(max);
}

static void _check_irq_lock(const char *name, void (*fn)(void))
{
    uint32_t lat = _max_latency_us(fn);
    uint32_t lock = (lat > _baseline) ? (lat - _baseline) : 0;

    printf("\n%s: max ISR delay %" PRIu32 " us, IRQ lock ~%" PRIu32 " us\n",
           name, lat, lock);
    TEST_ASSERT_MESSAGE(lock <= RTT_IRQ_LOCK_MAX_US, "RTT blocks IRQs for too long");
}

static void test_rtt_get_counter(void)
{
    _check_irq_lock("rtt_get_counter()", _get_counter);
}

static void test_rtt_set_alarm(void)
{
    _check_irq_lock("rtt_set_alarm()", _set_alarm);
}

static void test_rtt_set_alarm_twice(void)
{
    _check_irq_lock("2x rtt_set_alarm()", _set_alarm_twice);
}

static void test_rtt_set_and_clear_alarm(void)
{
    _check_irq_lock("rtt_set_alarm() + rtt_clear_alarm()", _set_and_clear_alarm);
}

#ifdef MODULE_PERIPH_RTT_OVERFLOW
static void test_rtt_set_overflow_cb(void)
{
    _check_irq_lock("rtt_set_overflow_cb()", _set_overflow_cb);
}
#endif

#ifdef MODULE_PERIPH_RTT_SET_COUNTER
static void test_rtt_set_counter(void)
{
    _check_irq_lock("rtt_set_counter()", _set_counter);
}

static void test_rtt_set_alarm_and_counter(void)
{
    _check_irq_lock("rtt_set_alarm() + rtt_set_counter()", _set_alarm_and_counter);
}
#endif

static Test *tests_rtt_irq_latency(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_rtt_get_counter),
        new_TestFixture(test_rtt_set_alarm),
        new_TestFixture(test_rtt_set_alarm_twice),
        new_TestFixture(test_rtt_set_and_clear_alarm),
#ifdef MODULE_PERIPH_RTT_OVERFLOW
        new_TestFixture(test_rtt_set_overflow_cb),
#endif
#ifdef MODULE_PERIPH_RTT_SET_COUNTER
        new_TestFixture(test_rtt_set_counter),
        new_TestFixture(test_rtt_set_alarm_and_counter),
#endif
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
    puts("\nRTT interrupt latency test");

#ifdef MODULE_PERIPH_TIMER_QUERY_FREQS
    _freq = timer_get_closest_freq(TEST_TIMER_DEV, TEST_TIMER_FREQ);
#else
    _freq = TEST_TIMER_FREQ;
#endif
    if (_freq < TEST_TIMER_FREQ_MIN) {
        printf("Error: timer frequency %" PRIu32 " Hz is too low, at least %lu Hz "
               "are required\n", _freq, TEST_TIMER_FREQ_MIN);
        return 1;
    }
    if (timer_init(TEST_TIMER_DEV, _freq, _timer_cb, NULL) != 0) {
        printf("Error: timer_init() failed for %" PRIu32 " Hz, "
               "try setting TEST_TIMER_FREQ\n", _freq);
        return 1;
    }
    _fire_offset = _us_to_ticks(FIRE_OFFSET_US);

    rtt_init();

    printf("RTT_FREQUENCY: %" PRIu32 "\n", (uint32_t)RTT_FREQUENCY);
    printf("Timer frequency: %" PRIu32 " Hz\n", _freq);
    printf("Max. accepted IRQ lock: %u us\n", RTT_IRQ_LOCK_MAX_US);

    _baseline = _max_latency_us(_empty);
    printf("baseline: max ISR delay %" PRIu32 " us\n", _baseline);

    TESTS_START();
    TESTS_RUN(tests_rtt_irq_latency());
    TESTS_END();

    rtt_clear_alarm();
#ifdef MODULE_PERIPH_RTT_OVERFLOW
    rtt_clear_overflow_cb();
#endif
    timer_stop(TEST_TIMER_DEV);

    return 0;
}
