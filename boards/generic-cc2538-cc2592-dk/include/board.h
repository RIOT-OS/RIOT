/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_generic_cc2538_cc2592_dk
 * @{
 *
 * @file
 * @brief       Definitions for the Generic CC2538-CC2592-DK Board
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"
#include "cc2538_eui_primary.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_GREEN_PIN       GPIO_PIN(PORT_C, 0) /**< GPIO pin for green LED */
#define LED_GREEN_PORT      GPIO_C              /**< GPIO port register to used to control green LED */
#define LED_GREEN_BIT       (1U << 0)           /**< Bitmask to write to @ref LED_GREEN_PORT */
#define LED0_PIN            LED_GREEN_PIN       /**< Alias for LED_GREEN_PIN */
#define LED0_PORT           LED_GREEN_PORT      /**< Alias for LED_GREEN_PORT */
#define LED0_BIT            LED_GREEN_BIT       /**< Alias for LED_GREEN_BIT */

#define LED_RED_PIN         GPIO_PIN(PORT_C, 1) /**< GPIO pin for red LED */
#define LED_RED_PORT        GPIO_C              /**< GPIO port register to used to control red LED */
#define LED_RED_BIT         (1U << 1)           /**< Bitmask to write to @ref LED_RED_PORT */
#define LED1_PIN            LED_RED_PIN         /**< Alias for LED_RED_PIN */
#define LED1_PORT           LED_RED_PORT        /**< Alias for LED_RED_PORT */
#define LED1_BIT            LED_RED_BIT         /**< Alias for LED_RED_BIT */

#define LED_YELLOW_PIN      GPIO_PIN(PORT_B, 1) /**< GPIO pin for yellow LED */
#define LED_YELLOW_PORT     GPIO_B              /**< GPIO port register to used to control yellow LED */
#define LED_YELLOW_BIT      (1U << 1)           /**< Bitmask to write to @ref LED_YELLOW_PORT */
#define LED2_PIN            LED_YELLOW_PIN      /**< Alias for LED_YELLOW_PIN */
#define LED2_PORT           LED_YELLOW_PORT     /**< Alias for LED_YELLOW_PORT */
#define LED2_BIT            LED_YELLOW_BIT      /**< Alias for LED_YELLOW_BIT */

#define LED_BLUE_PIN        GPIO_PIN(PORT_B, 0) /**< GPIO pin for blue LED */
#define LED_BLUE_PORT       GPIO_B              /**< GPIO port register to used to control blue LED */
#define LED_BLUE_BIT        (1U << 0)           /**< Bitmask to write to @ref LED_BLUE_PORT */
#define LED3_PIN            LED_BLUE_PIN        /**< Alias for LED_BLUE_PIN */
#define LED3_PORT           LED_BLUE_PORT       /**< Alias for LED_BLUE_PORT */
#define LED3_BIT            LED_BLUE_BIT        /**< Alias for LED_BLUE_BIT */

#define LED0_ON             (LED0_PORT->DATA |=  LED0_BIT)
#define LED0_OFF            (LED0_PORT->DATA &= ~LED0_BIT)
#define LED0_TOGGLE         (LED0_PORT->DATA ^=  LED0_BIT)

#define LED1_ON             (LED1_PORT->DATA |=  LED1_BIT)
#define LED1_OFF            (LED1_PORT->DATA &= ~LED1_BIT)
#define LED1_TOGGLE         (LED1_PORT->DATA ^=  LED1_BIT)

#define LED2_ON             (LED2_PORT->DATA |=  LED2_BIT)
#define LED2_OFF            (LED2_PORT->DATA &= ~LED2_BIT)
#define LED2_TOGGLE         (LED2_PORT->DATA ^=  LED2_BIT)

#define LED3_ON             (LED3_PORT->DATA |=  LED3_BIT)
#define LED3_OFF            (LED3_PORT->DATA &= ~LED3_BIT)
#define LED3_TOGGLE         (LED3_PORT->DATA ^=  LED3_BIT)
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN_LEFT_PIN        GPIO_PIN(PORT_C, 4) /**< GPIO pin for button "LEFT" */
#define BTN_LEFT_MODE       GPIO_IN_PU          /**< GPIO mode for button "LEFT" */
#define BTN0_PIN            BTN_LEFT_PIN        /**< Alias for BTN_LEFT_PIN */
#define BTN0_MODE           BTN_LEFT_MODE       /**< Alias for BTN_LEFT_MODE */

#define BTN_RIGHT_PIN       GPIO_PIN(PORT_C, 5) /**< GPIO pin for button "RIGHT" */
#define BTN_RIGHT_MODE      GPIO_IN_PU          /**< GPIO mode for button "RIGHT" */
#define BTN1_PIN            BTN_RIGHT_PIN       /**< Alias for BTN_RIGHT_PIN */
#define BTN1_MODE           BTN_RIGHT_MODE      /**< Alias for BTN_RIGHT_MODE */

#define BTN_UP_PIN          GPIO_PIN(PORT_C, 6) /**< GPIO pin for button "UP" */
#define BTN_UP_MODE         GPIO_IN_PU          /**< GPIO mode for button "UP" */
#define BTN2_PIN            BTN_UP_PIN          /**< Alias for BTN_UP_PIN */
#define BTN2_MODE           BTN_UP_MODE         /**< Alias for BTN_UP_MODE */

#define BTN_DOWN_PIN        GPIO_PIN(PORT_C, 7) /**< GPIO pin for button "DOWN" */
#define BTN_DOWN_MODE       GPIO_IN_PU          /**< GPIO mode for button "DOWN" */
#define BTN3_PIN            BTN_DOWN_PIN        /**< Alias for BTN_DOWN_PIN */
#define BTN3_MODE           BTN_DOWN_MODE       /**< Alias for BTN_DOWN_MODE */

#define BTN_SELECT_PIN      GPIO_PIN(PORT_A, 3) /**< GPIO pin for button "SELECT" */
#define BTN_SELECT_MODE     GPIO_IN_PU          /**< GPIO mode for button "SELECT" */
#define BTN4_PIN            BTN_SELECT_PIN      /**< Alias for BTN_SELECT_PIN */
#define BTN4_MODE           BTN_SELECT_MODE     /**< Alias for BTN_SELECT_MODE */
/** @} */

/**
 * @name    Flash Customer Configuration Area (CCA) parameters
 * @{
 */
#ifndef UPDATE_CCA
#define UPDATE_CCA                1
#endif

#define CCA_BACKDOOR_ENABLE       1
#define CCA_BACKDOOR_PORT_A_PIN   3 /**< Select button */
#define CCA_BACKDOOR_ACTIVE_LEVEL 0 /**< Active low */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
