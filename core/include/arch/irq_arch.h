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
 * @file        irq_arch.h
 * @brief       Interrupt handling interface for globally en- and disabling interrupts
 *
 * This file acts as a wrapper between the kernels interrupt interface and the architecture
 * dependent implementation of the interfaces.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __IRQ_ARCH_H
#define __IRQ_ARCH_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Define mapping between kernel internal and arch interfaces
 *
 * This mapping is done for compatibility of existing platforms,
 * new platforms should always use the *_arch_* interfaces.
 * @{
 */
#ifdef COREIF_NG
#define enableIRQ       irq_arch_enable
#define disableIRQ      irq_arch_disable
#define restoreIRQ      irq_arch_restore
#define inISR           irq_arch_in
#endif
/** @} */

/**
 * @brief Globally enable maskable interrupt sources
 *
 * @return              the IRQ state after enabling interrupts
 */
unsigned int irq_arch_enable(void);

/**
 * @brief Globally disable all maskable interrupt sources
 *
 * @return              the IRQ state before disabling interrupts
 */
unsigned int irq_arch_disable(void);


/**
 * @brief Restore a previously recorded IRQ state
 *
 * @param[in] state     the state to set the IRQ flags to
 */
void irq_arch_restore(unsigned int state);

/**
 * @brief See if the current context is inside an ISR
 *
 * @return              1 if currently in interrupt context, 0 otherwise
 */
int irq_arch_in(void);

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_ARCH_H */
/** @} */
