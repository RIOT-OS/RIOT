/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Another peripheral timer test application
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "bitarithm.h"
#include "fmt.h"
#include "mutex.h"
#include "random.h"
#include "div.h"
#include "matstat.h"
#include "thread.h"
#ifdef MODULE_PERIPH_RTT
#include "periph/rtt.h"
#endif
#if TEST_XTIMER
#include "xtimer.h"
#endif

#include "board.h"
#include "cpu.h"
#include "periph/timer.h"
#include "test_utils/expect.h"

#include "print_results.h"
#include "spin_random.h"
#include "bench_timers_config.h"

#ifndef TEST_TRACE
#define TEST_TRACE 0
#endif

/*
 * All different variations will be mixed to provide the most varied input
 * vector possible for the benchmark. A more varied input should yield a more
 * correct estimate of the mean error and variance. Random CPU processing delays
 * will be inserted between each step to avoid phase locking the benchmark to
 * unobservable timer internals.
 */
#if TEST_XTIMER
enum test_variants {
    TEST_XTIMER_SET             = 0,
    TEST_PARALLEL               = 1,
    TEST_XTIMER_PERIODIC_WAKEUP = 2,
    TEST_XTIMER_SPIN            = 4,
    TEST_VARIANT_NUMOF          = 6,
};
#else /* TEST_XTIMER */
/*
 * Results will be grouped by function, rescheduling yes/no, start/stop.
 * functions: timer_set, timer_set_absolute
 * reschedule: yes/no, when yes: first set one target time, before that time has
 *             passed, set the real target time
 * start/stop: if stop: call timer_stop before setting the target time, then call timer_start
 */
enum test_variants {
    TEST_RESCHEDULE         = 1,
    TEST_STOPPED            = 2,
    TEST_ABSOLUTE           = 4,
    TEST_VARIANT_NUMOF      = 8,
};
#endif /* else TEST_XTIMER */

/* Benchmark processing overhead, results will be compensated for this to make
 * the results easier to understand */
static int32_t overhead_target;
static int32_t overhead_read;

/* Seed for initializing the random module */
static uint32_t seed = 123;

/* Mutex used for signalling between main thread and ISR callback */
static mutex_t mtx_cb = MUTEX_INIT_LOCKED;

/* Test state element */
typedef struct {
    matstat_state_t *ref_state; /* timer_set error statistics state */
    matstat_state_t *int_state; /* timer_read error statistics state */
    unsigned int target_ref; /* Target time in reference timer */
    unsigned int target_tut; /* Target time in timer under test */
} test_ctx_t;

static test_ctx_t test_context;

#if DETAILED_STATS
#if LOG2_STATS
/* Group test values by 2-logarithm to reduce memory requirements */
static matstat_state_t ref_states[TEST_VARIANT_NUMOF * TEST_LOG2NUM];
#else
/* State vector, first half will contain state for timer_set tests, second half
 * will contain state for timer_set_absolute */
static matstat_state_t ref_states[TEST_VARIANT_NUMOF * TEST_NUM];
#endif
#else
/* Only keep stats per function variation */
static matstat_state_t ref_states[TEST_VARIANT_NUMOF];
#endif

/* timer_read error statistics states */
static matstat_state_t int_states[TEST_VARIANT_NUMOF];

/* Limits for the mean and variance, to compare the results against expectation */
static stat_limits_t ref_limits;
static stat_limits_t int_limits;

#if TEST_XTIMER
static const result_presentation_t presentation = {
    .groups = (const result_group_t[1]) {
        {
            .label = "xtimer",
            .sub_labels = (const char *[]){
                [TEST_XTIMER_SET] = "_xt_set",
                [TEST_XTIMER_SET | TEST_PARALLEL] = "_xt_set race",
                [TEST_XTIMER_PERIODIC_WAKEUP] = "_xt_periodic",
                [TEST_XTIMER_PERIODIC_WAKEUP | TEST_PARALLEL] = "_xt_periodic race",
                [TEST_XTIMER_SPIN] = "_xt_spin",
                [TEST_XTIMER_SPIN | TEST_PARALLEL] = "_xt_spin race",
            },
            .num_sub_labels = TEST_VARIANT_NUMOF,
        },
    },
    .num_groups = 1,
    .ref_limits = &ref_limits,
    .int_limits = &int_limits,
    .offsets = (const unsigned[]) {
        [TEST_XTIMER_SET]                               = TEST_MIN_REL,
        [TEST_XTIMER_SET | TEST_PARALLEL]               = TEST_MIN_REL,
        [TEST_XTIMER_PERIODIC_WAKEUP]                   = TEST_MIN_REL,
        [TEST_XTIMER_PERIODIC_WAKEUP | TEST_PARALLEL]   = TEST_MIN_REL,
        [TEST_XTIMER_SPIN]                              = TEST_MIN_REL,
        [TEST_XTIMER_SPIN | TEST_PARALLEL]              = TEST_MIN_REL,
    },
};
#else /* TEST_XTIMER */
/* Number of variant groups, used when printing results */
#define TEST_VARIANT_GROUPS 2

