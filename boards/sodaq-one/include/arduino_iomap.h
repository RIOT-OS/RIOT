/*
 * Copyright (C)  2019 Kees Bakker
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_sodaq-one
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Kees Bakker <kees@ijzerbout.nl>
 */

#include "periph/adc.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The green of the RGB led is used as "the builtin led"
 */
#define ARDUINO_LED         (15)

/**
 * @name    Mapping of MCU pins to digital Arduino pins
 * @{
 */
/* D0..D3 - Main IO Pins Digital Properties */
#define ARDUINO_PIN_0           GPIO_PIN(PA, 2)
#define ARDUINO_PIN_1           GPIO_PIN(PA, 3)
#define ARDUINO_PIN_2           GPIO_PIN(PB, 8)
#define ARDUINO_PIN_3           GPIO_PIN(PB, 9)

/* D4..D5 - Other Digital Pins */
#define ARDUINO_PIN_4           GPIO_PIN(PA, 21)
#define ARDUINO_PIN_5           GPIO_PIN(PA, 20)

/* D6..D13 - Main IO Pins Digital Properties */
#define ARDUINO_PIN_6           GPIO_PIN(PA, 6)
#define ARDUINO_PIN_7           GPIO_PIN(PA, 7)
#define ARDUINO_PIN_8           GPIO_PIN(PA, 8)
#define ARDUINO_PIN_9           GPIO_PIN(PA, 9)
#define ARDUINO_PIN_10          GPIO_PIN(PA, 10)
#define ARDUINO_PIN_11          GPIO_PIN(PA, 11)
#define ARDUINO_PIN_12          GPIO_PIN(PB, 2)
#define ARDUINO_PIN_13          GPIO_PIN(PB, 3)

/* D14..D21 - Other Digital Pins */
#define ARDUINO_PIN_14          GPIO_PIN(PA, 15)
#define ARDUINO_PIN_15          GPIO_PIN(PB, 10)
#define ARDUINO_PIN_16          GPIO_PIN(PB, 11)
#define ARDUINO_PIN_17          GPIO_PIN(PA, 14)
#define ARDUINO_PIN_18          GPIO_PIN(PA, 18)
#define ARDUINO_PIN_19          GPIO_PIN(PA, 16)
#define ARDUINO_PIN_20          GPIO_PIN(PB, 22)
#define ARDUINO_PIN_21          GPIO_PIN(PA, 17)

/* D22..D33 - Main IO Pins Analog Properties */
#define ARDUINO_PIN_22          GPIO_PIN(PA, 2)
#define ARDUINO_PIN_23          GPIO_PIN(PA, 3)
#define ARDUINO_PIN_24          GPIO_PIN(PB, 8)
#define ARDUINO_PIN_25          GPIO_PIN(PB, 9)
#define ARDUINO_PIN_26          GPIO_PIN(PA, 6)
#define ARDUINO_PIN_27          GPIO_PIN(PA, 7)
#define ARDUINO_PIN_28          GPIO_PIN(PA, 8)
#define ARDUINO_PIN_29          GPIO_PIN(PA, 9)
#define ARDUINO_PIN_30          GPIO_PIN(PA, 10)
#define ARDUINO_PIN_31          GPIO_PIN(PA, 11)
#define ARDUINO_PIN_32          GPIO_PIN(PB, 2)
#define ARDUINO_PIN_33          GPIO_PIN(PB, 3)

/* D34..D36 - Other Analog Pins */
#define ARDUINO_PIN_34          GPIO_PIN(PA, 2)
#define ARDUINO_PIN_35          GPIO_PIN(PA, 3)
#define ARDUINO_PIN_36          GPIO_PIN(PA, 5)

/* D37..D38 - USB Pins */
#define ARDUINO_PIN_37          GPIO_PIN(PA, 24)
#define ARDUINO_PIN_38          GPIO_PIN(PA, 25)

/* D39..D40 - Serial */
#define ARDUINO_PIN_39          GPIO_PIN(PB, 3)
#define ARDUINO_PIN_40          GPIO_PIN(PB, 2)

/* D41..D42 - Serial1 */
#define ARDUINO_PIN_41          GPIO_PIN(PA, 13)
#define ARDUINO_PIN_42          GPIO_PIN(PA, 12)

/* D43..D46 - SPI  */
#define ARDUINO_PIN_43          GPIO_PIN(PA, 8)
#define ARDUINO_PIN_44          GPIO_PIN(PA, 9)
#define ARDUINO_PIN_45          GPIO_PIN(PA, 10)
#define ARDUINO_PIN_46          GPIO_PIN(PA, 11)

/* D47..D48 - I2C */
#define ARDUINO_PIN_47          GPIO_PIN(PA, 22)
#define ARDUINO_PIN_48          GPIO_PIN(PA, 23)

/* D49 - LoRa RESET */
#define ARDUINO_PIN_49          GPIO_PIN(PA,  4)

/* D50 - MAG_INT */
#define ARDUINO_PIN_50          GPIO_PIN(PA, 19)

#define ARDUINO_PIN_LAST        50
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
#define ARDUINO_A10             ADC_LINE(10)

#define ARDUINO_ANALOG_PIN_LAST 10
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
