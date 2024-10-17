/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf51dk
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's SPI buses
 * @{
 */
/**
 * @brief   SPI_DEV(0) is connected to D11/D12/D13
 */
/* D11/D12/D13 are on P0.25, P0.28, P0.29 */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(0)
/* The ISP SPI bus is also connected to P0.25, P0.28, P0.29 */
#define ARDUINO_SPI_ISP         SPI_DEV(0)
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

/**
 * @name   Mapping of MCU pins to Arduino pins
 * @{
 */
/* P3 header on the nRF51DK */
#define ARDUINO_PIN_0           GPIO_PIN(0, 12)
#define ARDUINO_PIN_1           GPIO_PIN(0, 13)
#define ARDUINO_PIN_2           GPIO_PIN(0, 14)
#define ARDUINO_PIN_3           GPIO_PIN(0, 15)
#define ARDUINO_PIN_4           GPIO_PIN(0, 16)
#define ARDUINO_PIN_5           GPIO_PIN(0, 17)
#define ARDUINO_PIN_6           GPIO_PIN(0, 18)
#define ARDUINO_PIN_7           GPIO_PIN(0, 19)

/* P4 header on the nRF51DK */
#define ARDUINO_PIN_8           GPIO_PIN(0, 20)
#define ARDUINO_PIN_9           GPIO_PIN(0, 23)
#define ARDUINO_PIN_10          GPIO_PIN(0, 24)
#define ARDUINO_PIN_11          GPIO_PIN(0, 25)
#define ARDUINO_PIN_12          GPIO_PIN(0, 28)
#define ARDUINO_PIN_13          GPIO_PIN(0, 29)
/* SCL and SDA on this header are not available as digital pins in the
 * Arduino world. It is P0.30 and P0.07 here, though. Also what would be
 * AREF in the Arduino world is P0.00 here */

/* P2 header on the nRF51DK */
#define ARDUINO_PIN_14          GPIO_PIN(0, 1)
#define ARDUINO_PIN_15          GPIO_PIN(0, 2)
#define ARDUINO_PIN_16          GPIO_PIN(0, 3)
#define ARDUINO_PIN_17          GPIO_PIN(0, 4)
#define ARDUINO_PIN_18          GPIO_PIN(0, 5)
#define ARDUINO_PIN_19          GPIO_PIN(0, 6)

#define ARDUINO_PIN_LAST        19
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_14
#define ARDUINO_PIN_A1          ARDUINO_PIN_15
#define ARDUINO_PIN_A2          ARDUINO_PIN_16
#define ARDUINO_PIN_A3          ARDUINO_PIN_17
#define ARDUINO_PIN_A4          ARDUINO_PIN_18
#define ARDUINO_PIN_A5          ARDUINO_PIN_19
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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARDUINO_IOMAP_H */
