/*
 * SPDX-FileCopyrightText: 2026 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_ulanzi-tc001
 * @brief       Board specific definitions for Ulanzi TC001
 * @{
 *
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */
#define BTN0_PIN        GPIO26
#define BTN0_MODE       GPIO_IN_PU
#define BTN0_INT_FLANK  GPIO_FALLING

#define BTN1_PIN        GPIO27
#define BTN1_MODE       GPIO_IN_PU
#define BTN1_INT_FLANK  GPIO_FALLING

#define BTN2_PIN        GPIO14
#define BTN2_MODE       GPIO_IN_PU
#define BTN2_INT_FLANK  GPIO_FALLING
/** @} */

/**
 * @name    WS281x LED matrix definitions
 * @{
 */
#define WS281X_PARAM_PIN    GPIO32
#define WS281X_PARAM_NUMOF  (32*8)
/** @} */

/**
 * @brief   SHTC30 temperature / humidity sensor
 */
#define SHT3X_PARAM_I2C_ADDR SHT3X_I2C_ADDR_1

/**
 * @name    LED (on-board) configuration
 *
 * Generic ESP32 boards usually do not have on-board LEDs.
 * @{
 */
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
