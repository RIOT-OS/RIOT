/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_b-l475e-iot01a
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ST B-L475E-IOT01A board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        5
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        14
#define LED1_PORT           GPIO_PORT_B /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_B
/** @} */

/**
 * @name    User button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)    /**< User button pin */
#define BTN0_MODE           GPIO_IN_PU              /**< User button pin mode */
/** @} */

/**
 * @name    HTS221 temperature/humidity sensor configuration
 * @{
 */
#define HTS221_PARAM_I2C    I2C_DEV(1)
/** @} */

/**
 * @name    LIS3MDL 3-axis magnetometer configuration
 * @{
 */
#define LIS3MDL_PARAM_I2C   I2C_DEV(1)
/** @} */

/**
 * @name    LPS22HB pressure/temperature sensor configuration
 * @{
 */
#define LPSXXX_PARAM_I2C    I2C_DEV(1)
/** @} */

/**
 * @name    LSM6DSL accelerometer sensor configuration
 * @{
 */
#define LSM6DSXX_PARAM_I2C   I2C_DEV(1)
#define LSM6DSXX_PARAM_ADDR  (0x6A)
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
