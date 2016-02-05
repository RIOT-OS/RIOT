/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-due
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PA, 8)
#define ARDUINO_PIN_1           GPIO_PIN(PA, 9)
#define ARDUINO_PIN_2           GPIO_PIN(PB, 25)
#define ARDUINO_PIN_3           GPIO_PIN(PC, 28)
#define ARDUINO_PIN_4           GPIO_PIN(PA, 29)
#define ARDUINO_PIN_5           GPIO_PIN(PC, 25)
#define ARDUINO_PIN_6           GPIO_PIN(PC, 24)
#define ARDUINO_PIN_7           GPIO_PIN(PC, 23)
#define ARDUINO_PIN_8           GPIO_PIN(PC, 22)
#define ARDUINO_PIN_9           GPIO_PIN(PC, 21)
#define ARDUINO_PIN_10          GPIO_PIN(PA, 2)
#define ARDUINO_PIN_11          GPIO_PIN(PD, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PD, 8)
#define ARDUINO_PIN_13          GPIO_PIN(PB, 27)
#define ARDUINO_PIN_14          GPIO_PIN(PD, 4)
#define ARDUINO_PIN_15          GPIO_PIN(PD, 5)
#define ARDUINO_PIN_16          GPIO_PIN(PA, 13)
#define ARDUINO_PIN_17          GPIO_PIN(PA, 12)
#define ARDUINO_PIN_18          GPIO_PIN(PA, 11)
#define ARDUINO_PIN_19          GPIO_PIN(PA, 10)
#define ARDUINO_PIN_20          GPIO_PIN(PB, 12)
#define ARDUINO_PIN_21          GPIO_PIN(PB, 13)
#define ARDUINO_PIN_22          GPIO_PIN(PB, 26)
#define ARDUINO_PIN_23          GPIO_PIN(PA, 14)
#define ARDUINO_PIN_24          GPIO_PIN(PA, 15)
#define ARDUINO_PIN_25          GPIO_PIN(PD, 0)
#define ARDUINO_PIN_26          GPIO_PIN(PD, 1)
#define ARDUINO_PIN_27          GPIO_PIN(PD, 2)
#define ARDUINO_PIN_28          GPIO_PIN(PD, 3)
#define ARDUINO_PIN_29          GPIO_PIN(PD, 6)
#define ARDUINO_PIN_30          GPIO_PIN(PD, 9)
#define ARDUINO_PIN_31          GPIO_PIN(PA, 7)
#define ARDUINO_PIN_32          GPIO_PIN(PD, 10)
#define ARDUINO_PIN_33          GPIO_PIN(PC, 1)
#define ARDUINO_PIN_34          GPIO_PIN(PC, 2)
#define ARDUINO_PIN_35          GPIO_PIN(PC, 3)
#define ARDUINO_PIN_36          GPIO_PIN(PC, 4)
#define ARDUINO_PIN_37          GPIO_PIN(PC, 5)
#define ARDUINO_PIN_38          GPIO_PIN(PC, 6)
#define ARDUINO_PIN_39          GPIO_PIN(PC, 7)
#define ARDUINO_PIN_40          GPIO_PIN(PC, 8)
#define ARDUINO_PIN_41          GPIO_PIN(PC, 9)
#define ARDUINO_PIN_42          GPIO_PIN(PA, 19)
#define ARDUINO_PIN_43          GPIO_PIN(PA, 20)
#define ARDUINO_PIN_44          GPIO_PIN(PC, 19)
#define ARDUINO_PIN_45          GPIO_PIN(PC, 18)
#define ARDUINO_PIN_46          GPIO_PIN(PC, 17)
#define ARDUINO_PIN_47          GPIO_PIN(PC, 16)
#define ARDUINO_PIN_48          GPIO_PIN(PC, 15)
#define ARDUINO_PIN_49          GPIO_PIN(PC, 14)
#define ARDUINO_PIN_50          GPIO_PIN(PC, 13)
#define ARDUINO_PIN_51          GPIO_PIN(PC, 12)
#define ARDUINO_PIN_52          GPIO_PIN(PB, 21)
#define ARDUINO_PIN_53          GPIO_PIN(PB, 14)
#define ARDUINO_PIN_54          GPIO_PIN(PA, 16)
#define ARDUINO_PIN_55          GPIO_PIN(PA, 24)
#define ARDUINO_PIN_56          GPIO_PIN(PA, 23)
#define ARDUINO_PIN_57          GPIO_PIN(PA, 22)
#define ARDUINO_PIN_58          GPIO_PIN(PA, 6)
#define ARDUINO_PIN_59          GPIO_PIN(PA, 4)
#define ARDUINO_PIN_60          GPIO_PIN(PA, 3)
#define ARDUINO_PIN_61          GPIO_PIN(PA, 2)
#define ARDUINO_PIN_62          GPIO_PIN(PB, 17)
#define ARDUINO_PIN_63          GPIO_PIN(PB, 18)
#define ARDUINO_PIN_64          GPIO_PIN(PB, 19)
#define ARDUINO_PIN_65          GPIO_PIN(PB, 20)
#define ARDUINO_PIN_66          GPIO_PIN(PB, 15)
#define ARDUINO_PIN_67          GPIO_PIN(PB, 16)
#define ARDUINO_PIN_68          GPIO_PIN(PA, 1)
#define ARDUINO_PIN_69          GPIO_PIN(PA, 0)
#define ARDUINO_PIN_70          GPIO_PIN(PA, 17)
#define ARDUINO_PIN_71          GPIO_PIN(PA, 18)
#define ARDUINO_PIN_72          GPIO_PIN(PC, 30)
#define ARDUINO_PIN_73          GPIO_PIN(PA, 21)
#define ARDUINO_PIN_74          GPIO_PIN(PA, 25)
#define ARDUINO_PIN_75          GPIO_PIN(PA, 26)
#define ARDUINO_PIN_76          GPIO_PIN(PA, 27)
#define ARDUINO_PIN_77          GPIO_PIN(PA, 28)
#define ARDUINO_PIN_78          GPIO_PIN(PB, 23)
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
