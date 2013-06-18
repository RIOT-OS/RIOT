/**
 * Hardware timer abstraction
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup	kernel
 * @{
 * @file
 * @author Freie Universität Berlin, Computer Systems & Telematics
 * @author Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author Heiko Will <hwill@inf.fu-berlin.de>
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 */
#ifndef HWTIMER_ARCH_H_
#define HWTIMER_ARCH_H_

#include <stdint.h>

/**
 * Initialize architecture dependent kernel timer support.
 */
void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu);

/**
 * Enable interrupts of hardware timers.
 */
void hwtimer_arch_enable_interrupt(void);

/**
 * Disable interrupts of hardware timers.
 */
void hwtimer_arch_disable_interrupt(void);

/**
 * Set a kernel timer to raise an interrupt after ::offset kernel timer ticks
 * from now.
 */
void hwtimer_arch_set(unsigned long offset, short timer);

/**
 * Set a kernel timer to raise an interrupt at specified system time.
 */
void hwtimer_arch_set_absolute(unsigned long value, short timer);

/**
 * Unset the kernel timer with the given timer ID.
 */
void hwtimer_arch_unset(short timer);

/**
 * Get the current tick count of the default hardware timer.
 */
unsigned long hwtimer_arch_now(void);

/**
 * @}
 */
#endif /* HWTIMER_ARCH_H_ */
