/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf9160dk
 * @{
 *
 * @file
 * @brief       Board configuration for the nrf9160dk board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_HFCLK         (32U)           /* set to  0: internal RC oscillator
                                             *        32: 32MHz crystal */
#define CLOCK_LFCLK         (3)             /* set to  0: internal RC oscillator
                                             *         3: High Accuracy oscillator */

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 2)
#define LED1_PIN            GPIO_PIN(0, 3)
#define LED2_PIN            GPIO_PIN(0, 4)
#define LED3_PIN            GPIO_PIN(0, 5)

#define LED_PORT            (NRF_P0_S)
#define LED0_MASK           (1 << 2)
#define LED1_MASK           (1 << 3)
#define LED2_MASK           (1 << 4)
#define LED3_MASK           (1 << 5)

#define LED_MASK            (LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK)

#define LED3_ON             (LED_PORT->OUTCLR = LED3_MASK)
#define LED3_OFF            (LED_PORT->OUTSET = LED3_MASK)
#define LED3_TOGGLE         (LED_PORT->OUT   ^= LED3_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 6)
#define BTN0_MODE           GPIO_IN
#define BTN1_PIN            GPIO_PIN(0, 7)
#define BTN1_MODE           GPIO_IN
#define BTN2_PIN            GPIO_PIN(0, 8)
#define BTN2_MODE           GPIO_IN
#define BTN3_PIN            GPIO_PIN(0, 9)
#define BTN3_MODE           GPIO_IN
/** @} */

/**
 * @brief   Initialize the platform
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
