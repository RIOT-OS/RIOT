/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Configuration definitions for bench_periph_timer
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>

#include "periph/timer.h"
#include "cpu.h"
#if TEST_XTIMER
#include "xtimer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

/**
 * @brief Timer under test (TUT)
 */
#ifndef TIM_TEST_DEV
#define TIM_TEST_DEV (TIMER_DEV(0))
#endif
#ifndef TIM_TEST_FREQ
#define TIM_TEST_FREQ 1000000ul
#endif
#ifndef TIM_TEST_CHAN
#define TIM_TEST_CHAN 0
#endif

/* Use separate reference timer to compare against */
#ifndef USE_REFERENCE
#define USE_REFERENCE 1
#endif

/* Whether to keep statistics per timer target value, or only totals */
#ifndef DETAILED_STATS
#define DETAILED_STATS 1
#endif

/* Group statistics into log2 size buckets, instead of one record per timer target
 * i.e. 1, 2, 3-4, 5-8, 9-16, 17-32 etc. */
/* Only used if DETAILED_STATS is 1 */
#ifndef LOG2_STATS
#define LOG2_STATS 1
#endif

/* Margin to ensure that the rescheduling timer never is hit */
#ifndef RESCHEDULE_MARGIN
#define RESCHEDULE_MARGIN (SPIN_MAX_TARGET * 16)
#endif

/**
 * @brief Reference timer to compare against
 */
#ifndef TIM_REF_DEV
/* Avoid using the timer under test as reference */
#if ((TIM_TEST_DEV) == (TIMER_DEV(0)))
#define TIM_REF_DEV (TIMER_DEV(1))
#else
#define TIM_REF_DEV (TIMER_DEV(0))
#endif
#endif
#ifndef TIM_REF_FREQ
#define TIM_REF_FREQ 1000000ul
#endif

#if !USE_REFERENCE
#undef TIM_REF_DEV
#undef TIM_REF_FREQ
#undef TIM_TEST_TO_REF
#define TIM_REF_DEV TIM_TEST_DEV
#define TIM_REF_FREQ TIM_TEST_FREQ
#define TIM_TEST_TO_REF(x) (x)
#endif

/* Longest timer timeout tested (TUT ticks)*/
/* Reduce this if RAM usage is too high */
#ifndef TEST_MAX
#define TEST_MAX 128
#endif
/* Shortest timer timeout tested (TUT ticks) */
#ifndef TEST_MIN
#if TEST_XTIMER
/* Default minimum delay for xtimer */
#define TEST_MIN (XTIMER_ISR_BACKOFF)
#elif TIM_TEST_FREQ < 100000
/* this usually works for slow timers */
#define TEST_MIN 1
#else
/* avoid problems with timer_set_absolute setting a time in the past because of
 * processing delays */
#define TEST_MIN 16
#endif
#endif
/* Minimum delay for relative timers, should usually work with any value */
#ifndef TEST_MIN_REL
#if TEST_XTIMER
#define TEST_MIN_REL (TEST_MIN)
#else
#define TEST_MIN_REL (0)
#endif
#endif
/* Number of test values */
#define TEST_NUM ((TEST_MAX) - (TEST_MIN) + 1)
/* 2-logarithm of TEST_NUM, not possible to compute automatically by the
 * preprocessor unless comparing values like this */
#if TEST_NUM <=     (1 <<  2)
#define TEST_LOG2NUM       2
#elif TEST_NUM <=   (1 <<  3)
#define TEST_LOG2NUM       3
#elif TEST_NUM <=   (1 <<  4)
#define TEST_LOG2NUM       4
#elif TEST_NUM <=   (1 <<  5)
#define TEST_LOG2NUM       5
#elif TEST_NUM <=   (1 <<  6)
#define TEST_LOG2NUM       6
#elif TEST_NUM <=   (1 <<  7)
#define TEST_LOG2NUM       7
#elif TEST_NUM <=   (1 <<  8)
#define TEST_LOG2NUM       8
#elif TEST_NUM <=   (1 <<  9)
#define TEST_LOG2NUM       9
#elif TEST_NUM <=   (1 << 10)
#define TEST_LOG2NUM      10
#elif TEST_NUM <=   (1 << 11)
#define TEST_LOG2NUM      11
#elif TEST_NUM <=   (1 << 12)
#define TEST_LOG2NUM      12
#elif TEST_NUM <=   (1 << 13)
#define TEST_LOG2NUM      13
#elif TEST_NUM <=   (1 << 14)
#define TEST_LOG2NUM      14
#elif TEST_NUM <=   (1 << 15)
#define TEST_LOG2NUM      15
#elif TEST_NUM <=   (1 << 16)
#define TEST_LOG2NUM      16
#elif TEST_NUM <=   (1 << 20)
#define TEST_LOG2NUM      20
#elif TEST_NUM <=   (1 << 24)
#define TEST_LOG2NUM      24
#else
#define TEST_LOG2NUM      32
#endif

