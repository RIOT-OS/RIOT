/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    busy_wait   Busy Waiting low-level helpers
 * @ingroup     sys
 *
 * @brief       This modules provides helper functions for busy waiting
 *              on short intervals before timers are initialized, e.g.
 *              in `board_init()`.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @{
 */

#include <stdint.h>
#include "periph_conf.h"
#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CPU cycles per busy wait loop iteration
 *
 * This can be used to roughly estimate the number of cycles
 * for a given wait time.
 */
#ifndef CPU_CYCLES_PER_LOOP
#define CPU_CYCLES_PER_LOOP 3
#endif

/**
 * @brief   Spin for a number of cycles.
 *
 * This will not take into account cycles spent on interrupts
 * if they are enabled and occur.
 *
 * @param loops Number of loop iterations to take
 */
static inline void busy_wait(unsigned loops)
{
    while (loops) {
        /* This empty inline assembly should be enough to convince the
         * compiler that the loop cannot be optimized out. Tested with
         * GCC 12.2 and clang 16.0.0 successfully. */
        __asm__ __volatile__ (
            ""
            : /* no outputs */
            : /* no inputs */
            : /* no clobbers */
        );
        loops--;
    }
}

/**
 * @brief   Spin for a number of microseconds.
 *
 * This will roughly try to match the requested delay time, but don't
 * expect high accuracy.
 *
 * This will not take into account cycles spent on interrupts
 * if they are enabled and occur.
 *
 * @param usec  Number of µs to spin for.
 */
static inline void busy_wait_us(unsigned usec)
{
    unsigned loops = ((uint64_t)usec * CLOCK_CORECLOCK)
                   / (US_PER_SEC * CPU_CYCLES_PER_LOOP);
    busy_wait(loops);
}

#ifdef __cplusplus
}
#endif

/** @} */
