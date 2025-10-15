/*
 * SPDX-FileCopyrightText: 2025 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_motor
 * @{
 *
 * @file        board.c
 * @brief       Example configurations for the Nucleo-F446RE and Native
 *
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <container.h>

#include "motor_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BOARD_NUCLEO_F446RE)

#  include "board_nucleo.h"
#  include "arduino_pinmap.h"

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

#elif defined(BOARD_NATIVE) || defined(BOARD_NATIVE32) || defined(BOARD_NATIVE64)

#  if MODULE_PERIPH_QDEC
/**
 * @brief Simulate QDEC on motor_set() calls
 *
 * @param[in] motor_driver      motor driver to which motor is attached
 * @param[in] motor_id          motor ID on driver
 * @param[in] pwm_duty_cycle    Signed PWM duty_cycle to set motor speed and direction
 */
void native_motor_driver_qdec_simulation( \
    const motor_driver_config_t *motor_driver_conf, uint8_t motor_id, \
    int32_t pwm_duty_cycle);

/* C++ standard do not support designated initializers */
#    if !(defined __cplusplus)

/**
 * @name Describe DC motors with PWM channel and GPIOs
 * @{
 */
static const motor_driver_config_t motor_driver_config[] = {
    {
        .pwm_dev         = 0,
        .mode            = MOTOR_DRIVER_1_DIR_BRAKE,
        .mode_brake      = MOTOR_BRAKE_LOW,
        .pwm_mode        = PWM_LEFT,
        .pwm_frequency   = 20000U,
        .pwm_resolution  = 1000U,
        .nb_motors       = 2,
        .motors          = {
            {
                .pwm_channel            = 0,
                .gpio_enable            = GPIO_PIN(0, 0),
                .gpio_dir0              = GPIO_PIN(0, 0),
                .gpio_dir1_or_brake     = GPIO_PIN(0, 0),
                .gpio_dir_reverse       = 0,
                .gpio_enable_invert     = 0,
                .gpio_brake_invert      = 0,
            },
            {
                .pwm_channel            = 1,
                .gpio_enable            = GPIO_PIN(0, 0),
                .gpio_dir0              = GPIO_PIN(0, 0),
                .gpio_dir1_or_brake     = GPIO_PIN(0, 0),
                .gpio_dir_reverse       = 1,
                .gpio_enable_invert     = 0,
                .gpio_brake_invert      = 0,
            },
        },
        .cb = native_motor_driver_qdec_simulation,
    },
};

#    define MOTOR_DRIVER_NUMOF       ARRAY_SIZE(motor_driver_config)
/** @} */
#    endif

#  endif /* __cplusplus */

#endif

#ifdef __cplusplus
}
#endif

/** @} */
