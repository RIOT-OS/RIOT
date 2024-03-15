/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_e104-bt50xxa-tb
 * @{
 *
 * @file
 * @brief       Board specific configuration for the E104-BT50xxA Test Board
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
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
#define LED0_PIN            GPIO_PIN(0, 30)
#define LED1_PIN            GPIO_PIN(0, 31)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 30)
#define LED1_MASK           (1 << 31)
#define LED_MASK            (LED0_MASK | LED1_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 21)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(0, 29)
#define BTN1_MODE           GPIO_IN_PU

#define BTN_RST_PIN         BTN0_PIN    /**< reset button */
#define BTN_RST_MODE        BTN0_MODE   /**< reset button mode */
/** @} */

/**
 * @name    WS281x RGB LED configuration
 * @{
 */
#define WS281X_TIMER_DEV    TIMER_DEV(1)            /**< Timer device */
#define WS281X_TIMER_MAX_VALUE TIMER_1_MAX_VALUE    /**< Timer max value */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
