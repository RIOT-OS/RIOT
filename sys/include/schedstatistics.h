/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    schedstatistics Schedstatistics
 * @ingroup     sys
 * @brief       When including this module scheduler statistics
 *              (@ref schedstat_t) for a thread will be updated on every
 *              @ref sched_run().
 *
 * @note        If auto_init is disabled `init_schedstatistics()` needs to be
 *              called as well as xtimer_init().
 * @{
 *
 * @file
 * @brief       Scheduler statisctics
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 *  Scheduler statistics
 */
typedef struct {
    uint32_t laststart;      /**< Time stamp of the last time this thread was
                                  scheduled to run */
    unsigned int schedules;  /**< How often the thread was scheduled to run */
    uint64_t runtime_us;     /**< The total runtime of this thread in microseconds */
} schedstat_t;

/**
 *  Thread statistics table
 */
extern schedstat_t sched_pidlist[KERNEL_PID_LAST + 1];

/**
 *  @brief  Registers the sched statistics callback and sets laststart for
 *          caller thread
 */
void init_schedstatistics(void);

#ifdef __cplusplus
}
#endif

/** @} */
