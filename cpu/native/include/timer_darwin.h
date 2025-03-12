/*
 * Copyright (C) 2025 carl-tud
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup drivers_timer_darwin
 * @ingroup  cpu_native
 * @brief    macOS timer driver
 * @{
 *
 * Mimics LInux `timer.h` timer interface.
 */

/**
 * @file
 * @brief
 * @author carl-tud
 */
#ifndef TIMER_DARWIN_H
#define TIMER_DARWIN_H

#include <sys/time.h>
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - Creating a timer */
/**
 * @name Creating a timer
 * @{
 */
/**
 * @brief Like `itimerspec` on Linux
 */
struct itimerspec {
    /**
     * @brief Timer interval
     */
    struct timespec it_interval;
    
    /**
     * @brief Timer value
     */
    struct timespec it_value;
};

/**
 * @brief Timer
 */
typedef int timer_t;

/**
 * @brief Creates timer, like on Linux
 *
 * @param clockid Ignored on macOS.
 * @param sevp Ignored on macOS.
 * @param[out] timerid Always set to `ITIMER_REAL`
 *
 * @returns Negative integer on error. Any other result indicates success.
 */
static inline int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid) {
    (void)clockid;
    (void)sevp;
    *timerid = ITIMER_REAL;
    return 0;
}
/** @} */

/* MARK: - Controlling a timer */
/**
 * @name Controlling a timer
 * @{
 */
/**
 * @brief Sets timer
 *
 * @param tim Timer ID
 * @param flags Flags
 * @param its Timer specification
 * @param remainvalue Remaining time
 *
 * @returns Negative integer on error. Any other result indicates success.
 */
int timer_settime(timer_t tim, int flags, const struct itimerspec *its, struct itimerspec *remainvalue);

/**
 * @brief Deletes timer
 * 
 * @param tim Timer ID
 *
 * @returns Negative integer on error. Any other result indicates success.
 */
int timer_delete(timer_t tim);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TIMER_DARWIN_H */
/** @} */