static const result_presentation_t presentation = {
    .groups = (const result_group_t[TEST_VARIANT_GROUPS]) {
        {
            .label = "timer_set",
            .sub_labels = (const char *[]){
                [0] = "running",
                [TEST_RESCHEDULE] = "resched",
                [TEST_STOPPED] = "stopped",
                [TEST_STOPPED | TEST_RESCHEDULE] = "stopped, resched",
            },
            .num_sub_labels = (TEST_VARIANT_NUMOF / TEST_VARIANT_GROUPS),
        },
        {
            .label = "timer_set_absolute",
            .sub_labels = (const char *[]){
                [0] = "running",
                [TEST_RESCHEDULE] = "resched",
                [TEST_STOPPED] = "stopped",
                [TEST_STOPPED | TEST_RESCHEDULE] = "stopped, resched",
            },
            .num_sub_labels = (TEST_VARIANT_NUMOF / TEST_VARIANT_GROUPS),
        },
    },
    .num_groups = TEST_VARIANT_GROUPS,
    .ref_limits = &ref_limits,
    .int_limits = &int_limits,
    .offsets = (const unsigned[]) {
        [0] =                                               TEST_MIN_REL,
        [TEST_RESCHEDULE] =                                 TEST_MIN_REL,
        [TEST_STOPPED] =                                    TEST_MIN_REL,
        [TEST_STOPPED | TEST_RESCHEDULE] =                  TEST_MIN_REL,
        [TEST_ABSOLUTE] =                                   TEST_MIN,
        [TEST_ABSOLUTE | TEST_RESCHEDULE] =                 TEST_MIN,
        [TEST_ABSOLUTE | TEST_STOPPED] =                    TEST_MIN,
        [TEST_ABSOLUTE | TEST_STOPPED | TEST_RESCHEDULE] =  TEST_MIN,
    },
};
#endif /* else TEST_XTIMER */

#ifdef MODULE_PERIPH_RTT
static uint32_t rtt_begin;
#endif

static unsigned int ref_begin;
static unsigned int tut_begin;

/**
 * @brief   Calculate the limits for mean and variance for this test
 */
static void set_limits(void)
{
    ref_limits.mean_low = -(TEST_UNEXPECTED_MEAN);
    ref_limits.mean_high = (TEST_UNEXPECTED_MEAN);
    ref_limits.variance_low = 0;
    ref_limits.variance_high = (TEST_UNEXPECTED_STDDEV) * (TEST_UNEXPECTED_STDDEV);

    int_limits.mean_low = -(TEST_UNEXPECTED_MEAN);
    int_limits.mean_high = TEST_UNEXPECTED_MEAN;
    int_limits.variance_low = 0;
    int_limits.variance_high = (TEST_UNEXPECTED_STDDEV) * (TEST_UNEXPECTED_STDDEV);

    /* The quantization errors should be uniformly distributed within +/- 0.5
     * test timer ticks of the reference time */
    /* The formula for the variance of a rectangle distribution on [a, b] is
     * Var = (b - a)^2 / 12 (taken directly from a statistics textbook)
     * Using (b - a)^2 / 12 == (10b - 10a) * ((10b + 1) - (10a + 1)) / 1200
     * gives a smaller truncation error when using integer operations for
     * converting the ticks */
    uint32_t conversion_variance = ((TIM_TEST_TO_REF(10) - TIM_TEST_TO_REF(0)) *
        (TIM_TEST_TO_REF(11) - TIM_TEST_TO_REF(1))) / 1200;
    if (TIM_REF_FREQ > TIM_TEST_FREQ) {
        ref_limits.variance_low = ((TIM_TEST_TO_REF(10) - TIM_TEST_TO_REF(0) - 10 * (TEST_UNEXPECTED_STDDEV)) *
            (TIM_TEST_TO_REF(11) - TIM_TEST_TO_REF(1) - 10 * (TEST_UNEXPECTED_STDDEV))) / 1200;
        ref_limits.variance_high = ((TIM_TEST_TO_REF(10) - TIM_TEST_TO_REF(0) + 10 * (TEST_UNEXPECTED_STDDEV)) *
            (TIM_TEST_TO_REF(11) - TIM_TEST_TO_REF(1) + 10 * (TEST_UNEXPECTED_STDDEV))) / 1200;
        /* The limits of the mean should account for the conversion error as well */
        /* rounded towards positive infinity */
        int32_t mean_error = (TIM_TEST_TO_REF(128) - TIM_TEST_TO_REF(0) + 127) / 128;
        ref_limits.mean_high += mean_error;
    }

    print_str("Expected error variance due to truncation in tick conversion: ");
    print_u32_dec(conversion_variance);
    print("\n", 1);
}

