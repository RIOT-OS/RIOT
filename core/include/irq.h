/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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

/**
 * @brief   This function sets the IRQ disable bit in the status register
 *
 * @note    This function should be used in favour of dINT().
 *
 * @return  previous value of status register
 */
unsigned disableIRQ(void);

/**
 * @brief   This function clears the IRQ disable bit in the status register
 * @return  previous value of status register
 *
 * @see     restoreIRQ
 */
unsigned enableIRQ(void);

/**
 * @brief   This function restores the IRQ disable bit in the status register
 *          to the value contained within passed state
 * @param   state   state to restore
 *
 * @note    This function should be used in favour of eINT().
 *
 * @see     disableIRQ
 */
void restoreIRQ(unsigned state);

/**
 * @brief   Checks if the mcu is currently processing an interrupt.
 * @return  true, if in interrupt service routine
 */
int inISR(void);

/** @} */
#endif /* IRQ_H_ */
