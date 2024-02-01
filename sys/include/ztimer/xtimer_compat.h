/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_ztimer_xtimer_compat ztimer_xtimer_compat: xtimer wrapper
 * @ingroup   sys_ztimer
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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* make sure to overwrite potentially conflicting XTIMER_WIDTH definition from
 * board.h by eagerly including it */
#include "board.h"
#include "busy_wait.h"
#include "timex.h"
#ifdef MODULE_CORE_MSG
#include "msg.h"
#endif /* MODULE_CORE_MSG */
#include "mutex.h"
#include "sched.h"
#include "macros/math.h"

#include "ztimer.h"

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

/**
 * @brief   Generate a link-time failure if a function is used that can not be
 *          implemented with the selected backends.
 */
#define _XTIMER_BACKEND_NOT_IMPLEMENTED \
        extern void xtimer_function_called_but_no_backend_available(void); \
        xtimer_function_called_but_no_backend_available()

typedef ztimer_t xtimer_t;
typedef uint32_t xtimer_ticks32_t;
typedef uint64_t xtimer_ticks64_t;

static inline void xtimer_init(void)
{
    ztimer_init();
}

static inline bool xtimer_is_set(const xtimer_t *timer)
{
    return ztimer_is_set(ZTIMER_USEC, timer);
}

static inline xtimer_ticks32_t xtimer_ticks(uint32_t ticks)
{
    return ticks;
}

static inline uint32_t xtimer_usec_from_ticks(xtimer_ticks32_t ticks)
{
    return ticks;
}

static inline uint32_t _div_round_up_u32(uint32_t a, uint32_t b)
{
    return (a + b - 1) / b;
}

static inline uint32_t _div_round_up_u64(uint64_t a, uint32_t b)
{
    return (a + b - 1) / b;
}

static inline uint32_t xtimer_msec_from_ticks(xtimer_ticks32_t ticks)
{
    return _div_round_up_u32(ticks, US_PER_MS);
}

static inline uint64_t xtimer_usec_from_ticks64(xtimer_ticks64_t ticks)
{
    return ticks;
}

static inline xtimer_ticks32_t xtimer_ticks_from_usec(uint32_t usec)
{
    return usec;
}

static inline xtimer_ticks64_t xtimer_ticks_from_usec64(uint64_t usec)
{
    return usec;
}

static inline xtimer_ticks32_t xtimer_now(void)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        return ztimer_now(ZTIMER_USEC);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        return ztimer_now(ZTIMER_MSEC) * US_PER_MS;
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        return 0;
    }
}

static inline uint32_t xtimer_now_usec(void)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        return ztimer_now(ZTIMER_USEC);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        return ztimer_now(ZTIMER_MSEC) * US_PER_MS;
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        return 0;
    }
}

static inline void _ztimer_sleep_scale(ztimer_clock_t *clock, uint32_t time,
                                       uint32_t scale)
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
    if (IS_USED(MODULE_ZTIMER_MSEC)) {
        _ztimer_sleep_scale(ZTIMER_MSEC, seconds, MS_PER_SEC);
    }
    else {
        _ztimer_sleep_scale(ZTIMER_USEC, seconds, US_PER_SEC);
    }
}

static inline void xtimer_msleep(uint32_t milliseconds)
{
    if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_sleep(ZTIMER_MSEC, milliseconds);
    }
    else if (IS_USED(MODULE_ZTIMER_USEC)) {
        _ztimer_sleep_scale(ZTIMER_USEC, milliseconds, US_PER_MS);
    }
    else {
        busy_wait_us(US_PER_MS * milliseconds);
    }
}

static inline void xtimer_usleep(uint32_t microseconds)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_sleep(ZTIMER_USEC, microseconds);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_sleep(ZTIMER_MSEC, _div_round_up_u32(microseconds, US_PER_MS));
    }
    else {
        busy_wait_us(microseconds);
    }
}

static inline void xtimer_nanosleep(uint32_t nanoseconds)
{
    xtimer_usleep(nanoseconds / NS_PER_US);
}

static inline void xtimer_tsleep32(xtimer_ticks32_t ticks)
{
    xtimer_usleep(xtimer_usec_from_ticks(ticks));
}

static inline void xtimer_tsleep64(xtimer_ticks64_t ticks)
{
    const uint32_t max_sleep = UINT32_MAX;
    uint64_t time = xtimer_usec_from_ticks64(ticks);

    while (time > max_sleep) {
        xtimer_usleep(max_sleep);
        time -= max_sleep;
    }
    xtimer_usleep(time);
}

