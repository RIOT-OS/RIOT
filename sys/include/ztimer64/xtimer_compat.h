/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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

/**
 * @brief   Generate a link-time failure if a function is used that can not be
 *          implemented with the selected backends.
 */
#define _XTIMER_BACKEND_NOT_IMPLEMENTED \
    extern void xtimer_function_called_but_no_backend_available(void); \
    xtimer_function_called_but_no_backend_available()

typedef ztimer64_t xtimer_t;
typedef uint32_t xtimer_ticks32_t;
typedef uint64_t xtimer_ticks64_t;
typedef void (*xtimer_callback_t)(void *);

static inline uint32_t _div_round_up_u32(uint32_t a, uint32_t b)
{
    return (a + b - 1) / b;
}

static inline uint32_t _div_round_up_u64(uint64_t a, uint32_t b)
{
    return (a + b - 1) / b;
}

static inline void xtimer_init(void)
{
    ztimer64_init();
}

static inline xtimer_ticks32_t xtimer_ticks(uint32_t ticks)
{
    return ticks;
}

static inline uint64_t xtimer_usec_from_ticks64(xtimer_ticks64_t ticks)
{
    return ticks;
}

static inline uint32_t xtimer_usec_from_ticks(xtimer_ticks32_t ticks)
{
    return ticks;
}

static inline uint32_t xtimer_msec_from_ticks(xtimer_ticks32_t ticks)
{
    return _div_round_up_u32(ticks, US_PER_MS);
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
    return xtimer_now();
}

static inline uint32_t xtimer_now_msec(void)
{
    if (IS_USED(MODULE_ZTIMER_MSEC)) {
        return ztimer_now(ZTIMER_MSEC);
    }
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        return ztimer64_now(ZTIMER64_USEC) / US_PER_MS;
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        return 0;
    }
}

static inline uint64_t xtimer_now_usec64(void)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        return ztimer64_now(ZTIMER64_USEC);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        return ztimer64_now(ZTIMER64_MSEC) * US_PER_MS;
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        return 0;
    }
}

static inline uint32_t _xtimer_now(void)
{
    return ztimer64_now(ZTIMER64_USEC);
}

static inline xtimer_ticks64_t xtimer_now64(void)
{
    return xtimer_now_usec64();
}

static inline void xtimer_now_timex(timex_t *out)
{
    uint64_t now = xtimer_now_usec64();

    out->seconds = div_u64_by_1000000(now);
    out->microseconds = now - (out->seconds * US_PER_SEC);
}

static inline void xtimer_usleep64(uint64_t microseconds)
{
    ztimer64_sleep(ZTIMER64_USEC, microseconds);
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
    else if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer64_sleep(ZTIMER64_USEC, ((uint64_t)milliseconds) * 1000LLU);
    }
    else {
        assume(0);
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
        assume(0);
    }
}

static inline void xtimer_set(xtimer_t *timer, uint32_t offset)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer64_set(ZTIMER64_USEC, timer, offset);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer64_set(ZTIMER64_MSEC, timer, _div_round_up_u32(offset, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline void xtimer_remove(xtimer_t *timer)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer64_remove(ZTIMER64_USEC, timer);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer64_remove(ZTIMER64_MSEC, timer);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline bool xtimer_is_set(const xtimer_t *timer)
{
    return ztimer64_is_set(timer);
}

static inline void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg,
                                  kernel_pid_t target_pid)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer64_set_msg(ZTIMER64_USEC, timer, offset, msg, target_pid);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer64_set_msg(ZTIMER64_MSEC, timer, _div_round_up_u32(offset, US_PER_MS),
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
        return 0;
    }
}

static inline void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset,
                                     kernel_pid_t pid)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer64_set_wakeup(ZTIMER64_USEC, timer, offset, pid);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer64_set_wakeup(ZTIMER64_MSEC, timer, _div_round_up_u32(offset, US_PER_MS), pid);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline int xtimer_mutex_lock_timeout(mutex_t *mutex, uint64_t us)
{
    int res;

    if (IS_USED(MODULE_ZTIMER_USEC)) {
        res = ztimer64_mutex_lock_timeout(ZTIMER64_USEC, mutex, us);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        us = _div_round_up_u64(us, US_PER_MS);
        res = ztimer64_mutex_lock_timeout(ZTIMER64_MSEC, mutex, us);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        res = -1;
    }

    /* Impedance matching required: Convert -ECANCELED error code to -1: */
    return res ? -1 : 0;
}

static inline int xtimer_rmutex_lock_timeout(rmutex_t *rmutex, uint64_t us)
{
    int res;

    if (IS_USED(MODULE_ZTIMER_USEC)) {
        res = ztimer64_rmutex_lock_timeout(ZTIMER64_USEC, rmutex, us);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        us = _div_round_up_u64(us, US_PER_MS);
        res = ztimer64_rmutex_lock_timeout(ZTIMER64_MSEC, rmutex, us);
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
        ztimer64_set_timeout_flag(ZTIMER64_USEC, t, timeout);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer64_set_timeout_flag(ZTIMER64_MSEC, t, _div_round_up_u32(timeout, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline void xtimer_set_timeout_flag64(xtimer_t *t, uint64_t timeout)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer64_set_timeout_flag(ZTIMER64_USEC, t, timeout);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer64_set_timeout_flag(ZTIMER64_MSEC, t, _div_round_up_u64(timeout, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
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
        assume(0);
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
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer64_set_msg(ZTIMER64_USEC, timer, offset, msg, target_pid);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer64_set_msg(ZTIMER64_MSEC, timer,
                         _div_round_up_u64(offset, US_PER_MS), msg, target_pid);
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
    }
}

static inline int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t timeout)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        return ztimer64_msg_receive_timeout(ZTIMER64_USEC, msg, timeout);
    }
    else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        return ztimer64_msg_receive_timeout(ZTIMER64_MSEC, msg,
                                            _div_round_up_u64(timeout, US_PER_MS));
    }
    else {
        _XTIMER_BACKEND_NOT_IMPLEMENTED;
        return 0;
    }
}

#endif

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
