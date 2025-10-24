/*
 * SPDX-FileCopyrightText: 2023 COGIP Robotics association
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_motor_driver
 * @{
 *
 * @file
 * @brief       Default configuration for motor driver.
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 *
 */

#include "board.h"
#include "motor_driver.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for motor_driver
 * @{
 */

#ifndef MOTOR_DRIVER_PARAM_MODE
/** Default motor driver mode */
#  define MOTOR_DRIVER_PARAM_MODE               MOTOR_DRIVER_1_DIR
#endif
#ifndef MOTOR_DRIVER_PARAM_BRAKE_INVERTED
/** Default brake level */
#  define MOTOR_DRIVER_PARAM_BRAKE_INVERTED     false
#endif
#ifndef MOTOR_DRIVER_PARAM_ENABLE_INVERTED
/** Default enable level */
#  define MOTOR_DRIVER_PARAM_ENABLE_INVERTED    false
#endif
#ifndef MOTOR_DRIVER_PARAM_PWM
/** Default PWM device */
#  define MOTOR_DRIVER_PARAM_PWM                0
#endif
#ifndef MOTOR_DRIVER_PARAM_PWM_MODE
/** Default PWM mode */
#  define MOTOR_DRIVER_PARAM_PWM_MODE           PWM_LEFT
#endif
#ifndef MOTOR_DRIVER_PARAM_PWM_FREQUENCY
/** Default PWM frequency */
#  define MOTOR_DRIVER_PARAM_PWM_FREQUENCY      20000U
#endif
#ifndef MOTOR_DRIVER_PARAM_PWM_RESOLUTION
/** Default PWM resolution */
#  define MOTOR_DRIVER_PARAM_PWM_RESOLUTION     100U
#endif
#ifndef MOTOR_DRIVER_PARAM_NB_MOTORS
/** Default number of motors */
#  define MOTOR_DRIVER_PARAM_NB_MOTORS          2U
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR_SET_POST_CALLBACK
/** Default callback called at end of motor_set() */
#  define MOTOR_DRIVER_PARAM_MOTOR_SET_POST_CALLBACK    NULL
#endif

/* Motor 1 */
#ifndef MOTOR_DRIVER_PARAM_MOTOR1_PWM_CHANNEL
/** Default motor 1 PWM channel */
#  define MOTOR_DRIVER_PARAM_MOTOR1_PWM_CHANNEL         1U
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR1_GPIO_ENABLE
/** Default motor 1 enable GPIO */
#  define MOTOR_DRIVER_PARAM_MOTOR1_GPIO_ENABLE         GPIO_UNDEF
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR0
/** Default motor 1 direction GPIO */
#  define MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR0           GPIO_UNDEF
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR1_OR_BRAKE
/** Default motor 1 direction or brake GPIO */
#  define MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR1_OR_BRAKE  GPIO_UNDEF
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR_REVERSE
/** Default motor 1 direction GPIO(s) reverse */
#  define MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR_REVERSE    0
#endif

/* Motor 2 */
#ifndef MOTOR_DRIVER_PARAM_MOTOR2_PWM_CHANNEL
/** Default motor 2 PWM channel */
#  define MOTOR_DRIVER_PARAM_MOTOR2_PWM_CHANNEL         2U
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR2_GPIO_ENABLE
/** Default motor 2 enable GPIO */
#  define MOTOR_DRIVER_PARAM_MOTOR2_GPIO_ENABLE         GPIO_UNDEF
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR0
/** Default motor 2 direction GPIO */
#  define MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR0           GPIO_UNDEF
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR1_OR_BRAKE
/** Default motor 2 direction or brake GPIO */
#  define MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR1_OR_BRAKE  GPIO_UNDEF
#endif
#ifndef MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR_REVERSE
/** Default motor 2 direction GPIO(s) reverse */
#  define MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR_REVERSE    0
#endif

#ifndef MOTOR_DRIVER_PARAMS
/** Default motor driver parameters */
#  define MOTOR_DRIVER_PARAMS \
    { \
        .mode            = MOTOR_DRIVER_PARAM_MODE, \
        .pwm_dev         = MOTOR_DRIVER_PARAM_PWM, \
        .pwm_mode        = MOTOR_DRIVER_PARAM_PWM_MODE, \
        .pwm_frequency   = MOTOR_DRIVER_PARAM_PWM_FREQUENCY, \
        .pwm_resolution  = MOTOR_DRIVER_PARAM_PWM_RESOLUTION, \
        .brake_inverted  = MOTOR_DRIVER_PARAM_BRAKE_INVERTED, \
        .enable_inverted = MOTOR_DRIVER_PARAM_ENABLE_INVERTED, \
        .nb_motors       = MOTOR_DRIVER_PARAM_NB_MOTORS, \
        .motors          = { \
            { \
                .pwm_channel            = MOTOR_DRIVER_PARAM_MOTOR1_PWM_CHANNEL, \
                .gpio_enable            = MOTOR_DRIVER_PARAM_MOTOR1_GPIO_ENABLE, \
                .gpio_dir0              = MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR0, \
                .gpio_dir1              = MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR1_OR_BRAKE, \
                .gpio_dir_reverse       = MOTOR_DRIVER_PARAM_MOTOR1_GPIO_DIR_REVERSE, \
            }, \
            { \
                .pwm_channel            = MOTOR_DRIVER_PARAM_MOTOR2_PWM_CHANNEL, \
                .gpio_enable            = MOTOR_DRIVER_PARAM_MOTOR2_GPIO_ENABLE, \
                .gpio_dir0              = MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR0, \
                .gpio_dir1              = MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR1_OR_BRAKE, \
                .gpio_dir_reverse       = MOTOR_DRIVER_PARAM_MOTOR2_GPIO_DIR_REVERSE, \
            } \
        }, \
        .motor_set_post_cb = MOTOR_DRIVER_PARAM_MOTOR_SET_POST_CALLBACK \
    }
#endif

/* SAUL */
#ifndef MOTOR_DRIVER_SAUL_INFO
/** SAUL information */
#  define MOTOR_DRIVER_SAUL_INFO       { .name = "motor_driver" }
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