/* Callback for the timeout */
static void cb(void *arg)
{
    unsigned int now_tut = READ_TUT();
    unsigned int now_ref = timer_read(TIM_REF_DEV);
    if (arg == NULL) {
        print_str("cb: Warning! arg = NULL\n");
        return;
    }
    test_ctx_t *ctx = arg;
    if (ctx->ref_state == NULL) {
        print_str("cb: Warning! ref_state = NULL\n");
        return;
    }
    if (ctx->int_state == NULL) {
        print_str("cb: Warning! int_state = NULL\n");
        return;
    }
    /* Update running stats */
    /* When setting a timer with a timeout of X ticks, we expect the
     * duration between the set and the callback, dT, to be at least
     * X * time_per_tick.
     * In order to ensure that dT <= X * time_per_tick, the timer read value
     * will actually have incremented (X + 1) times during that period,
     * because the set can occur asynchrously anywhere between timer
     * increments. Therefore, in this test, we consider (X + 1) to be the
     * expected timer_read value at the point the callback is called.
     */

    /* Check that reference timer did not overflow during the test */
    if ((now_ref + 0x4000u) >= ctx->target_ref) {
        int32_t diff = now_ref - ctx->target_ref - 1 - overhead_target;
        matstat_add(ctx->ref_state, diff);
    }
    /* Update timer_read statistics only when timer_read has not overflowed
     * since the timer was set */
    if ((now_tut + 0x4000u) >= ctx->target_tut) {
        int32_t diff = now_tut - ctx->target_tut - 1 - overhead_read;
        matstat_add(ctx->int_state, diff);
    }

    mutex_unlock(&mtx_cb);
}

/* Wrapper for periph_timer callbacks */
static void cb_timer_periph(void *arg, int chan)
{
    (void)chan;
    cb(arg);
}

/**
 * @brief   Select the proper state for the given test number depending on the
 *          compile time configuration
 *
 * Depends on DETAILED_STATS, LOG2_STATS
 */
static void assign_state_ptr(test_ctx_t *ctx, unsigned int variant, uint32_t interval)
{
    ctx->int_state = &int_states[variant];
    if (DETAILED_STATS) {
        if (LOG2_STATS) {
            unsigned int log2num = bitarithm_msb(interval);

            ctx->ref_state = &ref_states[variant * TEST_LOG2NUM + log2num];
        }
        else {
            ctx->ref_state = &ref_states[variant * TEST_NUM + interval];
        }
    }
    else {
        ctx->ref_state = &ref_states[variant];
    }
}

static uint32_t derive_interval(uint32_t num)
{
    uint32_t interval;
    if ((DETAILED_STATS) && (LOG2_STATS)) {
        /* Use a logarithmic method to generate geometric variates in order to
         * populate the result table evenly across all buckets */

        /* Static exponent mask, picking the mask as tightly as possible reduces the
         * probability of discarded values, which reduces the computing overhead
         * between test iterations */

        static uint32_t exp_mask = 0;
        if (exp_mask == 0) {
            /* non-constant initializer */
            exp_mask = (2 << bitarithm_msb(TEST_LOG2NUM)) - 1;
            print_str("exp_mask = ");
            print_u32_hex(exp_mask);
            print("\n", 1);
            print_str("max interval = ");
            print_u32_dec((2 << exp_mask) - 1);
            print("\n", 1);
        }

        /* Pick an exponent based on the top bits of the number */
        /* exponent will be a number in the interval [0, log2(TEST_NUM) + 1] */
        unsigned int exponent = ((num >> (32 - 8)) & exp_mask);
        if (exponent == 0) {
            /* Special handling to avoid the situation where we never see a zero */
            /* We could also have used an extra right shift in the else case,
             * but the state grouping also groups 0 and 1 in the same bucket, which means that they are twice as likely  */
            interval = bitarithm_bits_set(num) & 1;
        }
        else {
            interval = (1 << exponent);
            interval |= (num & (interval - 1));
        }
    }
    else {
        static const uint32_t mask = (1 << TEST_LOG2NUM) - 1;
        interval = num & mask;
    }
    return interval;
}

