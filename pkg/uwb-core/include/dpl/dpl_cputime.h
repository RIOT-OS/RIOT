/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) cputime abstraction
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_CPUTIME_H
#define DPL_DPL_CPUTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "xtimer.h"
#include "hal/hal_timer.h"

/**
 * Returns the low 32 bits of cputime.
 *
 * @return uint32_t The lower 32 bits of cputime
 */
static inline uint32_t dpl_cputime_get32(void)
{
    return xtimer_now().ticks32;
}

/**
 * Converts the given number of microseconds into cputime ticks.
 *
 * @param usecs The number of microseconds to convert to ticks
 *
 * @return uint32_t The number of ticks corresponding to 'usecs'
 */
static inline uint32_t dpl_cputime_usecs_to_ticks(uint32_t usecs)
{
    return xtimer_ticks_from_usec(usecs).ticks32;
}

/**
 * Convert the given number of ticks into microseconds.
 *
 * @param ticks The number of ticks to convert to microseconds.
 *
 * @return uint32_t The number of microseconds corresponding to 'ticks'
 */
static inline uint32_t dpl_cputime_ticks_to_usecs(uint32_t ticks)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    return xtimer_usec_from_ticks(val);
}

/**
 * Wait until the number of ticks has elapsed. This is a blocking delay.
 *
 * @param ticks The number of ticks to wait.
 */
static inline void dpl_cputime_delay_ticks(uint32_t ticks)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    xtimer_tsleep32((xtimer_ticks32_t) val);
}

/**
 * Wait until 'usecs' microseconds has elapsed. This is a blocking delay.
 *
 * @param usecs The number of usecs to wait.
 */
static inline void dpl_cputime_delay_usecs(uint32_t usecs)
{
    xtimer_usleep(usecs);
}

/**
 * Initialize a CPU timer, using the given HAL timer.
 *
 * @param timer The timer to initialize. Cannot be NULL.
 * @param fp    The timer callback function. Cannot be NULL.
 * @param arg   Pointer to data object to pass to timer.
 */
static inline void dpl_cputime_timer_init(struct hal_timer *timer, hal_timer_cb fp,
        void *arg)
{
    timer->timer.callback = fp;
    timer->timer.arg = arg;
}

/**
 * Start a cputimer that will expire at 'cputime'. If cputime has already
 * passed, the timer callback will still be called (at interrupt context).
 *
 * NOTE: This must be called when the timer is stopped.
 *
 * @param timer     Pointer to timer to start. Cannot be NULL.
 * @param cputime   The cputime at which the timer should expire.
 *
 * @return int 0 on success; EINVAL if timer already started or timer struct
 *         invalid
 *
 */
static inline int dpl_cputime_timer_start(struct hal_timer *timer, uint32_t cputime)
{
    xtimer_set(&timer->timer, xtimer_now_usec() + cputime);
    return 0;
}

/**
 * Sets a cpu timer that will expire 'usecs' microseconds from the current
 * cputime.
 *
 * NOTE: This must be called when the timer is stopped.
 *
 * @param timer Pointer to timer. Cannot be NULL.
 * @param usecs The number of usecs from now at which the timer will expire.
 *
 * @return int 0 on success; EINVAL if timer already started or timer struct
 *         invalid
 */
static inline int dpl_cputime_timer_relative(struct hal_timer *timer, uint32_t usecs)
{
    uint32_t now = xtimer_now_usec();
    if (now > usecs) {
        xtimer_set(&timer->timer, now);
    } else {
        xtimer_set(&timer->timer, 0);
    }
    return 0;
}

/**
 *  @brief   Stops a cputimer from running.
 *
 * The timer is removed from the timer queue and interrupts are disabled
 * if no timers are left on the queue. Can be called even if timer is
 * not running.
 *
 * @param timer Pointer to cputimer to stop. Cannot be NULL.
 */
static inline void dpl_cputime_timer_stop(struct hal_timer *timer)
{
    xtimer_remove(&timer->timer);
}

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_CPUTIME_H */
