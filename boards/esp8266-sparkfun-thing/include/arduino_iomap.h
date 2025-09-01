/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp8266_sparkfun-thing
 * @brief       Board specific configuration for the Arduino API
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include "periph/adc.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to GPIO5 (Arduino pin 6) on this board
 */
#define ARDUINO_LED     (6)

/**
 * @brief   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0   GPIO1       /* D0 (RxD) */
#define ARDUINO_PIN_1   GPIO3       /* D1 (TxD) */
#define ARDUINO_PIN_2   GPIO0       /* D2 */
#define ARDUINO_PIN_3   GPIO4       /* D3 */
#if defined(CONFIG_FLASHMODE_QIO) || defined(CONFIG_FLASHMODE_QOUT)
#  define ARDUINO_PIN_4 GPIO9       /* D4 */
#  define ARDUINO_PIN_5 GPIO10      /* D5 */
#endif
#define ARDUINO_PIN_6   GPIO5       /* D6 */
#define ARDUINO_PIN_10  GPIO15      /* D10 (CS0)  */
#define ARDUINO_PIN_11  GPIO13      /* D11 (MOSI) */
#define ARDUINO_PIN_12  GPIO12      /* D12 (MISO) */
#define ARDUINO_PIN_13  GPIO14      /* D13 (SCK)  */

#define ARDUINO_PIN_18  GPIO2       /* D18 (A4 / SDA) */
#define ARDUINO_PIN_19  GPIO14      /* D19 (A5 / SCL) */

#define ARDUINO_PIN_LAST        19
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A4          ARDUINO_PIN_18
#define ARDUINO_PIN_A5          ARDUINO_PIN_19
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