#if TEST_XTIMER
static void nop(void *arg)
{
    (void)arg;
}

static void run_test(test_ctx_t *ctx, uint32_t interval, unsigned int variant)
{
    interval += TEST_MIN;
    unsigned int interval_ref = TIM_TEST_TO_REF(interval);
    xtimer_t xt = {
        .callback = cb,
        .arg = ctx,
    };
    xtimer_t xt_parallel = {
        .callback = nop,
        .arg = NULL,
    };
    if (TEST_TRACE) {
        switch (variant & ~TEST_PARALLEL) {
            case TEST_XTIMER_SET:
                print_str("rel ");
                break;
            case TEST_XTIMER_PERIODIC_WAKEUP:
                print_str("per ");
                break;
            case TEST_XTIMER_SPIN:
                print_str("spn ");
                break;
            default:
                break;
        }
        if (variant & TEST_PARALLEL) {
            print_str("= ");
        }
        else {
            print_str("- ");
        }
        print_u32_dec(interval);
        print("\n", 1);
    }

    spin_random_delay();
    if (variant & TEST_PARALLEL) {
        _xtimer_set64(&xt_parallel, interval, 0);
        //~ interval += XTIMER_BACKOFF;
        spin_random_delay();
    }
    ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
    uint32_t now = READ_TUT();
    ctx->target_tut = now + interval;
    switch (variant & ~TEST_PARALLEL) {
        case TEST_XTIMER_SET:
            _xtimer_set64(&xt, interval, 0);
            break;
        case TEST_XTIMER_PERIODIC_WAKEUP:
            _xtimer_periodic_wakeup(&now, interval);
            /* xtimer_periodic_wakeup sleeps the thread, no automatic callback */
            cb(xt.arg);
            break;
        case TEST_XTIMER_SPIN:
            _xtimer_spin(interval);
            /* xtimer_spin sleeps the thread, no automatic callback */
            cb(xt.arg);
            break;
        default:
            break;
    }
    mutex_lock(&mtx_cb);
    xtimer_remove(&xt_parallel);
    xtimer_remove(&xt);
}
#else /* TEST_XTIMER */
static void run_test(test_ctx_t *ctx, uint32_t interval, unsigned int variant)
{
    if (variant & TEST_ABSOLUTE) {
        interval += TEST_MIN;
    }
    else {
        interval += TEST_MIN_REL;
    }
    unsigned int interval_ref = TIM_TEST_TO_REF(interval);

    if (TEST_TRACE) {
        if (variant & TEST_ABSOLUTE) {
            print_str("A");
        }
        else {
            print_str("_");
        }
        if (variant & TEST_RESCHEDULE) {
            print_str("R");
        }
        else {
            print_str("_");
        }
        if (variant & TEST_STOPPED) {
            print_str("S ");
        }
        else {
            print_str("_ ");
        }
        print_u32_dec(interval);
        print_str("   ");
        print_u64_hex(READ_TUT());
        print("\n", 1);
    }

    spin_random_delay();
    if (variant & TEST_RESCHEDULE) {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval + RESCHEDULE_MARGIN);
        spin_random_delay();
    }
    if (variant & TEST_STOPPED) {
        timer_stop(TIM_TEST_DEV);
        spin_random_delay();
    }
    ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
    ctx->target_tut = READ_TUT() + interval;
    if (variant & TEST_ABSOLUTE) {
        timer_set_absolute(TIM_TEST_DEV, TIM_TEST_CHAN, ctx->target_tut);
    }
    else {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval);
    }
    if (variant & TEST_STOPPED) {
        spin_random_delay();
        /* do not update ctx->target_tut, because TUT should have been stopped
         * and not incremented during spin_random_delay */
        ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
        timer_start(TIM_TEST_DEV);
    }
    mutex_lock(&mtx_cb);
}
#endif /* TEST_XTIMER */

