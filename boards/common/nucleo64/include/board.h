/*
 * SPDX-FileCopyrightText: 2016-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_nucleo64 STM32 Nucleo-64
 * @ingroup     boards
 * @brief       Support for STM32 Nucleo-64 boards
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "board_nucleo.h"
#include "arduino_pinmap.h"
#include "motor_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#if defined(CPU_MODEL_STM32F302R8) || defined(CPU_MODEL_STM32L433RC)
#define LED0_PIN_NUM        13
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B
#else
#define LED0_PIN_NUM        5
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A
#endif
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)
#if defined(CPU_MODEL_STM32L433RC) || defined(CPU_MODEL_STM32G474RE) || \
    defined(CPU_MODEL_STM32G431RB)
#define BTN0_MODE           GPIO_IN_PD
#else
#define BTN0_MODE           GPIO_IN_PU
#endif
/** @} */

/**
 * @name Describe DC motors with PWM channel and GPIOs
 * @{
 */
static const motor_driver_config_t motor_driver_config[] = {
    {
        .pwm_dev         = 1,
        .mode            = MOTOR_DRIVER_1_DIR,
        .mode_brake      = MOTOR_BRAKE_HIGH,
        .pwm_mode        = PWM_LEFT,
        .pwm_frequency   = 20000U,
        .pwm_resolution  = 2250U,
        .nb_motors       = 1,
        .motors          = {
            {
                .pwm_channel            = 0,
                .gpio_enable            = 0,
                .gpio_dir0              = ARDUINO_PIN_15,
                .gpio_dir1_or_brake     = 0,
                .gpio_dir_reverse       = 0,
                .gpio_enable_invert     = 0,
                .gpio_brake_invert      = 0,
            },
        },
        .cb = NULL,
    },
};

#define MOTOR_DRIVER_NUMOF           ARRAY_SIZE(motor_driver_config)
/** @} */

/**
 * @name Describe MRF24J40 radio
 * @{
 */
#ifndef MRF24J40_PARAM_SPI
#define MRF24J40_PARAM_SPI      SPI_DEV(0)
#endif

#ifndef MRF24J40_PARAM_SPI_CLK
#define MRF24J40_PARAM_SPI_CLK  SPI_CLK_5MHZ
#endif

#ifndef MRF24J40_PARAM_CS
#define MRF24J40_PARAM_CS       ARDUINO_PIN_10
#endif

#ifndef MRF24J40_PARAM_INT
#define MRF24J40_PARAM_INT      ARDUINO_PIN_7
#endif

#ifndef MRF24J40_PARAM_RESET
#define MRF24J40_PARAM_RESET    ARDUINO_PIN_5
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
