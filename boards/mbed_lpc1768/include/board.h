/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_mbed_lpc1768 mbed LPC1768 development kit
 * @ingroup     boards
 * @brief       Support for the mbed LPC1762 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the mbed_lpc1768 board
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "bitarithm.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(1, 18)
#define LED1_PIN            GPIO_PIN(1, 20)
#define LED2_PIN            GPIO_PIN(1, 21)
#define LED3_PIN            GPIO_PIN(1, 23)

#define LED_PORT            (LPC_GPIO1)
#define LED0_MASK           (BIT18)
#define LED1_MASK           (BIT20)
#define LED2_MASK           (BIT21)
#define LED3_MASK           (BIT23)

#define LED0_ON             (LED_PORT->FIOSET  = LED0_MASK)
#define LED0_OFF            (LED_PORT->FIOCLR  = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->FIOPIN ^= LED0_MASK)
#define LED1_ON             (LED_PORT->FIOSET  = LED1_MASK)
#define LED1_OFF            (LED_PORT->FIOCLR  = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->FIOPIN ^= LED1_MASK)
#define LED2_ON             (LED_PORT->FIOSET  = LED2_MASK)
#define LED2_OFF            (LED_PORT->FIOCLR  = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->FIOPIN ^= LED2_MASK)
#define LED3_ON             (LED_PORT->FIOSET  = LED3_MASK)
#define LED3_OFF            (LED_PORT->FIOCLR  = LED3_MASK)
#define LED3_TOGGLE         (LED_PORT->FIOPIN ^= LED3_MASK)
/** @} */

/**
 * @brief Initialize board specific hardware, include clocks, LEDs and stdio
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
