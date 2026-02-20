/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_pro-micro-nrf52840
 * @{
 *
 * @file
 * @brief       Board specific configuration for Pro Micro nRF52840-compatible
 *              development boards
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_ADJUST_SET    7      /**< Overhead for the ztimer_set function in us */
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP  21     /**< Overhead for the ztimer_sleep function in us */
/** @} */

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 15)     /**< Pin Definition for the LED */

#define LED_PORT            (NRF_P0)            /**< GPIO Port of the LED GPIOs */
#define LED0_MASK           (1 << 15)           /**< Bit position of the LED GPIO output bit */

#define LED0_ON             (LED_PORT->OUTSET = LED0_MASK)      /**< Turn the LED on */
#define LED0_OFF            (LED_PORT->OUTCLR = LED0_MASK)      /**< Turn the LED off */
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)      /**< Toggle the LED */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
