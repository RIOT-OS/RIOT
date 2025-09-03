/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_crazyflie
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Crazyflie 2.1
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#include "cpu.h"
#include "motor_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_IS_INVERTED    1
#define LED0_PIN_NUM        3 /* LED top right red */
#define LED0_PORT           GPIO_PORT_C
#define LED0_PORT_NUM       PORT_C

#define LED1_IS_INVERTED    1
#define LED1_PIN_NUM        2 /* LED top right green */
#define LED1_PORT           GPIO_PORT_C
#define LED1_PORT_NUM       PORT_C

#define LED2_PIN_NUM        2 /* LED bottom left blue */
#define LED2_PORT           GPIO_PORT_D
#define LED2_PORT_NUM       PORT_D

#define LED3_IS_INVERTED    1
#define LED3_PIN_NUM        0 /* LED top left red*/
#define LED3_PORT           GPIO_PORT_C
#define LED3_PORT_NUM       PORT_C

#define LED4_IS_INVERTED    1
#define LED4_PIN_NUM        1 /* LED top left green */
#define LED4_PORT           GPIO_PORT_C
#define LED4_PORT_NUM       PORT_C
/** @} */

/**
 * @name Describe DC motors with PWM channel and GPIOs
 * @{
 */
static const motor_driver_config_t motor_driver_config[] = {
    {
        .pwm_dev = 1,
        .mode = MOTOR_DRIVER_1_DIR,
        .mode_brake = MOTOR_BRAKE_HIGH,
        .pwm_mode = PWM_LEFT,
        .pwm_frequency = 20000U,
        .pwm_resolution = 4200U,
        .nb_motors = 3,
        .motors = {
            {
                .pwm_channel = 1,
                .gpio_enable = 0,
                .gpio_dir0 = 0,
                .gpio_dir1_or_brake = 0,
                .gpio_dir_reverse = 0,
                .gpio_enable_invert = 0,
                .gpio_brake_invert = 0,
            },
            {
                .pwm_channel = 3,
                .gpio_enable = 0,
                .gpio_dir0 = 0,
                .gpio_dir1_or_brake = 0,
                .gpio_dir_reverse = 0,
                .gpio_enable_invert = 0,
                .gpio_brake_invert = 0,
            },
            {
                .pwm_channel = 0,
                .gpio_enable = 0,
                .gpio_dir0 = 0,
                .gpio_dir1_or_brake = 0,
                .gpio_dir_reverse = 0,
                .gpio_enable_invert = 0,
                .gpio_brake_invert = 0,
            },
        },
        .cb = NULL,
    },
    {
        .pwm_dev = 2,
        .mode = MOTOR_DRIVER_1_DIR,
        .mode_brake = MOTOR_BRAKE_HIGH,
        .pwm_mode = PWM_LEFT,
        .pwm_frequency = 20000U,
        .pwm_resolution = 4200U,
        .nb_motors = 1,
        .motors = {
            {
                .pwm_channel = 0,
                .gpio_enable = 0,
                .gpio_dir0 = 0,
                .gpio_dir1_or_brake = 0,
                .gpio_dir_reverse = 0,
                .gpio_enable_invert = 0,
                .gpio_brake_invert = 0,
            },
        },
        .cb = NULL,
    }
};

#define MOTOR_DRIVER_NUMOF           ARRAY_SIZE(motor_driver_config)
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"
/** @} */
