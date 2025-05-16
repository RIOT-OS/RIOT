/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Eistec AB
 *               2018 Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup   sys_xtimer

 * @{
 * @file
 * @brief   xtimer implementation
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author  Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 */

#ifndef XTIMER_H
#error "Do not include this file directly! Use xtimer.h instead"
#endif

#ifdef MODULE_XTIMER_ON_ZTIMER
#include "ztimer.h"
#else
#include "periph/timer.h"
#endif

#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint64_t _xtimer_current_time;

/**
 * @brief IPC message type for xtimer msg callback
 */
#define MSG_XTIMER 12345

/**
 * @brief returns the (masked) low-level timer counter value.
 */
static inline uint32_t _xtimer_lltimer_now(void)
{
#ifndef MODULE_XTIMER_ON_ZTIMER
    return timer_read(XTIMER_DEV);
#else
    return ztimer_now(ZTIMER_USEC);
#endif

}

/**
 * @brief drop bits of a value that don't fit into the low-level timer.
 */
static inline uint32_t _xtimer_lltimer_mask(uint32_t val)
{
    /* cppcheck-suppress shiftTooManyBits
     * (reason: cppcheck bug. `XTIMER_MASK` is zero when `XTIMER_WIDTH` is 32) */
    return val & ~XTIMER_MASK;
}

/**
 * @{
 * @brief xtimer internal stuff
 * @internal
 */

uint32_t _xtimer_now(void);

void _xtimer_set64(xtimer_t *timer, uint32_t offset, uint32_t long_offset);
void _xtimer_periodic_wakeup(uint32_t *last_wakeup, uint32_t period);
void _xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid);
void _xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid);
#ifdef MODULE_CORE_MSG
int _xtimer_msg_receive_timeout(msg_t *msg, uint32_t ticks);
int _xtimer_msg_receive_timeout64(msg_t *msg, uint64_t ticks);
void _xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid);
void _xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid);
#endif /* MODULE_CORE_MSG */

/**
 * @brief  Sleep for the given number of ticks
 */
void _xtimer_tsleep(uint32_t offset, uint32_t long_offset);
/** @} */

#ifndef XTIMER_MIN_SPIN
/**
 * @brief Minimal value xtimer_spin() can spin
 */
#define XTIMER_MIN_SPIN _xtimer_usec_from_ticks(1)
#endif

#ifndef DOXYGEN
/* Doxygen warns that these are undocumented, but the documentation can be found in xtimer.h */

static inline uint64_t _xtimer_now64(void)
{
    uint32_t now, elapsed;

    /* time sensitive since _xtimer_current_time is updated here */
    unsigned state = irq_disable();
    now = _xtimer_lltimer_now();
#if XTIMER_MASK
    elapsed = _xtimer_lltimer_mask(now - _xtimer_lltimer_mask((uint32_t)_xtimer_current_time));
    _xtimer_current_time += (uint64_t)elapsed;
#else
    elapsed = now - ((uint32_t)_xtimer_current_time & 0xFFFFFFFF);
    _xtimer_current_time += (uint64_t)elapsed;
#endif
    irq_restore(state);

    return _xtimer_current_time;
}

static inline xtimer_ticks32_t xtimer_now(void)
{
    xtimer_ticks32_t ret;
    ret.ticks32 = _xtimer_now();
    return ret;
}

static inline xtimer_ticks64_t xtimer_now64(void)
{
    xtimer_ticks64_t ret;
    ret.ticks64 = _xtimer_now64();
    return ret;
}

static inline uint32_t xtimer_now_usec(void)
{
    return xtimer_usec_from_ticks(xtimer_now());
}

static inline uint64_t xtimer_now_usec64(void)
{
    return xtimer_usec_from_ticks64(xtimer_now64());
}

static inline void _xtimer_spin(uint32_t offset) {
    uint32_t start = _xtimer_lltimer_now();
#if XTIMER_MASK
    offset = _xtimer_lltimer_mask(offset);
    while (_xtimer_lltimer_mask(_xtimer_lltimer_now() - start) < offset);
#else
    while ((_xtimer_lltimer_now() - start) < offset);
#endif
}

static inline void _xtimer_tsleep32(uint32_t ticks)
{
    _xtimer_tsleep(ticks, 0);
}

static inline void _xtimer_tsleep64(uint64_t ticks)
{
    _xtimer_tsleep((uint32_t)ticks, (uint32_t)(ticks >> 32));
}

static inline void xtimer_spin(xtimer_ticks32_t ticks) {
    _xtimer_spin(ticks.ticks32);
}

static inline void xtimer_msleep(uint32_t milliseconds)
{
    _xtimer_tsleep64(_xtimer_ticks_from_usec64(milliseconds * US_PER_MS));
}

static inline void xtimer_usleep(uint32_t microseconds)
{
    _xtimer_tsleep32(_xtimer_ticks_from_usec(microseconds));
}

