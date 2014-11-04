/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#include "adc_legacy.h"

#define ADC_NUM                  (6)
#define ADC_OFFSET            (0x10)
#define ADC_INDEX                (4)

#define ADC_DONE        (0x80000000)
#define ADC_OVERRUN     (0x40000000)

/**
 * @brief   Initialize ADC.
 */
void adc_init_1(void);
void adc_init_2(void);

/** @} */
#endif /* LPC2387ADC_H_ */
