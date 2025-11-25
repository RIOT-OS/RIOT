/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_ztimer64_xtimer_compat ztimer64_xtimer_compat: 64 Bit xtimer wrapper
 * @ingroup   sys_ztimer64
 * @{
 * @file
 * @brief   ztimer64 xtimer wrapper interface
 *
 * Please check out xtimer's documentation for usage.
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef ZTIMER64_XTIMER_COMPAT_H
#define ZTIMER64_XTIMER_COMPAT_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* make sure to overwrite potentially conflicting XTIMER_WIDTH definition from
 * board.h by eagerly including it */
#include "board.h"
#include "div.h"
#include "timex.h"
#ifdef MODULE_CORE_MSG
#include "msg.h"
#endif /* MODULE_CORE_MSG */
#include "mutex.h"
#include "sched.h"

#include "ztimer.h"
#include "ztimer64.h"

#ifdef __cplusplus
extern "C" {
#endif

/* the xtimer API is documented elsewhere. This is just an (incomplete) wrapper,
 * so skip doxygen.
 */
#ifndef DOXYGEN

/* ztimer clocks with width lower than 32 bit get extended to 32 bit in software
 * via ztimer_extend. So no matter what was defined elsewhere, we overwrite it
 */
#ifdef XTIMER_WIDTH
#undef XTIMER_WIDTH
#endif

#define XTIMER_WIDTH    (32)
#define XTIMER_MASK     (0)

/**
 * a default XTIMER_BACKOFF value, this is not used by ztimer, but other code
 * uses this value to set timers
 */
#ifndef XTIMER_BACKOFF
#define XTIMER_BACKOFF  1
#endif

typedef ztimer64_t xtimer_t;
typedef uint32_t xtimer_ticks32_t;
typedef uint64_t xtimer_ticks64_t;
typedef void (*xtimer_callback_t)(void *);

static inline void xtimer_init(void)
{
    ztimer64_init();
}

static inline xtimer_ticks32_t xtimer_ticks(uint32_t ticks)
{
    return ticks;
}

static inline xtimer_ticks32_t xtimer_now(void)
{
    return ztimer64_now(ZTIMER64_USEC);
}

static inline uint32_t _xtimer_now(void)
{
    return ztimer64_now(ZTIMER64_USEC);
}

static inline xtimer_ticks64_t xtimer_now64(void)
{
    return ztimer64_now(ZTIMER64_USEC);
}

static inline void xtimer_usleep64(uint64_t microseconds)
{
    ztimer64_sleep(ZTIMER64_USEC, microseconds);
}

static inline uint32_t xtimer_now_usec(void)
{
    return ztimer64_now(ZTIMER64_USEC);
}

static inline uint64_t xtimer_now_usec64(void)
{
    return ztimer64_now(ZTIMER64_USEC);
}

static inline void xtimer_sleep(uint32_t seconds)
{
    /* TODO: use ZTIMER64_SEC */
    if (IS_ACTIVE(MODULE_ZTIMER64_MSEC)) {
        ztimer64_sleep(ZTIMER64_MSEC, ((uint64_t)seconds) * 1000LLU);
    }
    else {
        ztimer64_sleep(ZTIMER64_USEC, ((uint64_t)seconds) * 1000000LLU);
    }
}

static inline void xtimer_msleep(uint32_t milliseconds)
{
    if (IS_ACTIVE(MODULE_ZTIMER_MSEC)) {
        ztimer_sleep(ZTIMER_MSEC, milliseconds);
    }
    else {
        ztimer64_sleep(ZTIMER64_USEC, ((uint64_t)milliseconds) * 1000LLU);
    }
}

static inline void xtimer_usleep(uint32_t microseconds)
{
    ztimer_sleep(ZTIMER_USEC, microseconds);
}

static inline void xtimer_nanosleep(uint32_t nanoseconds)
{
    ztimer_sleep(ZTIMER_USEC, nanoseconds / NS_PER_US);
}

static inline void xtimer_set(xtimer_t *timer, uint32_t offset)
{
    ztimer64_set(ZTIMER64_USEC, timer, offset);
}

static inline void xtimer_remove(xtimer_t *timer)
{
    ztimer64_remove(ZTIMER64_USEC, timer);
}

static inline bool xtimer_is_set(const xtimer_t *timer)
{
    return ztimer64_is_set(timer);
}

static inline void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg,
                                  kernel_pid_t target_pid)
{
    ztimer64_set_msg(ZTIMER64_USEC, timer, offset, msg, target_pid);
}

static inline void xtimer_periodic_wakeup(xtimer_ticks32_t *last_wakeup,
                                          uint32_t period)
{
    ztimer_periodic_wakeup(ZTIMER_USEC, last_wakeup, period);
}

static inline uint32_t xtimer_usec_from_ticks(xtimer_ticks32_t ticks)
{
    return ticks;
}

static inline xtimer_ticks32_t xtimer_ticks_from_usec(uint32_t usec)
{
    return usec;
}

static inline void xtimer_now_timex(timex_t *out)
{
    uint64_t now = xtimer_now_usec64();

    out->seconds = div_u64_by_1000000(now);
    out->microseconds = now - (out->seconds * US_PER_SEC);
}

static inline int xtimer_msg_receive_timeout(msg_t *msg, uint32_t timeout)
{
    return ztimer_msg_receive_timeout(ZTIMER_USEC, msg, timeout);
}

static inline void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset,
                                     kernel_pid_t pid)
{
    ztimer64_set_wakeup(ZTIMER64_USEC, timer, offset, pid);
}

