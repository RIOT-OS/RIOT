/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_adafruit-metro-m4-express
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PA, 23)
#define ARDUINO_PIN_1           GPIO_PIN(PA, 22)
#define ARDUINO_PIN_2           GPIO_PIN(PB, 17)
#define ARDUINO_PIN_3           GPIO_PIN(PB, 16)
#define ARDUINO_PIN_4           GPIO_PIN(PB, 13)
#define ARDUINO_PIN_5           GPIO_PIN(PB, 14)
#define ARDUINO_PIN_6           GPIO_PIN(PB, 15)
#define ARDUINO_PIN_7           GPIO_PIN(PB, 12)

#define ARDUINO_PIN_8           GPIO_PIN(PA, 21)
#define ARDUINO_PIN_9           GPIO_PIN(PA, 20)
#define ARDUINO_PIN_10          GPIO_PIN(PA, 18)
#define ARDUINO_PIN_11          GPIO_PIN(PA, 19)
#define ARDUINO_PIN_12          GPIO_PIN(PA, 17)
#define ARDUINO_PIN_13          GPIO_PIN(PA, 16)

/* SDA and SCL */
#define ARDUINO_PIN_14          GPIO_PIN(PB, 2) /* SDA */
#define ARDUINO_PIN_15          GPIO_PIN(PB, 3) /* SCL */

/* Analog pins as digital pins: */
#define ARDUINO_PIN_16          GPIO_PIN(PA, 2) /* A0 */
#define ARDUINO_PIN_17          GPIO_PIN(PA, 5) /* A1 */
#define ARDUINO_PIN_18          GPIO_PIN(PA, 6) /* A2 */
#define ARDUINO_PIN_19          GPIO_PIN(PA, 4) /* A3 */
#define ARDUINO_PIN_20          GPIO_PIN(PB, 8) /* A4 */
#define ARDUINO_PIN_21          GPIO_PIN(PB, 9) /* A5 */

#define ARDUINO_PIN_LAST        21
/** @} */

/**
 * @name    Aliases for non-digital pins as digital pins
 * @{
 */
#define ARDUINO_PIN_SDA         ARDUINO_PIN_14
#define ARDUINO_PIN_SCL         ARDUINO_PIN_15
#define ARDUINO_PIN_A0          ARDUINO_PIN_16
#define ARDUINO_PIN_A1          ARDUINO_PIN_17
#define ARDUINO_PIN_A2          ARDUINO_PIN_18
#define ARDUINO_PIN_A3          ARDUINO_PIN_19
#define ARDUINO_PIN_A4          ARDUINO_PIN_20
#define ARDUINO_PIN_A5          ARDUINO_PIN_21
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)
#define ARDUINO_A1              ADC_LINE(1)
#define ARDUINO_A2              ADC_LINE(2)
#define ARDUINO_A3              ADC_LINE(3)
#define ARDUINO_A4              ADC_LINE(4)
#define ARDUINO_A5              ADC_LINE(5)

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

/**
 * @name    Arduino's default SPI device
 * @{
 */
/**
 * @brief   SPI_DEV(0) is connected to the ISP header
 */
#define ARDUINO_SPI_ISP         SPI_DEV(0)

/**
 * @brief   SPI_DEV(1) is connected to D11/D12/D13
 *
 * The SPI on D11/D12/D13 is only available when `periph_uart` is not used due
 * to a conflicting use of the same SERCOM. If it is available, it is the last
 * SPI bus.
 */
#if !MODULE_PERIPH_UART
#  define ARDUINO_SPI_D11D12D13 SPI_DEV(1)
#endif
/** @} */

/**
 * @name    Arduino's UART devices
 * @{
 */
#define ARDUINO_UART_D0D1       UART_DEV(0)
/** @} */

/**
 * @name    Arduino's I2C buses
 * @{
 */
/**
 * @brief   The first I2C bus is where shields for the Arduino UNO/Mega expect
 *          it
 */
#define ARDUINO_I2C_UNO         I2C_DEV(0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
