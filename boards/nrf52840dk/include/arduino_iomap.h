/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dk
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
 * @name    Arduino's UART devices
 * @{
 */
#define ARDUINO_UART_D0D1       UART_DEV(1)
/** @} */

/**
 * @name    Arduino's SPI buses
 * @{
 */
/**
 * @brief   SPI_DEV(1) is connected to D11/D12/D13
 */
/* D11/D12/D13 are on P1.13, P1.14, P1.15 */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(0)
/* The ISP SPI bus is also connected to P1.13, P1.14, P1.15 */
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
/* P3 header on the nRF52840DK */
#define ARDUINO_PIN_0           GPIO_PIN(1, 1)
#define ARDUINO_PIN_1           GPIO_PIN(1, 2)
#define ARDUINO_PIN_2           GPIO_PIN(1, 3)
#define ARDUINO_PIN_3           GPIO_PIN(1, 4)
#define ARDUINO_PIN_4           GPIO_PIN(1, 5)
#define ARDUINO_PIN_5           GPIO_PIN(1, 6)
#define ARDUINO_PIN_6           GPIO_PIN(1, 7)
#define ARDUINO_PIN_7           GPIO_PIN(1, 8)

/* P4 header on the nRF52840DK */
#define ARDUINO_PIN_8           GPIO_PIN(1, 10)
#define ARDUINO_PIN_9           GPIO_PIN(1, 11)
#define ARDUINO_PIN_10          GPIO_PIN(1, 12)
#define ARDUINO_PIN_11          GPIO_PIN(1, 13)
#define ARDUINO_PIN_12          GPIO_PIN(1, 14)
#define ARDUINO_PIN_13          GPIO_PIN(1, 15)
/* SCL and SDA on this header are not available as digital pins in the
 * Arduino world. It is P0.26 and P0.27 here, though. Also what would be
 * AREF in the Arduino world is P0.02 here */

/* P6 header on the nRF52840DK */
#define ARDUINO_PIN_14          GPIO_PIN(0, 10)
#define ARDUINO_PIN_15          GPIO_PIN(0, 9)
#define ARDUINO_PIN_16          GPIO_PIN(0, 8)
#define ARDUINO_PIN_17          GPIO_PIN(0, 7)
#define ARDUINO_PIN_18          GPIO_PIN(0, 6)
#define ARDUINO_PIN_19          GPIO_PIN(0, 5)
#define ARDUINO_PIN_20          GPIO_PIN(0, 1)
#define ARDUINO_PIN_21          GPIO_PIN(0, 0)

/* P24 header on the nRF52840DK
 * Note:    The header is one column short on the right (missing a column of
 *          two 5V supply pins) and 8 columns on the left (missing D40 to D53
 *          and two GND on the leftmost column).
 * Warning: Where D39 is expected a GND pin is sitting instead */
#define ARDUINO_PIN_22          GPIO_PIN(0, 11)
#define ARDUINO_PIN_23          GPIO_PIN(0, 12)
#define ARDUINO_PIN_24          GPIO_PIN(0, 13)
#define ARDUINO_PIN_25          GPIO_PIN(0, 14)
#define ARDUINO_PIN_26          GPIO_PIN(0, 15)
#define ARDUINO_PIN_27          GPIO_PIN(0, 16)
#define ARDUINO_PIN_28          GPIO_PIN(0, 17)
#define ARDUINO_PIN_29          GPIO_PIN(0, 18)
#define ARDUINO_PIN_30          GPIO_PIN(0, 19)
#define ARDUINO_PIN_31          GPIO_PIN(0, 20)
#define ARDUINO_PIN_32          GPIO_PIN(0, 21)
#define ARDUINO_PIN_33          GPIO_PIN(0, 22)
#define ARDUINO_PIN_34          GPIO_PIN(0, 23)
#define ARDUINO_PIN_35          GPIO_PIN(0, 24)
#define ARDUINO_PIN_36          GPIO_PIN(0, 25)
#define ARDUINO_PIN_37          GPIO_PIN(1, 0)
#define ARDUINO_PIN_38          GPIO_PIN(1, 9)

/* P2 header on the nRF52840DK
 * Note:    This header only provides A0 - A5, but it should be two pins
 *          longer for full Arduino Mega compatibility */
#define ARDUINO_PIN_54          GPIO_PIN(0, 3)
#define ARDUINO_PIN_55          GPIO_PIN(0, 4)
#define ARDUINO_PIN_56          GPIO_PIN(0, 28)
#define ARDUINO_PIN_57          GPIO_PIN(0, 29)
#define ARDUINO_PIN_58          GPIO_PIN(0, 30)
#define ARDUINO_PIN_59          GPIO_PIN(0, 31)

/* Note:    There is no header for A8 - A15 present, which would be required
 *          for full Arduino Mega compatibility */
#define ARDUINO_PIN_LAST        59
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_54
#define ARDUINO_PIN_A1          ARDUINO_PIN_55
#define ARDUINO_PIN_A2          ARDUINO_PIN_56
#define ARDUINO_PIN_A3          ARDUINO_PIN_57
#define ARDUINO_PIN_A4          ARDUINO_PIN_58
#define ARDUINO_PIN_A5          ARDUINO_PIN_59
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
/* The nRF52840 has a fixed ADC to GPIO mapping:
 *
 * nRF  | MCU pin   | Exposed as Arduino pin
 * -----|-----------|-----------------------
 * AIN0 | P0.02     | AREF
 * AIN1 | P0.03     | A0 (D54)
 * AIN2 | P0.04     | A1 (D55)
 * AIN3 | P0.05     | -  (D19)
 * AIN4 | P0.28     | A2 (D56)
 * AIN5 | P0.29     | A3 (D57)
 * AIN6 | P0.30     | A4 (D48)
 * AIN7 | P0.31     | A5 (D59)
 */
#define ARDUINO_A0              ADC_LINE(1)
#define ARDUINO_A1              ADC_LINE(2)
#define ARDUINO_A2              ADC_LINE(4)
#define ARDUINO_A3              ADC_LINE(5)
#define ARDUINO_A4              ADC_LINE(6)
#define ARDUINO_A5              ADC_LINE(7)

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARDUINO_IOMAP_H */