static inline int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t us)
{
    if (ztimer64_mutex_lock_timeout(ZTIMER64_USEC, mutex, us)) {
        /* Impedance matching required: Convert -ECANCELED error code to -1: */
        return -1;
    }
    return 0;
}

static inline int xtimer_rmutex_lock_timeout(rmutex_t *rmutex, uint64_t timeout)
{
    if (ztimer64_rmutex_lock_timeout(ZTIMER64_USEC, rmutex, timeout)) {
        /* Impedance matching required: Convert -ECANCELED error code to -1: */
        return -1;
    }
    return 0;
}

static inline void xtimer_set_timeout_flag64(xtimer_t *t, uint64_t timeout)
{
    ztimer64_set_timeout_flag(ZTIMER64_USEC, t, timeout);
}

static inline void xtimer_set_timeout_flag(xtimer_t *t, uint32_t timeout)
{
    xtimer_set_timeout_flag64(t, timeout);
}

static inline void xtimer_spin(xtimer_ticks32_t ticks)
{
    assert(ticks < US_PER_MS);
    ztimer_now_t start = ztimer_now(ZTIMER_USEC);

    while (ztimer_now(ZTIMER_USEC) - start < ticks) {
        /* busy waiting */
    }
}

static inline xtimer_ticks32_t xtimer_diff(xtimer_ticks32_t a,
                                           xtimer_ticks32_t b)
{
    return a - b;
}

static inline xtimer_ticks64_t xtimer_diff64(xtimer_ticks64_t a,
                                             xtimer_ticks64_t b)
{
    return a - b;
}

static inline xtimer_ticks32_t xtimer_diff32_64(xtimer_ticks64_t a,
                                                xtimer_ticks64_t b)
{
    return (xtimer_ticks32_t)(a - b);
}

static inline xtimer_ticks64_t xtimer_ticks64(uint64_t ticks)
{
    return ticks;
}

static inline bool xtimer_less(xtimer_ticks32_t a, xtimer_ticks32_t b)
{
    return a < b;
}

static inline bool xtimer_less64(xtimer_ticks64_t a, xtimer_ticks64_t b)
{
    return a < b;
}

static inline void xtimer_set64(xtimer_t *timer, uint64_t offset_us)
{
    ztimer64_set(ZTIMER64_USEC, timer, offset_us);
}

static inline void xtimer_tsleep32(xtimer_ticks32_t ticks)
{
    ztimer_sleep(ZTIMER_USEC, ticks);
}

static inline void xtimer_tsleep64(xtimer_ticks64_t ticks)
{
    ztimer64_sleep(ZTIMER64_USEC, ticks);
}

static inline void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset,
                                       kernel_pid_t pid)
{
    ztimer64_set_wakeup(ZTIMER64_USEC, timer, offset, pid);
}

#if defined(MODULE_CORE_MSG) || defined(DOXYGEN)
static inline void xtimer_set_msg64(xtimer_t *timer, uint64_t offset,
                                    msg_t *msg, kernel_pid_t target_pid)
{
    ztimer64_set_msg(ZTIMER64_USEC, timer, offset, msg, target_pid);
}

static inline int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t timeout)
{
    return ztimer64_msg_receive_timeout(ZTIMER64_USEC, msg, timeout);
}

#endif

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZTIMER64_XTIMER_COMPAT_H */
