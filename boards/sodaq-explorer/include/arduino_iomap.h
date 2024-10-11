/*
 * Copyright (C)  2019 Kees Bakker
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-explorer
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Kees Bakker <kees@ijzerbout.nl>
 */

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/adc.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The builtin LED
 */
#define ARDUINO_LED         (13)

/**
 * @name    Mapping of MCU pins to digital Arduino pins
 * @{
 */
/* D0..D1 - SERCOM/UART (Serial) */
#define ARDUINO_PIN_0           GPIO_PIN(PB, 31)
#define ARDUINO_PIN_1           GPIO_PIN(PB, 30)

/* D2..D15 - Digital */
#define ARDUINO_PIN_2           GPIO_PIN(PA, 2)
#define ARDUINO_PIN_3           GPIO_PIN(PA, 3)
#define ARDUINO_PIN_4           GPIO_PIN(PB, 4)
#define ARDUINO_PIN_5           GPIO_PIN(PB, 6)
#define ARDUINO_PIN_6           GPIO_PIN(PB, 7)
#define ARDUINO_PIN_7           GPIO_PIN(PB, 8)
#define ARDUINO_PIN_8           GPIO_PIN(PB, 10)
#define ARDUINO_PIN_9           GPIO_PIN(PB, 11)
#define ARDUINO_PIN_10          GPIO_PIN(PA, 23)
#define ARDUINO_PIN_11          GPIO_PIN(PA, 20)
#define ARDUINO_PIN_12          GPIO_PIN(PA, 22)
#define ARDUINO_PIN_13          GPIO_PIN(PA, 21)
#define ARDUINO_PIN_14          GPIO_PIN(PA, 10)
#define ARDUINO_PIN_15          GPIO_PIN(PA, 11)

/* D16..D19 - Other Digital */
#define ARDUINO_PIN_16          GPIO_PIN(PA, 12)
#define ARDUINO_PIN_17          GPIO_PIN(PB, 15)
#define ARDUINO_PIN_18          GPIO_PIN(PA, 13)
#define ARDUINO_PIN_19          GPIO_PIN(PA, 15)

/* D20..D28 - A0..A8 */
#define ARDUINO_PIN_20          GPIO_PIN(PB,  0)
#define ARDUINO_PIN_21          GPIO_PIN(PB,  1)
#define ARDUINO_PIN_22          GPIO_PIN(PB,  2)
#define ARDUINO_PIN_23          GPIO_PIN(PB,  3)
#define ARDUINO_PIN_24          GPIO_PIN(PA,  8)
#define ARDUINO_PIN_25          GPIO_PIN(PA,  9)
#define ARDUINO_PIN_26          GPIO_PIN(PA,  4)
#define ARDUINO_PIN_27          GPIO_PIN(PA,  10)
#define ARDUINO_PIN_28          GPIO_PIN(PA,  11)

/* D29..D30 - SERCOM/UART (Serial1) */
#define ARDUINO_PIN_29          GPIO_PIN(PB, 14)
#define ARDUINO_PIN_30          GPIO_PIN(PB, 13)

/* D31..D32 - SERCOM/UART (Serial2) */
#define ARDUINO_PIN_31          GPIO_PIN(PA, 6)
#define ARDUINO_PIN_32          GPIO_PIN(PA, 5)

/* D33..D34 - I2C pins (SDA/SCL) */
#define ARDUINO_PIN_33          GPIO_PIN(PA, 16)
#define ARDUINO_PIN_34          GPIO_PIN(PA, 17)

/* D35..D36 - I2C1 pins (SDA/SCL) */
#define ARDUINO_PIN_35          GPIO_PIN(PA, 8)
#define ARDUINO_PIN_36          GPIO_PIN(PA, 9)

/* D37..D40 - SPI pins (ICSP: MISO, SS, MOSI, SCK) */
/* Notice that SCK is sharing the same PA21 with LED_BUILTIN */
#define ARDUINO_PIN_37          GPIO_PIN(PA, 22)
#define ARDUINO_PIN_38          GPIO_PIN(PA, 23)
#define ARDUINO_PIN_39          GPIO_PIN(PA, 20)
#define ARDUINO_PIN_40          GPIO_PIN(PA, 21)

/* D41..D42 - USB */
#define ARDUINO_PIN_41          GPIO_PIN(PA, 24)
#define ARDUINO_PIN_42          GPIO_PIN(PA, 25)

/* D43 - DAC */
#define ARDUINO_PIN_43          GPIO_PIN(PA,  2)

/* D44 - Flash CS */
#define ARDUINO_PIN_44          GPIO_PIN(PB, 16)

/* D45..D46 - LoRa, BT Reset */
#define ARDUINO_PIN_45          GPIO_PIN(PA,  7)
#define ARDUINO_PIN_46          GPIO_PIN(PB,  17)

/* D47 - Button */
#define ARDUINO_PIN_47          GPIO_PIN(PA,  14)

/* D48 - BAT_VOLT */
#define ARDUINO_PIN_48          GPIO_PIN(PB,  5)

#define ARDUINO_PIN_LAST        48
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_20
#define ARDUINO_PIN_A1          ARDUINO_PIN_21
#define ARDUINO_PIN_A2          ARDUINO_PIN_22
#define ARDUINO_PIN_A3          ARDUINO_PIN_23
#define ARDUINO_PIN_A4          ARDUINO_PIN_24
#define ARDUINO_PIN_A5          ARDUINO_PIN_25
#define ARDUINO_PIN_A6          ARDUINO_PIN_26
#define ARDUINO_PIN_A7          ARDUINO_PIN_27
#define ARDUINO_PIN_A8          ARDUINO_PIN_28
/** @} */

/**
 * @name    Analog pin to adc line mapping
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)
#define ARDUINO_A1              ADC_LINE(1)
#define ARDUINO_A2              ADC_LINE(2)
#define ARDUINO_A3              ADC_LINE(3)
#define ARDUINO_A4              ADC_LINE(4)
#define ARDUINO_A5              ADC_LINE(5)
#define ARDUINO_A6              ADC_LINE(6)
#define ARDUINO_A7              ADC_LINE(7)
#define ARDUINO_A8              ADC_LINE(8)
#define ARDUINO_A9              ADC_LINE(9)

#define ARDUINO_ANALOG_PIN_LAST 9
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARDUINO_IOMAP_H */