static int test_timer(void)
{
    uint32_t time_last = timer_read(TIM_REF_DEV);
    uint32_t time_elapsed = 0;
    do {
        uint32_t num = random_uint32();

        unsigned int variant = (num >> (32 - 3));
        if (variant >= TEST_VARIANT_NUMOF) {
            continue;
        }
        uint32_t interval = derive_interval(num);
        if (interval >= TEST_NUM) {
            /* Discard values outside our test range */
            continue;
        }
        assign_state_ptr(&test_context, variant, interval);
        run_test(&test_context, interval, variant);
        uint32_t now = timer_read(TIM_REF_DEV);
        if (now >= time_last) {
            /* Account for reference timer possibly overflowing before 30 seconds have passed */
            time_elapsed += now - time_last;
        }
        time_last = now;
        //~ print_str("e: ");
        //~ print_u32_dec(time_elapsed);
        //~ print_str("\n");
    } while(time_elapsed < TEST_PRINT_INTERVAL_TICKS);

    uint32_t ref_now = timer_read(TIM_REF_DEV);
    uint32_t tut_now = READ_TUT();
#ifdef MODULE_PERIPH_RTT
    uint32_t rtt_now = rtt_get_counter();
#endif
    print_str("Elapsed time:\n");
    print_str("        Reference: ");
    print_u32_dec((ref_now - ref_begin) / TIM_REF_FREQ);
    print_str("\n");
    print_str(" Timer under test: ");
    print_u32_dec((tut_now - tut_begin) / TIM_TEST_FREQ);
    print_str("\n");
#ifdef MODULE_PERIPH_RTT
    print_str(" Wall clock (RTT): ");
    print_u32_dec((rtt_now - rtt_begin) / RTT_FREQUENCY);
    print_str("\n");
#endif
    print_results(&presentation, &ref_states[0], &int_states[0]);

    return 0;
}

static void estimate_cpu_overhead(void)
{
    /* Try to estimate the amount of CPU overhead between test start to test
     * finish to get a better reading */
    print_str("Estimating benchmark overhead...\n");
    uint32_t interval = 0;
    overhead_target = 0;
    overhead_read = 0;
    test_ctx_t context;
    test_ctx_t *ctx = &context;
    matstat_state_t ref_state = MATSTAT_STATE_INIT;
    matstat_state_t int_state = MATSTAT_STATE_INIT;
    ctx->ref_state = &ref_state;
    ctx->int_state = &int_state;
    for (unsigned int k = 0; k < ESTIMATE_CPU_ITERATIONS; ++k) {
        unsigned int interval_ref = TIM_TEST_TO_REF(interval);
        spin_random_delay();
        ctx->target_tut = READ_TUT() + interval - 1;
        ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref - 1;
        /* call yield to simulate a context switch to isr and back */
        thread_yield_higher();
        cb_timer_periph(ctx, TIM_TEST_CHAN);
        mutex_lock(&mtx_cb);
    }
    overhead_target = matstat_mean(&ref_state);
    overhead_read = matstat_mean(&int_state);
    print_str("overhead_target = ");
    print_s32_dec(overhead_target);
    print_str(" (s2 = ");
    uint32_t var = matstat_variance(&ref_state);
    print_u32_dec(var);
    print_str(")\n");
    if (var > 2) {
        print_str("Warning: Variance in CPU estimation is too high\n");
#ifdef CPU_NATIVE
        print_str("This is expected on native when other processes are running\n");
#endif
    }
    print_str("overhead_read = ");
    print_s32_dec(overhead_read);
    print_str(" (s2 = ");
    var = matstat_variance(&int_state);
    print_u32_dec(var);
    print_str(")\n");
    if (var > 2) {
        print_str("Warning: Variance in CPU estimation is too high\n");
#ifdef CPU_NATIVE
        print_str("This is expected on native when other processes are running\n");
#endif
    }
}

