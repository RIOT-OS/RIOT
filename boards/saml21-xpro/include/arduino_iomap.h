/*
 * SPDX-FileCopyrightText: 2018 Federico Pellegrin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_saml21-xpro
 * @brief       Mapping from board pins to Arduino pins
 * @file
 * @author      Federico Pellegrin <fede@evolware.org>
 * @{
 */

#include "board.h"
#include "periph/adc.h"
#include "periph/uart.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's default UART device
 * @{
 */
#define ARDUINO_UART_DEV         UART_DEV(0)
/** @} */

/**
 * @name    Arduino's digital pins mappings
 * @{
 */
#define ARDUINO_PIN_0       LED0_PIN
#define ARDUINO_PIN_1       BTN0_PIN

#define ARDUINO_PIN_LAST    1
/** @} */

/**
 * @name    Arduino's analog pins mappings
 * @{
 */
#define ARDUINO_A0                  ADC_LINE(0)
#define ARDUINO_A1                  ADC_LINE(1)
#define ARDUINO_A2                  ADC_LINE(2)

#define ARDUINO_ANALOG_PIN_LAST     2
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
