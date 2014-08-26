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
 * @file        irq.h
 * @brief       IRQ driver interface
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 */

#ifndef IRQ_H_
#define IRQ_H_

#include <stdbool.h>
#include "arch/irq_arch.h"

/**
 * @brief   This function sets the IRQ disable bit in the status register
 *
 * @note    This function should be used in favour of dINT().
 *
 * @return  Previous value of status register. The return value should not
 *          interpreted as a boolean value. The actual value is only
 *          significant for restoreIRQ().
 *
 * @see     restoreIRQ
 */
unsigned disableIRQ(void);

/**
 * @brief   This function clears the IRQ disable bit in the status register
 *
 * @note    This function should be used in favour of eINT().
 *
 * @return  Previous value of status register. The return value should not
 *          interpreted as a boolean value. The actual value is only
 *          significant for restoreIRQ().
 *
 * @see     restoreIRQ
 */
unsigned enableIRQ(void);

/**
 * @brief   This function restores the IRQ disable bit in the status register
 *          to the value contained within passed state
 *
 * @param[in] state   state to restore
 *
 * @note    This function should be used in favour of eINT().
 *
 * @see     enableIRQ
 * @see     disableIRQ
 */
void restoreIRQ(unsigned state);

/**
 * @brief   Check whether called from interrupt service routine
 * @return  true, if in interrupt service routine, false if not
 */
int inISR(void);

#endif /* IRQ_H_ */
/** @} */
