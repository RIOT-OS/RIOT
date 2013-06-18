/**
 * Hardware timer interface
 *
 * The Hardware timers are directly mapped to hardware timers with minimum
 * latency. They are intended for short intervals and to be used in time
 * critical low-level drivers (e.g. radio). hwtimer callbacks are run in the
 * interrupt context and must use the shortest possible execution time (e.g.
 * set a flag and trigger a worker thread).
 *
 * <b>hwtimer must not be used within applications</b>, use \ref vtimer
 * instead.
 *
 * @defgroup hwtimer    Hardware timer
 * @ingroup kernel
 * @{
 * @file
 *
 * @author Freie Universität Berlin, Computer Systems & Telematics
 * @author Heiko Will
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Michael Baar
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef __HWTIMER_H
#define __HWTIMER_H

#include <stdint.h>
#include <hwtimer_cpu.h>

/**
 * @def    HWTIMER_SPEED
 * @brief    Number of kernel timer ticks per second
 */
#ifndef HWTIMER_SPEED
#warning "HWTIMER_SPEED undefined. Set HWTIMER_SPEED to number of ticks per second for the current architecture."
#endif

/**
 * @brief   Convert microseconds to kernel timer ticks
 * @param[in]   us number of microseconds
 * @return      kernel timer ticks
 */
#define HWTIMER_TICKS(us)        (us / (1000000L / HWTIMER_SPEED))

/**
 * @brief       Convert ticks to microseconds
 * @param[in]   ticks   number of ticks
 * @return      microseconds
 */
#define HWTIMER_TICKS_TO_US(ticks)        (ticks * (1000000L/HWTIMER_SPEED))

/**
 * @def    HWTIMER_MAXTICKS
 * @brief    Maximum hwtimer tick count (before overflow)
 */
#ifndef HWTIMER_MAXTICKS
#warning "HWTIMER_MAXTICKS undefined. Set HWTIMER_MAXTICKS to maximum number of ticks countable on the current architecture."
#endif

/**
 * @brief    microseconds before hwtimer overflow
 */
#define HWTIMER_OVERFLOW_MICROS()        (1000000L / HWTIMER_SPEED * HWTIMER_MAXTICKS)

typedef uint32_t timer_tick_t;

void hwtimer_init(void);
void hwtimer_init_comp(uint32_t fcpu);

/**
 * @brief Set a kernel timer
 * @param[in]    offset        Offset until callback invocation in timer ticks
 * @param[in]    callback    Callback function
 * @param[in]    ptr            Argument to callback function
 * @return        timer id
 */
int hwtimer_set(unsigned long offset, void (*callback)(void*), void *ptr);

/**
 * @brief Set a kernel timer
 * @param[in]    absolute    Absolute timer counter value for invocation of handler
 * @param[in]    callback    Callback function
 * @param[in]    ptr            Argument to callback function
 * @return        timer id
 */
int hwtimer_set_absolute(unsigned long absolute, void (*callback)(void*), void *ptr);

/**
 * @brief Remove a kernel timer
 * @param[in]    t            Id of timer to remove
 * @retval    1 on success
 */
int hwtimer_remove(int t);

/**
 * @brief    Delay current thread
 * @param[in]    ticks        Number of kernel ticks to delay
 */
void hwtimer_wait(unsigned long ticks);

/**
 * @brief    Delay current thread, spinning. Use only in interrupts for VERY short delays!
 * @param[in]    ticks        Number of kernel ticks to delay
 */
void hwtimer_spin(unsigned long ticks);

int hwtimer_active(void);

/* internal */

/**
 * @brief    TODO
 * @internal
 */
void hwtimer_cpu_init(void (*handler)(int), uint32_t fcpu);
void hwtimer_t0_disable_interrupt(void);
void hwtimer_t0_enable_interrupt(void);
void hwtimer_t0_set(unsigned long value, short timer);
void hwtimer_t0_unset(short timer);
unsigned long hwtimer_t0_now(void);
unsigned long hwtimer_now(void);

/** @} */
#endif /* __HWTIMER_H */
