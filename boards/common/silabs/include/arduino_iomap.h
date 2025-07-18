/*
 * SPDX-FileCopyrightText: 2018 Federico Pellegrin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_silabs
 * @brief       SiLabs Boards configuration for the Arduino API
 * @file
 * @author      Federico Pellegrin <fede@evolware.org>
 * @{
 */

#include "board.h"
#include "periph_cpu.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's digital pins mappings
 * @{
 */
#define ARDUINO_PIN_0       LED0_PIN
#define ARDUINO_PIN_1       LED1_PIN
#define ARDUINO_PIN_2       PB0_PIN
#define ARDUINO_PIN_3       PB1_PIN

#define ARDUINO_PIN_LAST    3
/** @} */

/**
 * @name    Arduino's analog pins mappings
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)

#define ARDUINO_ANALOG_PIN_LAST 0
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
