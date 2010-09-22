#ifndef IRQ_H_
#define IRQ_H_

/**
 * @defgroup    irq     IRQ Handling
 * @ingroup     kernel
 *
 * Provides an API to control interrupt processing.
 *
 * @{
 */

/**
 * @file
 * @brief       IRQ driver interface
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @version     $Revision: 1410 $
 *
 * @note        $Id$
 */

#include <stdbool.h>

/**
 * @brief   This function sets the IRQ disable bit in the status register
 * @return  previous value of status register
 */
unsigned disableIRQ(void);

/**
 * @brief   This function clears the IRQ disable bit in the status register
 * @return  previous value of status register
 *
 * @example irq-driver-example.c
 * @see     restoreIRQ
 */
unsigned enableIRQ(void);

/**
 * @brief   This function restores the IRQ disable bit in the status register
 *          to the value contained within passed state
 * @param   state   state to restore
 * @return  previous state
 *
 * @example irq-driver-example.c
 * @see     disableIRQ
 */
void restoreIRQ(unsigned state);

/**
 * @brief   Checks if the mcu is currently processing an interrupt.
 * @return  true, if in interrupt service routine
 */
int inISR(void);

/** @} */ // end of group dev_irq
#endif /* IRQ_H_ */
