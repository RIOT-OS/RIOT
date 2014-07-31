/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This file is subject to the terms and conditions of the LGPLv2.
See the file LICENSE in the top level directory for more details.
*******************************************************************************/

#ifndef LPC2387ADC_H_
#define LPC2387ADC_H_

/**
 * @defgroup    lpc2387_adc     LPC2387 ADC
 * @ingroup     lpc2387
 *
 * @{
 */

/**
 * @file
 * @brief       LPC2387 ADC
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3249 $
 *
 * @note        $Id: lpc2387-adc.h 3249 2011-03-11 09:44:46Z schmittb $
 */

#include <stdint.h>

#define ADC_NUM                  (6)
#define ADC_OFFSET            (0x10)
#define ADC_INDEX                (4)

#define ADC_DONE        (0x80000000)
#define ADC_OVERRUN     (0x40000000)

/**
 * @brief   Initialize ADC.
 */
void adc_init(void);
void adc_init_1(void);
void adc_init_2(void);

/**
 * @brief   Read ADC value of selected channel.
 *
 * Valid channel numbers are from 0 to 2.
 *
 * @return  ADC value of selected channel.
 */
uint16_t adc_read(uint8_t channel);

/** @} */
#endif /* LPC2387ADC_H_ */
