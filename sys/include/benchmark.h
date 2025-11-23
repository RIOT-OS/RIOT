/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_benchmark Benchmark
 * @ingroup     sys
 * @brief       Framework for running simple runtime benchmarks
 * @{
 *
 * @file
 * @brief       Interface for running simple benchmarks
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

#include "irq.h"
#include "ztimer/stopwatch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Measure the runtime of a given function call
 *
 * As we are doing a time sensitive measurement here, there is no way around
 * using a preprocessor function, as going with a function pointer or similar
 * would influence the measured runtime...
 *
 * @param[in] name      name for labeling the output
 * @param[in] runs      number of times to run @p func
 * @param[in] func      function call to benchmark
 */
#define BENCHMARK_FUNC(name, runs, func)                        \
    do {                                                        \
        ztimer_stopwatch_t timer = { .clock = ZTIMER_USEC };    \
        ztimer_stopwatch_start(&timer);                         \
        for (unsigned long i = 0; i < runs; i++) {              \
            func;                                               \
        }                                                       \
        benchmark_print_time(ztimer_stopwatch_measure(&timer), runs, name); \
        ztimer_stopwatch_stop(&timer);                          \
    } while (0)

/**
 * @brief   Output the given time as well as the time per run on STDIO
 *
 * @param[in] time      overall runtime in us
 * @param[in] runs      number of runs
 * @param[in] name      name to label the output
 */
void benchmark_print_time(uint32_t time, unsigned long runs, const char *name);

#ifdef __cplusplus
}
#endif

/** @} */
