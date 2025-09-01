/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_b-u585i-iot02a
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ST B-U585I-IOT02A board
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
#define LED0_PIN_NUM        13
#define LED0_PORT           GPIO_PORT_E /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_E

#define LED1_PIN_NUM        6
#define LED1_PORT           GPIO_PORT_H /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_H

#define LED2_PIN_NUM        7
#define LED2_PORT           GPIO_PORT_H /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_H
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    HTS221 temperature/humidity sensor configuration
 * @{
 */
#define HTS221_PARAM_I2C    I2C_DEV(1)
/** @} */

/**
 * @name    LPS22HH pressure/temperature sensor configuration
 * @{
 */
#define LPSXXX_PARAM_I2C    I2C_DEV(1)
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
