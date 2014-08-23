/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_hwtimer Hardware timer
 * @ingroup     core
 * @brief       Hardware timer interface
 *
 * The Hardware timers are directly mapped to hardware timers with minimum
 * latency. They are intended for short intervals and to be used in time
 * critical low-level drivers (e.g. radio). hwtimer callbacks are run in the
 * interrupt context and must use the shortest possible execution time (e.g.
 * set a flag and trigger a worker thread).
 *
 * <b>The hardware timer should not be used (until you know what
 * you're doing)<b>, use \ref sys_vtimer instead.
 *
 * @{
 *
 * @file        hwtimer.h
 * @brief       HW-timer abstraction
 *
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Michael Baar
 */

#ifndef __HWTIMER_H
#define __HWTIMER_H

#include <stdint.h>
#include "hwtimer_cpu.h"

/**
 * @brief   Number of kernel timer ticks per second
 * @def     HWTIMER_SPEED
 */
#ifndef HWTIMER_SPEED
#warning "HWTIMER_SPEED undefined. Set HWTIMER_SPEED to number of ticks per second for the current architecture."
#endif

/**
 * @brief       Convert microseconds to kernel timer ticks
 * @param[in]   us number of microseconds
 * @return      kernel timer ticks
 */
#define HWTIMER_TICKS(us)        ((us) / (1000000L / HWTIMER_SPEED))

/**
 * @brief       Convert ticks to microseconds
 * @param[in]   ticks   number of ticks
 * @return      microseconds
 */
#define HWTIMER_TICKS_TO_US(ticks)        ((ticks) * (1000000L/HWTIMER_SPEED))

/**
 * @brief   Maximum hwtimer tick count (before overflow)
 * @def     HWTIMER_MAXTICKS
 */
#ifndef HWTIMER_MAXTICKS
#warning "HWTIMER_MAXTICKS undefined. Set HWTIMER_MAXTICKS to maximum number of ticks countable on the current architecture."
#endif

/**
 * @brief   microseconds before hwtimer overflow
 */
#define HWTIMER_OVERFLOW_MICROS()        (1000000L / HWTIMER_SPEED * HWTIMER_MAXTICKS)

typedef uint32_t timer_tick_t; /**< data type for hwtimer ticks */

/**
 * @brief   initialize the hwtimer module
 */
void hwtimer_init(void);

/**
 * @brief   Get the hardware time
 * @return  The current tick count of the hardware timer
 */
unsigned long hwtimer_now(void);

/**
 * @brief Set a kernel timer
 * @param[in]   offset      Offset until callback invocation in timer ticks
 * @param[in]   callback    Callback function
 * @param[in]   ptr         Argument to callback function
 * @return      timer id
 */
int hwtimer_set(unsigned long offset, void (*callback)(void*), void *ptr);

/**
 * @brief Set a kernel timer
 * @param[in]   absolute    Absolute timer counter value for invocation of handler
 * @param[in]   callback    Callback function
 * @param[in]   ptr         Argument to callback function
 * @return      timer id
 */
int hwtimer_set_absolute(unsigned long absolute, void (*callback)(void*), void *ptr);

/**
 * @brief Remove a kernel timer
 * @param[in]   t   Id of timer to remove
 * @return      1 on success
 */
int hwtimer_remove(int t);

/**
 * @brief        Delay current thread
 * @param[in]    ticks  Number of kernel ticks to delay
 */
void hwtimer_wait(unsigned long ticks);

/**
 * @brief   determine if the hwtimer module is initialized
 * @return  1 if the hwtimer module is initialized
 */
int hwtimer_active(void);

/**
 * @brief       initialize hwtimer module data structures and hardware
 *
 * @param[in]   fcpu        cpu frequency
 */
void hwtimer_init_comp(uint32_t fcpu);

/**
 * @brief       Delay current thread, spinning. Use only in interrupts for VERY short delays!
 *
 * @param[in]   ticks        Number of kernel ticks to delay
 */
void hwtimer_spin(unsigned long ticks);

/** @} */
#endif /* __HWTIMER_H */
