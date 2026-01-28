/*
 * SPDX-FileCopyrightText: 2025 Tom Hert git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           GPIO configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

/** The number of GPIO pins available on the RP2350 */
#define GPIO_PIN_NUMOF 30u

#ifdef __cplusplus
extern "C" {
#endif

#define HAVE_GPIO_MODE_T

typedef enum {
    GPIO_IN ,               /**< configure as input without pull resistor */
    GPIO_IN_PD,             /**< configure as input with pull-down resistor */
    GPIO_IN_PU,             /**< configure as input with pull-up resistor */
    GPIO_OUT,               /**< configure as output in push-pull mode */
    GPIO_OD,                /**< configure as output in open-drain mode without
                             *   pull resistor */
    GPIO_OD_PU,              /**< configure as output in open-drain mode with
                             *   pull resistor enabled */
    GPIO_PIO0,               /**< configure pin for PIO usage */
    GPIO_PIO1
} gpio_mode_t;

/**
 * @brief   Possible function values for @ref gpio_io_ctrl_t::function_select
 */
typedef enum {
    /** connect pin to the SPI peripheral (MISO/MOSI/SCK depends on pin) */
    FUNCTION_SELECT_SPI = 1,

    /** connect pin to the UART peripheral (TXD/RXD depends on pin) */
    FUNCTION_SELECT_UART = 2,

    /** connect pin to the I2C peripheral (SCL/SDA depends on pin) */
    FUNCTION_SELECT_I2C = 3,

    /** connect pin to the timer for PWM (channel depends on pin) */
    FUNCTION_SELECT_PWM = 4,

    /** use pin as vanilla GPIO */
    FUNCTION_SELECT_SIO = 5,

    /** connect pin to the first PIO peripheral */
    FUNCTION_SELECT_PIO0 = 6,

    /** connect pin to the second PIO peripheral */
    FUNCTION_SELECT_PIO1 = 7,

    /** connect pin to the timer (depending on pin: external clock,
    *   clock output, or not supported) */
    FUNCTION_SELECT_CLOCK = 8,

    /** connect pin to the USB peripheral (function depends on pin) */
    FUNCTION_SELECT_USB = 9,

    /** Reset value, pin unconnected */
    FUNCTION_SELECT_NONE = 31,
} gpio_function_select_t;

#ifdef __cplusplus
}
#endif

/** @} */