static inline void xtimer_set(xtimer_t *timer, uint32_t offset)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_set(ZTIMER_USEC, timer, offset);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_set(ZTIMER_MSEC, timer, _div_round_up_u32(offset, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline void xtimer_remove(xtimer_t *timer)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_remove(ZTIMER_USEC, timer);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_remove(ZTIMER_MSEC, timer);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg,
                                  kernel_pid_t target_pid)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_set_msg(ZTIMER_USEC, timer, offset, msg, target_pid);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_set_msg(ZTIMER_MSEC, timer, _div_round_up_u32(offset, US_PER_MS),
                       msg, target_pid);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline void xtimer_periodic_wakeup(xtimer_ticks32_t *last_wakeup,
                                          uint32_t period)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_periodic_wakeup(ZTIMER_USEC, last_wakeup, period);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_periodic_wakeup(ZTIMER_MSEC, last_wakeup, _div_round_up_u32(period, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline int xtimer_msg_receive_timeout(msg_t *msg, uint32_t timeout)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        return ztimer_msg_receive_timeout(ZTIMER_USEC, msg, timeout);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        return ztimer_msg_receive_timeout(ZTIMER_MSEC, msg, _div_round_up_u32(timeout, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset,
                                     kernel_pid_t pid)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_set_wakeup(ZTIMER_USEC, timer, offset, pid);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_set_wakeup(ZTIMER_MSEC, timer, _div_round_up_u32(offset, US_PER_MS), pid);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t us)
{
    int res;

    if (IS_USED(MODULE_ZTIMER_USEC)) {
        assert(us <= UINT32_MAX);
        res = ztimer_mutex_lock_timeout(ZTIMER_USEC, mutex, (uint32_t)us);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        us = _div_round_up_u64(us, US_PER_MS);
        assert(us <= UINT32_MAX);
        res = ztimer_mutex_lock_timeout(ZTIMER_MSEC, mutex, (uint32_t)us);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        res = -1;
    }

    /* Impedance matching required: Convert -ECANCELED error code to -1: */
    return res ? -1 : 0;
}

static inline int xtimer_rmutex_lock_timeout(rmutex_t *mutex, uint64_t us)
{
    int res;

    if (IS_USED(MODULE_ZTIMER_USEC)) {
        assert(us <= UINT32_MAX);
        res = ztimer_rmutex_lock_timeout(ZTIMER_USEC, mutex, (uint32_t)us);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        us = _div_round_up_u64(us, US_PER_MS);
        assert(us <= UINT32_MAX);
        res = ztimer_rmutex_lock_timeout(ZTIMER_MSEC, mutex, (uint32_t)us);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        res = -1;
    }

    /* Impedance matching required: Convert -ECANCELED error code to -1: */
    return res ? -1 : 0;
}

static inline void xtimer_set_timeout_flag(xtimer_t *t, uint32_t timeout)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_set_timeout_flag(ZTIMER_USEC, t, timeout);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_set_timeout_flag(ZTIMER_MSEC, t, _div_round_up_u32(timeout, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline void xtimer_set_timeout_flag64(xtimer_t *t, uint64_t timeout)
{
    assert(timeout <= UINT32_MAX);
    xtimer_set_timeout_flag(t, timeout);
}

static inline void xtimer_spin(xtimer_ticks32_t ticks)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_spin(ZTIMER_USEC, xtimer_usec_from_ticks(ticks));
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_spin(ZTIMER_MSEC, xtimer_msec_from_ticks(ticks));
    }
    else {
        busy_wait_us(xtimer_usec_from_ticks(ticks));
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

/* unsupported due to using ztimer (32Bit):
 * please use ztimer64_xtimer_compat if need

   xtimer_ticks64_t xtimer_now64(void);
   uint64_t xtimer_now_usec64(void):
   void xtimer_now_timex(timex_t *out)
   void xtimer_set64(xtimer_t *timer, uint64_t offset_us);
   void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset,
                                       kernel_pid_t pid);
   void xtimer_set_msg64(xtimer_t *timer, uint64_t offset,
                                    msg_t *msg, kernel_pid_t target_pid);
   int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t timeout);
 */

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ZTIMER_XTIMER_COMPAT_H */
