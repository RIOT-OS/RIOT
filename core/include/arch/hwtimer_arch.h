/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup	    core_arch
 * @{
 *
 * @file        hwtimer_arch.h
 * @brief       The kernel's hardware timer abstraction interface
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef HWTIMER_ARCH_H_
#define HWTIMER_ARCH_H_

#include <stdint.h>

/**
 * @brief Initialize architecture dependent kernel timer support
 *
 * @brief[in] handler   callback that is called when timer offset is reached
 * @brief[in] fcpu      the core CPU-frequency for tick interval calculation
 */
void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu);

/**
 * @brief Enable interrupts of hardware timers
 */
void hwtimer_arch_enable_interrupt(void);

/**
 * @brief Disable interrupts of hardware timers
 */
void hwtimer_arch_disable_interrupt(void);

/**
 * @brief Set a kernel timer to raise an interrupt after ::offset kernel timer
 *              ticks from now
 *
 * @param[in] offset    number of ticks until the timer fires
 * @param[in] timer     the channel to set
 */
void hwtimer_arch_set(unsigned long offset, short timer);

/**
 * @brief Unset the kernel timer with the given timer ID
 *
 * @param[in] timer     the channel to unset
 */
void hwtimer_arch_unset(short timer);

/**
 * @brief Get the current tick count of the default hardware timer
 *
 * @return              the current value of the hwtimer
 */
unsigned long hwtimer_arch_now(void);


#endif /* HWTIMER_ARCH_H_ */
/** @} */
