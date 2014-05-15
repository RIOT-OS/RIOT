/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_hwtimer
 * @{
 *
 * @file        hwtimer_arch.h
 * @brief       Architecture specific hwtimer API
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#ifndef HWTIMER_ARCH_H_
#define HWTIMER_ARCH_H_

#include <stdint.h>

/**
 * @brief Initialize architecture dependent kernel timer support.
 *
 * @param[in] handler   callback function for the interrupt handler
 * @param[in] fcpu      cpu frequency
 */
void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu);

/**
 * @brief Enable interrupts of hardware timers.
 */
void hwtimer_arch_enable_interrupt(void);

/**
 * @brief Disable interrupts of hardware timers.
 */
void hwtimer_arch_disable_interrupt(void);

/**
 * @brief Set a kernel timer to raise an interrupt after `offset` kernel timer
 * ticks from now.
 *
 * @param[in] offset    number of ticks
 * @param[in] timer     hardware timer identifier
 */
void hwtimer_arch_set(unsigned long offset, short timer);

/**
 * @brief Set a kernel timer to raise an interrupt at specified system time.
 *
 * @param[in] value     system time
 * @param[in] timer     hardware timer identifier
 */
void hwtimer_arch_set_absolute(unsigned long value, short timer);

/**
 * @brief Unset the kernel timer with the given timer ID.
 *
 * @param[in] timer     hardware timer identifier
 */
void hwtimer_arch_unset(short timer);

/**
 * Get the current tick count of the default hardware timer.
 *
 * @return  The current tick count of the hardware timer
 */
unsigned long hwtimer_arch_now(void);

#endif /* HWTIMER_ARCH_H_ */
