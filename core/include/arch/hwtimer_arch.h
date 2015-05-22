/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file
 * @brief       The kernel's hardware timer abstraction interface
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef HWTIMER_ARCH_H_
#define HWTIMER_ARCH_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize architecture dependent kernel timer support
 *
 * @param[in] handler   callback that is called when timer @p offset is reached
 * @param[in] fcpu      the core CPU-frequency for tick interval calculation
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
 * @brief Set a kernel timer to raise an interrupt after @p offset kernel timer
 *              ticks from now
 *
 * @param[in] offset    number of ticks until the timer fires
 * @param[in] timer     the channel to set
 */
void hwtimer_arch_set(unsigned long offset, short timer);

/**
 * @brief Set a kernel timer to raise an interrupt at specified system time.
 *
 * @param[in] value     absolute timer tick value to set a timer channel to
 * @param[in] timer     the channel to set
 */
void hwtimer_arch_set_absolute(unsigned long value, short timer);

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

#ifdef __cplusplus
}
#endif

#endif /* HWTIMER_ARCH_H_ */
/** @} */
