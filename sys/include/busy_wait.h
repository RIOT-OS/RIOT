/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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
#  define CPU_CYCLES_PER_LOOP 3
#endif

/**
 * @brief   Spin for a number of cycles.
 *
 * This will not take into account cycles spent on interrupts
 * if they are enabled and occur.
 *
 * @note The `loops` parameter does not specify the actual number of cycles this
 *       will wait for! Even with `CPU_CYCLES_PER_LOOP` calibrated, this is a
 *       rough delay only!
 *
 * @param loops Number of loop iterations to take
 */
static inline void busy_wait(unsigned loops)
{
    while (loops) {
        /* The empty volatile inline assembly prevents the compiler from
         * optimizing out the loop. Declaring `loops` as read-write operand
         * hides its value from the compiler, so the number of iterations
         * is unknown at compile time and the loop cannot be unrolled. */
        __asm__ __volatile__ (
            ""
            : "+r" (loops) /* no actual output */
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
