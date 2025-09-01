/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_microbit
 * @{
 *
 * @file
 * @brief       Board specific configuration for the BBC micro:bit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED matrix pin configuration
 * @{
 */
#define MICROBIT_LED_COL1           GPIO_PIN(0,  4)
#define MICROBIT_LED_COL2           GPIO_PIN(0,  5)
#define MICROBIT_LED_COL3           GPIO_PIN(0,  6)
#define MICROBIT_LED_COL4           GPIO_PIN(0,  7)
#define MICROBIT_LED_COL5           GPIO_PIN(0,  8)
#define MICROBIT_LED_COL6           GPIO_PIN(0,  9)
#define MICROBIT_LED_COL7           GPIO_PIN(0, 10)
#define MICROBIT_LED_COL8           GPIO_PIN(0, 11)
#define MICROBIT_LED_COL9           GPIO_PIN(0, 12)
#define MICROBIT_LED_ROW1           GPIO_PIN(0, 13)
#define MICROBIT_LED_ROW2           GPIO_PIN(0, 14)
#define MICROBIT_LED_ROW3           GPIO_PIN(0, 15)
/** @} */

/**
 * @name    Button configuration
 * @{
 */
#define BTN0_PIN                    GPIO_PIN(0, 17)
#define BTN0_MODE                   GPIO_IN
#define BTN1_PIN                    GPIO_PIN(0, 26)
#define BTN1_MODE                   GPIO_IN
/** @} */

/**
 * @name    MMA8653 accelerometer configuration
 * @{
 */
#define MMA8X5X_PARAM_I2C           I2C_DEV(0)
#define MMA8X5X_PARAM_ADDR          0x1d
/** @} */

/**
 * @name    MAG3110 magnetometer configuration
 * @{
 */
#define MAG3110_PARAM_I2C           I2C_DEV(0)
#define MAG3110_PARAM_ADDR          0x0e
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
