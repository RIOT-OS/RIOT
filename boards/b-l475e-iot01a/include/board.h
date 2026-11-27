/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

/**
 * @name    PDM microphone configuration
 *
 * The board has two MP34DT01-M PDM microphones on one clock and data line.
 * @{
 */
/**
 * @brief   GPIO pin for the data line
 */
#ifndef PDM_DIN_PIN
#  define PDM_DIN_PIN GPIO_PIN(PORT_E, 7)
#endif

/**
 * @brief   GPIO pin for the clock
 */
#ifndef PDM_CLK_PIN
#  define PDM_CLK_PIN GPIO_PIN(PORT_E, 9)
#endif

/**
 * @brief   Minimum clock frequency of the MP34DT01-M in normal mode
 * @see     https://www.st.com/resource/en/datasheet/mp34dt01-m.pdf
 */
#ifndef PDM_MIC_CLK_MIN_HZ
#  define PDM_MIC_CLK_MIN_HZ 1000000
#endif

/**
 * @brief   Maximum clock frequency of the MP34DT01-M in normal mode
 * @see     https://www.st.com/resource/en/datasheet/mp34dt01-m.pdf
 */
#ifndef PDM_MIC_CLK_MAX_HZ
#  define PDM_MIC_CLK_MAX_HZ 3250000
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