static inline void xtimer_usleep64(uint64_t microseconds)
{
    _xtimer_tsleep64(_xtimer_ticks_from_usec64(microseconds));
}

static inline void xtimer_sleep(uint32_t seconds)
{
    _xtimer_tsleep64(_xtimer_ticks_from_usec64((uint64_t)seconds * US_PER_SEC));
}

static inline void xtimer_nanosleep(uint32_t nanoseconds)
{
    _xtimer_tsleep32(_xtimer_ticks_from_usec(nanoseconds / NS_PER_US));
}

static inline void xtimer_tsleep32(xtimer_ticks32_t ticks)
{
    _xtimer_tsleep32(ticks.ticks32);
}

static inline void xtimer_tsleep64(xtimer_ticks64_t ticks)
{
    _xtimer_tsleep64(ticks.ticks64);
}

static inline void xtimer_periodic_wakeup(xtimer_ticks32_t *last_wakeup, uint32_t period)
{
    _xtimer_periodic_wakeup(&last_wakeup->ticks32, _xtimer_ticks_from_usec(period));
}

static inline void xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid)
{
    _xtimer_set_wakeup(timer, _xtimer_ticks_from_usec(offset), pid);
}

static inline void xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid)
{
    _xtimer_set_wakeup64(timer, _xtimer_ticks_from_usec64(offset), pid);
}

static inline void xtimer_set(xtimer_t *timer, uint32_t offset)
{
    _xtimer_set64(timer, _xtimer_ticks_from_usec(offset), 0);
}

static inline void xtimer_set64(xtimer_t *timer, uint64_t period_us)
{
    uint64_t ticks = _xtimer_ticks_from_usec64(period_us);
    _xtimer_set64(timer, ticks, ticks >> 32);
}

#ifdef MODULE_CORE_MSG
static inline int xtimer_msg_receive_timeout(msg_t *msg, uint32_t timeout)
{
    return _xtimer_msg_receive_timeout(msg, _xtimer_ticks_from_usec(timeout));
}

static inline int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t timeout)
{
    return _xtimer_msg_receive_timeout64(msg, _xtimer_ticks_from_usec64(timeout));
}

static inline void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _xtimer_set_msg(timer, _xtimer_ticks_from_usec(offset), msg, target_pid);
}

static inline void xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _xtimer_set_msg64(timer, _xtimer_ticks_from_usec64(offset), msg, target_pid);
}
#endif /* MODULE_CORE_MSG */

static inline xtimer_ticks32_t xtimer_ticks_from_usec(uint32_t usec)
{
    xtimer_ticks32_t ticks;
    ticks.ticks32 = _xtimer_ticks_from_usec(usec);
    return ticks;
}

static inline xtimer_ticks64_t xtimer_ticks_from_usec64(uint64_t usec)
{
    xtimer_ticks64_t ticks;
    ticks.ticks64 = _xtimer_ticks_from_usec64(usec);
    return ticks;
}

static inline uint32_t xtimer_usec_from_ticks(xtimer_ticks32_t ticks)
{
    return _xtimer_usec_from_ticks(ticks.ticks32);
}

static inline uint64_t xtimer_usec_from_ticks64(xtimer_ticks64_t ticks)
{
    return _xtimer_usec_from_ticks64(ticks.ticks64);
}

static inline xtimer_ticks32_t xtimer_ticks(uint32_t ticks)
{
    xtimer_ticks32_t ret;
    ret.ticks32 = ticks;
    return ret;
}

static inline xtimer_ticks64_t xtimer_ticks64(uint64_t ticks)
{
    xtimer_ticks64_t ret;
    ret.ticks64 = ticks;
    return ret;
}

static inline xtimer_ticks32_t xtimer_diff(xtimer_ticks32_t a, xtimer_ticks32_t b)
{
    xtimer_ticks32_t ret;
    ret.ticks32 = a.ticks32 - b.ticks32;
    return ret;
}

static inline xtimer_ticks64_t xtimer_diff64(xtimer_ticks64_t a, xtimer_ticks64_t b)
{
    xtimer_ticks64_t ret;
    ret.ticks64 = a.ticks64 - b.ticks64;
    return ret;
}

static inline xtimer_ticks32_t xtimer_diff32_64(xtimer_ticks64_t a, xtimer_ticks64_t b)
{
    uint64_t diff = a.ticks64 - b.ticks64;
    xtimer_ticks32_t ret;
    ret.ticks32 = diff;
    return ret;
}

static inline bool xtimer_less(xtimer_ticks32_t a, xtimer_ticks32_t b)
{
    return (a.ticks32 < b.ticks32);
}

static inline bool xtimer_less64(xtimer_ticks64_t a, xtimer_ticks64_t b)
{
    return (a.ticks64 < b.ticks64);
}

static inline bool xtimer_is_set(const xtimer_t *timer)
{
    return timer->offset || timer->long_offset;
}

#endif /* !defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
