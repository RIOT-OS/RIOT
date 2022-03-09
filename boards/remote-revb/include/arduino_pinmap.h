/*
 * Copyright (C) 2020 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_remote-revb
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins for Remote revision B board
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Mapping of MCU pins to Arduino pins
 *
 * @warning Beware: Despite an Arudino pin mapping being available, Arduino shields
 *          are mechanically not compatible with the board. Check header file
 *          https://github.com/RIOT-OS/RIOT/blob/master/boards/remote-revb/include/arduino_pinmap.h#L43
 *          for the exact mapping.
 *
 * @{
 */

#define ARDUINO_PIN_1           GPIO_PIN(PORT_D, 4) // LED1.R
#define ARDUINO_PIN_2           GPIO_PIN(PORT_B, 7) // LED2.G/JTAG.TDO
#define ARDUINO_PIN_3           GPIO_PIN(PORT_B, 6) // LED3.B/JTAG.TDI
#define ARDUINO_PIN_4           GPIO_PIN(PORT_A, 0) // UART0.RX
#define ARDUINO_PIN_5           GPIO_PIN(PORT_A, 1) // UART0.TX
#define ARDUINO_PIN_6           GPIO_PIN(PORT_D, 0) // I2C.INT
#define ARDUINO_PIN_7           GPIO_PIN(PORT_C, 2) // I2C.SDA
#define ARDUINO_PIN_8           GPIO_PIN(PORT_C, 3) // I2C.SCL
#define ARDUINO_PIN_11          GPIO_PIN(PORT_B, 4) // CC1200.GPIO0
#define ARDUINO_PIN_12          GPIO_PIN(PORT_B, 0) // CC1200.GPIO2
#define ARDUINO_PIN_13          GPIO_PIN(PORT_C, 1) // UART1.RX
#define ARDUINO_PIN_14          GPIO_PIN(PORT_C, 0) // UART1.TX
#define ARDUINO_PIN_17          GPIO_PIN(PORT_B, 5) // CC1200.CSN
#define ARDUINO_PIN_21          GPIO_PIN(PORT_A, 3) // USER.BUTTON
#define ARDUINO_PIN_22          GPIO_PIN(PORT_B, 2) // CC1200.CLK
#define ARDUINO_PIN_23          GPIO_PIN(PORT_B, 1) // CC1200.MOSI
#define ARDUINO_PIN_24          GPIO_PIN(PORT_B, 3) // CC1200.MISO
#define ARDUINO_PIN_25          GPIO_PIN(PORT_A, 7) // ADC5/AIN7
#define ARDUINO_PIN_28          GPIO_PIN(PORT_A, 5) // ADC1/AIN5
#define ARDUINO_PIN_29          GPIO_PIN(PORT_A, 4) // ADC2/AIN4
#define ARDUINO_PIN_32          GPIO_PIN(PORT_A, 2) // ADC3/AIN2
#define ARDUINO_PIN_33          GPIO_PIN(PORT_A, 6) // USD.SEL/ADC4
#define ARDUINO_PIN_34          GPIO_PIN(PORT_C, 6) // USD.MISO
#define ARDUINO_PIN_35          GPIO_PIN(PORT_C, 5) // USD.MOSI
#define ARDUINO_PIN_36          GPIO_PIN(PORT_C, 4) // USD.SCLK
#define ARDUINO_PIN_A1          ARDUINO_PIN_28
#define ARDUINO_PIN_A2          ARDUINO_PIN_29
#define ARDUINO_PIN_A3          ARDUINO_PIN_32
#define ARDUINO_PIN_A4          ARDUINO_PIN_33
#define ARDUINO_PIN_A5          ARDUINO_PIN_25

#define ARDUINO_A1              ADC_LINE(1)
#define ARDUINO_A2              ADC_LINE(2)
#define ARDUINO_A3              ADC_LINE(3)
#define ARDUINO_A4              ADC_LINE(4)
#define ARDUINO_A5              ADC_LINE(5)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
