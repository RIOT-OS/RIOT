/*
 * Copyright (C) 2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef BOARD_H
#define BOARD_H

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
#define LED0_PORT           GPIOB
#define LED0_PIN            GPIO_PIN(PORT_B, 13)
#define LED0_MASK           (1 << 13)
#else
#define LED0_PORT           GPIOA
#define LED0_PIN            GPIO_PIN(PORT_A, 5)
#define LED0_MASK           (1 << 5)
#endif

#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED0_PORT->ODR  ^= LED0_MASK)
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

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
