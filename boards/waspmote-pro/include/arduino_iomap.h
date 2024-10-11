/*
 * Copyright (C) 2020 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waspmote-pro
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins for Waspmote Pro board
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Mapping of MCU pins to digital Arduino pins
 *
 * @note    ISCP pins are not mapped.
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_E, 0)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_E, 1)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_E, 3)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_E, 4)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_C, 4)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_C, 5)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_C, 6)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_C, 7)
#define ARDUINO_PIN_8           GPIO_PIN(PORT_A, 2)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_A, 3)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_D, 5)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_D, 6)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_14          GPIO_PIN(PORT_F, 1)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_F, 2)
#define ARDUINO_PIN_16          GPIO_PIN(PORT_F, 3)
#define ARDUINO_PIN_17          GPIO_PIN(PORT_F, 4)
#define ARDUINO_PIN_18          GPIO_PIN(PORT_F, 5)
#define ARDUINO_PIN_19          GPIO_PIN(PORT_F, 6)
#define ARDUINO_PIN_20          GPIO_PIN(PORT_F, 7)
#define ARDUINO_PIN_21          GPIO_PIN(PORT_F, 0)
#define ARDUINO_PIN_22          GPIO_PIN(PORT_A, 1)
#define ARDUINO_PIN_23          GPIO_PIN(PORT_D, 7)
#define ARDUINO_PIN_24          GPIO_PIN(PORT_E, 5)
#define ARDUINO_PIN_25          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_26          GPIO_PIN(PORT_E, 2)
#define ARDUINO_PIN_27          GPIO_PIN(PORT_A, 5)
#define ARDUINO_PIN_28          GPIO_PIN(PORT_C, 0)
#define ARDUINO_PIN_29          GPIO_PIN(PORT_B, 0)
#define ARDUINO_PIN_30          GPIO_PIN(PORT_B, 1)
#define ARDUINO_PIN_31          GPIO_PIN(PORT_B, 2)
#define ARDUINO_PIN_32          GPIO_PIN(PORT_B, 3)
#define ARDUINO_PIN_33          GPIO_PIN(PORT_B, 4)
#define ARDUINO_PIN_34          GPIO_PIN(PORT_B, 5)
#define ARDUINO_PIN_35          GPIO_PIN(PORT_A, 0)
#define ARDUINO_PIN_36          GPIO_PIN(PORT_B, 6)
#define ARDUINO_PIN_37          GPIO_PIN(PORT_B, 7)
#define ARDUINO_PIN_38          GPIO_PIN(PORT_E, 6)
#define ARDUINO_PIN_39          GPIO_PIN(PORT_E, 7)
#define ARDUINO_PIN_40          GPIO_PIN(PORT_D, 0)
#define ARDUINO_PIN_41          GPIO_PIN(PORT_D, 1)
#define ARDUINO_PIN_42          GPIO_PIN(PORT_C, 3)
#define ARDUINO_PIN_43          GPIO_PIN(PORT_D, 2)
#define ARDUINO_PIN_44          GPIO_PIN(PORT_D, 3)
#define ARDUINO_PIN_45          GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_46          GPIO_PIN(PORT_C, 2)
#define ARDUINO_PIN_47          GPIO_PIN(PORT_D, 4)
#define ARDUINO_PIN_48          GPIO_PIN(PORT_G, 2)
#define ARDUINO_PIN_49          GPIO_PIN(PORT_G, 1)
#define ARDUINO_PIN_50          GPIO_PIN(PORT_G, 0)
#define ARDUINO_PIN_51          GPIO_PIN(PORT_G, 3)
#define ARDUINO_PIN_52          GPIO_PIN(PORT_G, 4)

#define ARDUINO_PIN_LAST        52
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
#define ARDUINO_PIN_A6          ARDUINO_PIN_20
#define ARDUINO_PIN_A7          ARDUINO_PIN_21
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
#define ARDUINO_ANALOG_PIN_LAST 7
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARDUINO_IOMAP_H */
