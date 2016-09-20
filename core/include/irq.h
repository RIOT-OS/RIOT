/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_irq     IRQ Handling
 * @ingroup     core
 * @brief       Provides an API to control interrupt processing
 * @{
 *
 * @file
 * @brief       IRQ driver interface
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 */

#ifndef IRQ_H
#define IRQ_H

#include <stdbool.h>
#include "arch/irq_arch.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   This function sets the IRQ disable bit in the status register
 *
 * @return  Previous value of status register. The return value should not
 *          interpreted as a boolean value. The actual value is only
 *          significant for irq_restore().
 *
 * @see     irq_restore
 */
unsigned irq_disable(void);

/**
 * @brief   This function clears the IRQ disable bit in the status register
 *
 * @return  Previous value of status register. The return value should not
 *          interpreted as a boolean value. The actual value is only
 *          significant for irq_restore().
 *
 * @see     irq_restore
 */
unsigned irq_enable(void);

/**
 * @brief   This function restores the IRQ disable bit in the status register
 *          to the value contained within passed state
 *
 * @param[in] state   state to restore
 *
 * @see     irq_enable
 * @see     irq_disable
 */
void irq_restore(unsigned state);

/**
 * @brief   Check whether called from interrupt service routine
 * @return  true, if in interrupt service routine, false if not
 */
int irq_is_in(void);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_H */
/** @} */