int main(void)
{
    print_str("\nStatistical benchmark for timers\n");
    for (unsigned int k = 0; k < ARRAY_SIZE(ref_states); ++k) {
        matstat_clear(&ref_states[k]);
    }
    for (unsigned int k = 0; k < ARRAY_SIZE(int_states); ++k) {
        matstat_clear(&int_states[k]);
    }
    /* print test overview */
    print_str("Running timer test with seed ");
    print_u32_dec(seed);
    print_str(" using ");
#if MODULE_PRNG_MERSENNE
    print_str("Mersenne Twister PRNG.\n");
#elif MODULE_PRNG_MINSTD
    print_str("Park & Miller Minimal Standard PRNG.\n");
#elif MODULE_PRNG_MUSL_LCG
    print_str("Musl C PRNG.\n");
#elif MODULE_PRNG_TINYMT32
    print_str("Tiny Mersenne Twister PRNG.\n");
#elif MODULE_PRNG_XORSHIFT
    print_str("XOR Shift PRNG.\n");
#else
    print_str("unknown PRNG.\n");
#endif

    print_str("TEST_MIN = ");
    print_u32_dec(TEST_MIN);
    print("\n", 1);
    print_str("TEST_MAX = ");
    print_u32_dec(TEST_MAX);
    print("\n", 1);
    print_str("TEST_MIN_REL = ");
    print_u32_dec(TEST_MIN_REL);
    print("\n", 1);
    print_str("TEST_MAX_REL = ");
    print_u32_dec(TEST_MIN_REL + TEST_NUM - 1);
    print("\n", 1);
    print_str("TEST_NUM = ");
    print_u32_dec(TEST_NUM);
    print("\n", 1);
    print_str("log2(TEST_NUM - 1) = ");
    unsigned log2test = bitarithm_msb(TEST_NUM - 1);
    print_u32_dec(log2test);
    print("\n", 1);
    print_str("state vector elements per variant = ");
    print_u32_dec(ARRAY_SIZE(ref_states) / TEST_VARIANT_NUMOF);
    print("\n", 1);
    print_str("number of variants = ");
    print_u32_dec(TEST_VARIANT_NUMOF);
    print("\n", 1);
    print_str("sizeof(state) = ");
    print_u32_dec(sizeof(ref_states[0]));
    print_str(" bytes\n");
    print_str("state vector total memory usage = ");
    print_u32_dec(sizeof(ref_states));
    print_str(" bytes\n");
    expect(log2test < TEST_LOG2NUM);
    print_str("TIM_TEST_DEV = ");
    print_u32_dec(TIM_TEST_DEV);
    print_str(", TIM_TEST_FREQ = ");
    print_u32_dec(TIM_TEST_FREQ);
    print_str(", TIM_TEST_CHAN = ");
    print_u32_dec(TIM_TEST_CHAN);
    print("\n", 1);
    print_str("TIM_REF_DEV  = ");
    print_u32_dec(TIM_REF_DEV);
    print_str(", TIM_REF_FREQ  = ");
    print_u32_dec(TIM_REF_FREQ);
    print("\n", 1);
#ifdef TIM_TEST_TO_REF_SHIFT
    print_str("TIM_TEST_TO_REF_SHIFT = ");
    print_u32_dec(TIM_TEST_TO_REF_SHIFT);
    print("\n", 1);
#endif
    print_str("USE_REFERENCE = ");
    print_u32_dec(USE_REFERENCE);
    print("\n", 1);
    print_str("TEST_PRINT_INTERVAL_TICKS = ");
    print_u32_dec(TEST_PRINT_INTERVAL_TICKS);
    print("\n", 1);

    if (TEST_MAX > 512) { /* Arbitrarily chosen limit */
        print_str("Warning: Using long intervals for testing makes the result "
                  "more likely to be affected by clock drift between the "
                  "reference timer and the timer under test. This can be "
                  "detected as a skewness in the mean values between different "
                  "intervals in the results table.\n");
        if (LOG2_STATS) {
            print_str("The variance of the larger intervals may also be greater "
                      "than expected if there is significant clock drift across "
                      "the bucketed time frame\n");
        }

    }
    int res = timer_init(TIM_REF_DEV, TIM_REF_FREQ, cb_timer_periph, NULL);
    if (res < 0) {
        print_str("Error ");
        print_s32_dec(res);
        print_str(" initializing reference timer\n");
        return res;
    }
    random_init(seed);

#if !(TEST_XTIMER)
    res = timer_init(TIM_TEST_DEV, TIM_TEST_FREQ, cb_timer_periph, &test_context);
    if (res < 0) {
        print_str("Error ");
        print_s32_dec(res);
        print_str(" initializing timer under test\n");
        return res;
    }
#endif

    set_limits();

    print_str("Calibrating spin delay...\n");
    uint32_t spin_max = spin_random_calibrate(TIM_TEST_DEV, SPIN_MAX_TARGET);
    print_str("spin_max = ");
    print_u32_dec(spin_max);
    print("\n", 1);
    estimate_cpu_overhead();
#ifdef MODULE_PERIPH_RTT
    rtt_begin = rtt_get_counter();
#endif
    ref_begin = timer_read(TIM_REF_DEV);
    tut_begin = READ_TUT();
    while(1) {
        test_timer();
    }

    return 0;
}
