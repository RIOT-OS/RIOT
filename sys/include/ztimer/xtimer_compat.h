/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_ztimer_util
 * @{
 * @file
 * @brief   ztimer xtimer wrapper interface
 *
 * Please check out xtimer's documentation for usage.
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef ZTIMER_XTIMER_COMPAT_H
#define ZTIMER_XTIMER_COMPAT_H

#include <stdbool.h>
#include <stdint.h>

#include "div.h"
#include "timex.h"
#ifdef MODULE_CORE_MSG
#include "msg.h"
#endif /* MODULE_CORE_MSG */
#include "mutex.h"
#include "kernel_types.h"

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* the xtimer API is documented elsewhere. This is just an (incomplete) wrapper,
 * so skip doxygen.
 */
#ifndef DOXYGEN

typedef ztimer_t xtimer_t;
typedef uint32_t xtimer_ticks32_t;
typedef uint64_t xtimer_ticks64_t;

static inline xtimer_ticks32_t xtimer_now(void)
{
    return ztimer_now(ZTIMER_USEC);
}

static inline xtimer_ticks64_t xtimer_now64(void)
{
    return ztimer_now(ZTIMER_USEC);
}

/*static void xtimer_now_timex(timex_t *out) {
   }*/

static inline uint32_t xtimer_now_usec(void)
{
    return ztimer_now(ZTIMER_USEC);
}

static inline uint64_t xtimer_now_usec64(void)
{
    return ztimer_now(ZTIMER_USEC);
}

static inline void _ztimer_sleep_scale(ztimer_clock_t *clock, uint32_t time, uint32_t scale)
{
    const uint32_t max_sleep = UINT32_MAX / scale;

    while (time > max_sleep) {
        ztimer_sleep(clock, max_sleep * scale);
        time -= max_sleep;
    }

    ztimer_sleep(clock, time * scale);
}

static inline void xtimer_sleep(uint32_t seconds)
{
    /* TODO: use ZTIMER_SEC */
    if (IS_ACTIVE(MODULE_ZTIMER_MSEC)) {
        _ztimer_sleep_scale(ZTIMER_MSEC, seconds, 1000);
    } else {
        _ztimer_sleep_scale(ZTIMER_USEC, seconds, 1000000);
    }
}

static inline void xtimer_msleep(uint32_t milliseconds)
{
    if (IS_ACTIVE(MODULE_ZTIMER_MSEC)) {
        ztimer_sleep(ZTIMER_MSEC, milliseconds);
    } else {
        _ztimer_sleep_scale(ZTIMER_USEC, seconds, 1000);
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
    ztimer_set(ZTIMER_USEC, timer, offset);
}

static inline void xtimer_remove(xtimer_t *timer)
{
    ztimer_remove(ZTIMER_USEC, timer);
}

static inline void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg,
                                  kernel_pid_t target_pid)
{
    ztimer_set_msg(ZTIMER_USEC, timer, offset, msg, target_pid);
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
    ztimer_set_wakeup(ZTIMER_USEC, timer, offset, pid);
}

/*
   static inline void xtimer_set64(xtimer_t *timer, uint64_t offset_us);
   static inline void xtimer_tsleep32(xtimer_ticks32_t ticks);
   static inline void xtimer_tsleep64(xtimer_ticks64_t ticks);
   static inline void xtimer_spin(xtimer_ticks32_t ticks);
   static inline void xtimer_periodic_wakeup(xtimer_ticks32_t *last_wakeup,
                                          uint32_t period);
   static inline void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset,
                                       kernel_pid_t pid);
   static inline xtimer_ticks32_t xtimer_ticks_from_usec(uint32_t usec);
   static inline xtimer_ticks64_t xtimer_ticks_from_usec64(uint64_t usec);
   static inline uint32_t xtimer_usec_from_ticks(xtimer_ticks32_t ticks);
   static inline uint64_t xtimer_usec_from_ticks64(xtimer_ticks64_t ticks);
   static inline xtimer_ticks32_t xtimer_ticks(uint32_t ticks);
   static inline xtimer_ticks64_t xtimer_ticks64(uint64_t ticks);
   static inline xtimer_ticks32_t xtimer_diff(xtimer_ticks32_t a,
                                           xtimer_ticks32_t b);
   static inline xtimer_ticks64_t xtimer_diff64(xtimer_ticks64_t a,
                                             xtimer_ticks64_t b);
   static inline xtimer_ticks32_t xtimer_diff32_64(xtimer_ticks64_t a,
                                                xtimer_ticks64_t b);
   static inline bool xtimer_less(xtimer_ticks32_t a, xtimer_ticks32_t b);
   static inline bool xtimer_less64(xtimer_ticks64_t a, xtimer_ticks64_t b);
   int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t us);
   void xtimer_set_timeout_flag(xtimer_t *t, uint32_t timeout);

 #if defined(MODULE_CORE_MSG) || defined(DOXYGEN)
   static inline void xtimer_set_msg64(xtimer_t *timer, uint64_t offset,
                                    msg_t *msg, kernel_pid_t target_pid);
   static inline int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t timeout);
 #endif
 */

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZTIMER_XTIMER_COMPAT_H */
