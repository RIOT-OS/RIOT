/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf52840-mdk-dongle
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF52840 MDK USB Dongle
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name USB configuration
 * @{
 */
#define INTERNAL_PERIPHERAL_VID         (0x239a)
#define INTERNAL_PERIPHERAL_PID         (0x0029)
/** @} */

/**
 * @name    LED pin configuration
 * @{
 */
 /** @brief The red channel of the LED */
#define LED0_PIN            GPIO_PIN(0, 23)
 /** @brief The green channel of the LED */
#define LED1_PIN            GPIO_PIN(0, 22)
 /** @brief The blue channel of the LED */
#define LED2_PIN            GPIO_PIN(0, 24)

/** @} */

/**
 * @name    LED access macros
 * @{
 */

/** Enable LED's red channel */
#define LED0_ON gpio_clear(LED0_PIN)
/** Disable LED's red channel */
#define LED0_OFF gpio_set(LED0_PIN)
/** Toggle LED's red channel */
#define LED0_TOGGLE gpio_toggle(LED0_PIN)

/** Enable LED's green channel */
#define LED1_ON gpio_clear(LED1_PIN)
/** Disable LED's green channel */
#define LED1_OFF gpio_set(LED1_PIN)
/** Toggle LED's green channel */
#define LED1_TOGGLE gpio_toggle(LED1_PIN)

/** Enable LED's blue channel */
#define LED2_ON gpio_clear(LED2_PIN)
/** Disable LED's blue channel */
#define LED2_OFF gpio_set(LED2_PIN)
/** Toggle LED's blue channel */
#define LED2_TOGGLE gpio_toggle(LED2_PIN)

/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
/** @brief The button labelled RESET */
#define BTN0_PIN            GPIO_PIN(0, 18)
/** @brief The GPIO pin mode of the button labelled RESET */
#define BTN0_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
