/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_xtimer

 * @{
 * @file
 * @brief   xtimer implementation
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#ifndef XTIMER_IMPLEMENTATION_H
#define XTIMER_IMPLEMENTATION_H

#ifndef XTIMER_H
#error "Do not include this file directly! Use xtimer.h instead"
#endif

#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if XTIMER_MASK
extern volatile uint32_t _xtimer_high_cnt;
#endif

/**
 * @brief IPC message type for xtimer msg callback
 */
#define MSG_XTIMER 12345

/**
 * @brief returns the (masked) low-level timer counter value.
 */
static inline uint32_t _xtimer_lltimer_now(void)
{
    return timer_read(XTIMER_DEV);
}

/**
 * @brief drop bits of a value that don't fit into the low-level timer.
 */
static inline uint32_t _xtimer_lltimer_mask(uint32_t val)
{
    return val & ~XTIMER_MASK;
}

/**
 * @{
 * @brief xtimer internal stuff
 * @internal
 */
uint64_t _xtimer_now64(void);
int _xtimer_set_absolute(xtimer_t *timer, uint32_t target);
void _xtimer_set64(xtimer_t *timer, uint32_t offset, uint32_t long_offset);
void _xtimer_set(xtimer_t *timer, uint32_t offset);
void _xtimer_periodic_wakeup(uint32_t *last_wakeup, uint32_t period);
void _xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid);
void _xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid);
void _xtimer_set_wakeup(xtimer_t *timer, uint32_t offset, kernel_pid_t pid);
void _xtimer_set_wakeup64(xtimer_t *timer, uint64_t offset, kernel_pid_t pid);
void _xtimer_set(xtimer_t *timer, uint32_t offset);
int _xtimer_msg_receive_timeout(msg_t *msg, uint32_t ticks);
int _xtimer_msg_receive_timeout64(msg_t *msg, uint64_t ticks);

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

static inline uint32_t _xtimer_now(void)
{
#if XTIMER_MASK
    uint32_t latched_high_cnt, now;

    /* _high_cnt can change at any time, so check the value before
     * and after reading the low-level timer. If it hasn't changed,
     * then it can be safely applied to the timer count. */

    do {
        latched_high_cnt = _xtimer_high_cnt;
        now = _xtimer_lltimer_now();
    } while (_xtimer_high_cnt != latched_high_cnt);

    return latched_high_cnt | now;
#else
    return _xtimer_lltimer_now();
#endif
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

static inline void xtimer_set_msg(xtimer_t *timer, uint32_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _xtimer_set_msg(timer, _xtimer_ticks_from_usec(offset), msg, target_pid);
}

static inline void xtimer_set_msg64(xtimer_t *timer, uint64_t offset, msg_t *msg, kernel_pid_t target_pid)
{
    _xtimer_set_msg64(timer, _xtimer_ticks_from_usec64(offset), msg, target_pid);
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
    _xtimer_set(timer, _xtimer_ticks_from_usec(offset));
}

static inline int xtimer_msg_receive_timeout(msg_t *msg, uint32_t timeout)
{
    return _xtimer_msg_receive_timeout(msg, _xtimer_ticks_from_usec(timeout));
}

static inline int xtimer_msg_receive_timeout64(msg_t *msg, uint64_t timeout)
{
    return _xtimer_msg_receive_timeout64(msg, _xtimer_ticks_from_usec64(timeout));
}

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

#endif /* !defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* XTIMER_IMPLEMENTATION_H */
