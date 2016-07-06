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
#ifndef XTIMER_IMPLEMENTATION_H_
#define XTIMER_IMPLEMENTATION_H_

#ifndef XTIMER_H
#error "Do not include this file directly! Use xtimer.h instead"
#endif

#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if XTIMER_MASK
extern volatile uint32_t _xtimer_high_cnt;
#define XTIMER_PERIOD_LENGTH    XTIMER_TICKS_TO_USEC(~XTIMER_MASK + 1)
#endif

#define XTIMER_MSBMASK (((uint32_t)1)<<(XTIMER_WIDTH - 1 + XTIMER_SHIFT))

#if (XTIMER_SHIFT < 0)
#define XTIMER_USEC_TO_TICKS(value) ( (value) << -XTIMER_SHIFT )
#define XTIMER_TICKS_TO_USEC(value) ( (value) >> -XTIMER_SHIFT )
#else
#define XTIMER_USEC_TO_TICKS(value) ( (value) >> XTIMER_SHIFT )
#define XTIMER_TICKS_TO_USEC(value) ( (value) << XTIMER_SHIFT )
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
#if XTIMER_SHIFT
    return XTIMER_TICKS_TO_USEC((uint32_t)timer_read(XTIMER_DEV));
#else
    return timer_read(XTIMER_DEV);
#endif
}

/**
 * @brief drop bits of a value that don't fit into the low-level timer.
 */
static inline uint32_t _xtimer_lltimer_mask(uint32_t val)
{
    return val & ~XTIMER_MASK_SHIFTED;
}

/**
 * @{
 * @brief xtimer internal stuff
 * @internal
 */
void _xtimer_set_absolute(xtimer_t *timer);
void _xtimer_set64(xtimer_t *timer, uint32_t offset, uint32_t long_offset);
void _xtimer_sleep(uint32_t offset, uint32_t long_offset);
/** @} */

#ifndef XTIMER_MIN_SPIN
/**
 * @brief Minimal value xtimer_spin() can spin
 */
#define XTIMER_MIN_SPIN XTIMER_TICKS_TO_USEC(1)
#endif

#ifndef DOXYGEN
/* Doxygen warns that these are undocumented, but the documentation can be found in xtimer.h */

static inline uint32_t xtimer_now(void)
{
#if XTIMER_MASK
    uint32_t high_cnt = _xtimer_high_cnt;
    uint32_t now = _xtimer_lltimer_now();

    /* _xtimer_high_cnt and the underlying hardware timer both count the most
     * significant bit of the underlying hardware timer.
     *
     * Should the counter variable indicate we're in the first half of the
     * hardware timer period, but the hardware timer indicates otherwise, the
     * OR handles the missed interrupt.
     *
     * Should the counter variable indicate we're in the second half of the
     * hardware timer period, but the actual timer value does not, we
     * compensate by adding a half timer period.
     *
     * That way, timer overflow interrupts which are up to half a timer period
     * late can be compensated for.
     */

    return (high_cnt | now) +
        ((uint32_t)((high_cnt & (XTIMER_MSBMASK)) && (~now & XTIMER_MSBMASK))) * (XTIMER_PERIOD_LENGTH >> 1);
#else
    return _xtimer_lltimer_now();
#endif
}

static inline void xtimer_spin(uint32_t offset) {
    uint32_t start = _xtimer_lltimer_now();
#if XTIMER_MASK
    offset = _xtimer_lltimer_mask(offset);
    while (_xtimer_lltimer_mask(_xtimer_lltimer_now() - start) < offset);
#else
    while ((_xtimer_lltimer_now() - start) < offset);
#endif
}

static inline void xtimer_usleep(uint32_t microseconds)
{
    _xtimer_sleep(microseconds, 0);
}

static inline void xtimer_usleep64(uint64_t microseconds)
{
    _xtimer_sleep((uint32_t) microseconds, (uint32_t) (microseconds >> 32));
}

static inline void xtimer_sleep(uint32_t seconds)
{
    xtimer_usleep64((uint64_t)seconds * SEC_IN_USEC);
}

static inline void xtimer_nanosleep(uint32_t nanoseconds)
{
    _xtimer_sleep(nanoseconds / USEC_IN_NS, 0);
}

#endif /* !defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif
