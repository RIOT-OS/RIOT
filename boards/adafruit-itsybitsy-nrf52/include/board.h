/*
 * Copyright (C)  2020 Bruno Chianca <brunobcf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-itsybitsy-nrf52
 * @{
 *
 * @file
 * @brief       Board specific configuration for Adafruit ItsyBitsy nRF52840
 *
 * @author      Bruno Chianca <brunobcf@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

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

/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
/** @brief The button labelled SW1 */
#define BTN0_PIN GPIO_PIN(0, 29)
/** @brief The GPIO pin mode of the button labelled SW1 */
#define BTN0_MODE GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
