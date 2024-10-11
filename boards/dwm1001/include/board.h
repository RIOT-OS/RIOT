/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_dwm1001
 * @{
 *
 * @file
 * @brief       Board specific configuration for the DWM1001 dev board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 30)  /**< Green LED on D9 */
#define LED1_PIN            GPIO_PIN(0, 14)  /**< Red LED on D12 */
#define LED2_PIN            GPIO_PIN(0, 22)  /**< Red LED on D11 */
#define LED3_PIN            GPIO_PIN(0, 31)  /**< Blue LED on D10 */

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 30)
#define LED1_MASK           (1 << 14)
#define LED2_MASK           (1 << 22)
#define LED3_MASK           (1 << 31)
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
#define BTN0_PIN            GPIO_PIN(0, 2)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    LIS2DH12 driver configuration
 * @{
 */
#define LIS2DH12_PARAM_INT_PIN1     GPIO_PIN(0, 25)
/** @} */

/**
 * @name    DW1000 UWB transceiver
 * @{
 */
#define DW1000_PARAM_SPI            SPI_DEV(1)
#define DW1000_PARAM_CS_PIN         GPIO_PIN(0, 17)
#define DW1000_PARAM_INT_PIN        GPIO_PIN(0, 19)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
