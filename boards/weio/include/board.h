/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_weio WeIO development kit
 * @ingroup     boards
 * @brief       Support for the WeIO board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the WeIO board
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
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
#define LED0_PIN            GPIO_PIN(0, 13)
#define LED1_PIN            GPIO_PIN(0, 14)
#define LED2_PIN            GPIO_PIN(0, 15)

#define LED_PORT            LPC_GPIO
#define LED0_MASK           BIT13
#define LED1_MASK           BIT14
#define LED2_MASK           BIT15

#define LED0_ON             (LED_PORT->CLR[1] = LED0_MASK)
#define LED0_OFF            (LED_PORT->SET[1] = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->NOT[1] = LED0_MASK)

#define LED1_ON             (LED_PORT->CLR[1] = LED1_MASK)
#define LED1_OFF            (LED_PORT->SET[1] = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->NOT[1] = LED1_MASK)

#define LED2_ON             (LED_PORT->CLR[1] = LED2_MASK)
#define LED2_OFF            (LED_PORT->SET[1] = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->NOT[1] = LED2_MASK)
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
