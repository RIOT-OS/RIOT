/*
 * SPDX-FileCopyrightText: 2019 Christian Amsüss <chrysn@fsfe.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf52840dongle
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF52840-Dongle
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
 /** @brief The LED labelled LD1 */
#define LED0_PIN            GPIO_PIN(0, 6)

 /** @brief The red channel of the LED labelled LD2 */
#define LED1_PIN            GPIO_PIN(0, 8)
 /** @brief The green channel of the LED labelled LD2 */
#define LED2_PIN            GPIO_PIN(1, 9)
 /** @brief The blue channel of the LED labelled LD2 */
#define LED3_PIN            GPIO_PIN(0, 12)

/** @} */

/**
 * @name    LED access macros
 * @{
 */

/** Enable LD1 */
#define LED0_ON gpio_clear(LED0_PIN)
/** Disable LD1 */
#define LED0_OFF gpio_set(LED0_PIN)
/** Toggle LD1 */
#define LED0_TOGGLE gpio_toggle(LED0_PIN)

/** Enable LD2's red channel */
#define LED1_ON gpio_clear(LED1_PIN)
/** Disable LD2's red channel */
#define LED1_OFF gpio_set(LED1_PIN)
/** Toggle LD2's red channel */
#define LED1_TOGGLE gpio_toggle(LED1_PIN)

/** Enable LD2's green channel */
#define LED2_ON gpio_clear(LED2_PIN)
/** Disable LD2's green channel */
#define LED2_OFF gpio_set(LED2_PIN)
/** Toggle LD2's green channel */
#define LED2_TOGGLE gpio_toggle(LED2_PIN)

/** Enable LD2's blue channel */
#define LED3_ON gpio_clear(LED3_PIN)
/** Disable LD2's blue channel */
#define LED3_OFF gpio_set(LED3_PIN)
/** Toggle LD2's blue channel */
#define LED3_TOGGLE gpio_toggle(LED3_PIN)

/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
/** @brief The button labelled SW1 */
#define BTN0_PIN            GPIO_PIN(1, 6)
/** @brief The GPIO pin mode of the button labelled SW1 */
#define BTN0_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
