/*
 * Copyright (C) 2018 Inria
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32l496g-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32L496G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LCD Backlight control defines
 * @{
 */
#define BACKLIGHT_PIN       GPIO_PIN(PORT_I, 0)     /**< Backlight pin */
#define BACKLIGHT_MASK      (1 << 0)                /**< Backlight pin mask */

/** Set the backlight pin */
#define BACKLIGHT_ON        (GPIOI->BSRR = BACKLIGHT_MASK)
/** Clear the backlight pin */
#define BACKLIGHT_OFF       (GPIOI->BSRR = (BACKLIGHT_MASK << 16))
/** Toggle the backlight pin */
#define BACKLIGHT_TOGGLE    (GPIOI->ODR  ^= BACKLIGHT_MASK)
/** @} */

/**
 * @name    LCD display definitions
 * @{
 */
#define LCD_DISP_PIN        GPIO_PIN(PORT_H, 0)     /**< LCD screen enable pin */
#define LCD_SCREEN_WIDTH    240                     /**< LCD screen width */
#define LCD_SCREEN_HEIGHT   240                     /**< LCD screen height */
/** @} */

/**
 * @name    LED pin definitions and handlers
 *
 * The board has 3 user LEDs:
 * - LD1 orange, active HIGH (not directly usable, connected to MFX GPIO4
 * - LD2 green, active HIGH (connected to PB13)
 * - LD3 green, active LOW (connected to PA5)
 * @{
 */
#define LED0_PIN_NUM        13      /**< green LED (LD2) pin */
#define LED0_PORT_NUM       PORT_B  /**< green LED (LD2) port */

#define LED1_PIN_NUM        5       /**< green LED (LD3) pin */
#define LED1_PORT_NUM       PORT_A  /**< green LED (LD3) port */
#define LED1_IS_INVERTED    1       /**< green LED (LD3) is LOW active */
/** @} */

/**
 * @name   Joystick buttons
 *
 * The board has a joystick that is exposed as user buttons.
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)    /**< Center button pin  */
#define BTN0_MODE           GPIO_IN_PD              /**< Center button mode */

#define BTN1_PIN            GPIO_PIN(PORT_I, 9)     /**< Left button pin    */
#define BTN1_MODE           GPIO_IN_PD              /**< Left button mode   */

#define BTN2_PIN            GPIO_PIN(PORT_I, 10)    /**< Down button pin    */
#define BTN2_MODE           GPIO_IN_PD              /**< Down button mode   */

#define BTN3_PIN            GPIO_PIN(PORT_F, 11)    /**< Right button pin   */
#define BTN3_MODE           GPIO_IN_PD              /**< Right button mode  */

#define BTN4_PIN            GPIO_PIN(PORT_I, 8)     /**< Up button pin      */
#define BTN4_MODE           GPIO_IN_PD              /**< Up button mode     */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
