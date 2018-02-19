/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Prepare the current scope for running BENCHMARK_x() macros
 */
#define BENCHMARK_SETUP()                       \
    unsigned state;                             \
    unsigned time

/**
 * @brief   Measure the runtime of a given function call
 *
 * As we are doing a time sensitive measurement here, there is no way around
 * using a preprocessor function, as going with a function pointer or similar
 * would influence the measured runtime...
 *
 * @note    BENCHMARK_SETUP() needs to be called in the same scope
 *
 * @param[in] name      name for labeling the output
 * @param[in] runs      number of times to run @p func
 * @param[in] func      function call to benchmark
 */
#define BENCHMARK_FUNC(name, runs, func)        \
    state = irq_disable();                      \
    time = xtimer_now_usec();                   \
    for (unsigned long i = 0; i < runs; i++) {  \
        func;                                   \
    }                                           \
    time = (xtimer_now_usec() - time);          \
    irq_restore(state);                         \
    benchmark_print_time(time, runs, name)

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

#endif /* BENCHMARK_H */
/** @} */