/* convert TUT ticks to reference ticks */
/* x is expected to be < 2**16 */
#ifndef TIM_TEST_TO_REF
#if (TIM_TEST_FREQ == TIM_REF_FREQ)
#define TIM_TEST_TO_REF(x) (x)
#elif (TIM_TEST_FREQ == 32768ul) && (TIM_REF_FREQ == 1000000ul)
#define TIM_TEST_TO_REF(x) (((uint32_t)(x) * 15625ul) >> 9)
#elif (TIM_TEST_FREQ == 1000000ul) && (TIM_REF_FREQ == 32768ul)
#define TIM_TEST_TO_REF(x) (div_u32_by_15625div512(x))
/* General conversion for Timer with 2^x factor */
#elif (TIM_TEST_FREQ < TIM_REF_FREQ ) && ((TIM_REF_FREQ % TIM_TEST_FREQ) == 0)
#ifndef TIM_TEST_TO_REF_SHIFT
#if ((TIM_REF_FREQ >> 1) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   1
#elif ((TIM_REF_FREQ >> 2) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   2
#elif ((TIM_REF_FREQ >> 3) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   3
#elif ((TIM_REF_FREQ >> 4) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   4
#elif ((TIM_REF_FREQ >> 5) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   5
#elif ((TIM_REF_FREQ >> 6) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   6
#elif ((TIM_REF_FREQ >> 7) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   7
#elif ((TIM_REF_FREQ >> 8) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   8
#elif ((TIM_REF_FREQ >> 9) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   9
#elif ((TIM_REF_FREQ >> 10) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   10
#elif ((TIM_REF_FREQ >> 11) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   11
#elif ((TIM_REF_FREQ >> 12) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   12
#elif ((TIM_REF_FREQ >> 13) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   13
#elif ((TIM_REF_FREQ >> 14) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   14
#elif ((TIM_REF_FREQ >> 15) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   15
#elif ((TIM_REF_FREQ >> 16) == TIM_TEST_FREQ)
#define TIM_TEST_TO_REF_SHIFT   16
#else
#error define TIM_TEST_TO_REF_SHIFT so that TIM_REF_FREQ == (TIM_TEST_FREQ << TIM_TEST_FREQ_SHIFT)
#endif
#endif
#define TIM_TEST_TO_REF(x) ((uint32_t)(x) << (TIM_TEST_TO_REF_SHIFT))
#else
#error define TIM_TEST_TO_REF so that TIM_REF_FREQ == TIM_TEST_TO_REF(TIM_TEST_FREQ)
#endif
#endif

/* Print results every X reference ticks */
#ifndef TEST_PRINT_INTERVAL_TICKS
#define TEST_PRINT_INTERVAL_TICKS ((TIM_REF_FREQ) * 30)
#endif

/* If variance or mean exceeds these values the row will be marked with a "SIC!"
 * in the table output */
#ifndef TEST_UNEXPECTED_STDDEV
#define TEST_UNEXPECTED_STDDEV 4
#endif
#ifndef TEST_UNEXPECTED_MEAN
#define TEST_UNEXPECTED_MEAN 10
#endif

/* The spin calibration will try to set spin_limit to a number of loop
 * iterations which correspond to this many TUT ticks */
#ifndef SPIN_MAX_TARGET
#define SPIN_MAX_TARGET 16
#endif

/* estimate_cpu_overhead will loop for this many iterations to get a proper estimate */
#define ESTIMATE_CPU_ITERATIONS 2048

#if TEST_XTIMER
#define READ_TUT() _xtimer_now()
#else
#define READ_TUT() timer_read(TIM_TEST_DEV)
#endif

#ifdef __cplusplus
}
#endif
