/*
 * SPDX-FileCopyrightText: 2025 COGIP Robotics association
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_bitcraze_crazyflie21_main
 * @{
 *
 * @file
 * @brief       Configuration for motor driver on Crazyflie 2.1
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 */

#include "board.h"
#include "motor_driver.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Motor driver configuration for Crazyflie 2.1
 * @{
 */

/**
 * @brief   Motor driver 0 parameters (3 motors on PWM device 1)
 */
#define MOTOR_DRIVER_0_PARAMS \
    { \
        .mode            = MOTOR_DRIVER_1_DIR, \
        .pwm_dev         = 1, \
        .pwm_mode        = PWM_LEFT, \
        .pwm_frequency   = 20000U, \
        .pwm_resolution  = 4200U, \
        .brake_inverted  = true, \
        .enable_inverted = false, \
        .nb_motors       = 3, \
        .motors          = { \
            { \
                .pwm_channel      = 1, \
                .gpio_enable      = GPIO_UNDEF, \
                .gpio_dir0        = GPIO_UNDEF, \
                .gpio_dir1        = GPIO_UNDEF, \
                .gpio_dir_reverse = GPIO_UNDEF, \
            }, \
            { \
                .pwm_channel      = 3, \
                .gpio_enable      = GPIO_UNDEF, \
                .gpio_dir0        = GPIO_UNDEF, \
                .gpio_dir1        = GPIO_UNDEF, \
                .gpio_dir_reverse = GPIO_UNDEF, \
            }, \
            { \
                .pwm_channel      = 0, \
                .gpio_enable      = GPIO_UNDEF, \
                .gpio_dir0        = GPIO_UNDEF, \
                .gpio_dir1        = GPIO_UNDEF, \
                .gpio_dir_reverse = GPIO_UNDEF, \
            } \
        }, \
        .motor_set_post_cb = NULL \
    }

/**
 * @brief   Motor driver 1 parameters (1 motor on PWM device 2)
 */
#define MOTOR_DRIVER_1_PARAMS \
    { \
        .mode            = MOTOR_DRIVER_1_DIR, \
        .pwm_dev         = 2, \
        .pwm_mode        = PWM_LEFT, \
        .pwm_frequency   = 20000U, \
        .pwm_resolution  = 4200U, \
        .brake_inverted  = true, \
        .enable_inverted = false, \
        .nb_motors       = 1, \
        .motors          = { \
            { \
                .pwm_channel      = 0, \
                .gpio_enable      = GPIO_UNDEF, \
                .gpio_dir0        = GPIO_UNDEF, \
                .gpio_dir1        = GPIO_UNDEF, \
                .gpio_dir_reverse = GPIO_UNDEF, \
            } \
        }, \
        .motor_set_post_cb = NULL \
    }

#ifndef MOTOR_DRIVER_PARAMS
/**
 * @brief   Motor driver configuration array
 */
#  define MOTOR_DRIVER_PARAMS \
    MOTOR_DRIVER_0_PARAMS, \
    MOTOR_DRIVER_1_PARAMS
#endif

#ifndef MOTOR_DRIVER_SAUL_INFO
/**
 * @brief   SAUL registry information for motor drivers
 */
#  define MOTOR_DRIVER_SAUL_INFO \
    { .name = "motor_driver_0" }, \
    { .name = "motor_driver_1" }
#endif
/**@}*/

/**
 * @brief   MOTOR_DRIVER configuration
 */
static const motor_driver_params_t motor_driver_params[] =
{
    MOTOR_DRIVER_PARAMS,
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t motor_driver_saul_info[] =
{
    MOTOR_DRIVER_SAUL_INFO
};

#ifdef __cplusplus
}
#endif
/** @} */
