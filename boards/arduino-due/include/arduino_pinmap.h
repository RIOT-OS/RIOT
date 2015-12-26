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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_A, 8)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_A, 9)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_B, 25)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_C, 28)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_A, 29)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_C, 25)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_C, 24)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_C, 23)
#define ARDUINO_PIN_8           GPIO_PIN(PORT_C, 22)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_C, 21)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_A, 2)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_D, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_D, 8)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_B, 27)
#define ARDUINO_PIN_14          GPIO_PIN(PORT_D, 4)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_D, 5)
#define ARDUINO_PIN_16          GPIO_PIN(PORT_A, 13)
#define ARDUINO_PIN_17          GPIO_PIN(PORT_A, 12)
#define ARDUINO_PIN_18          GPIO_PIN(PORT_A, 11)
#define ARDUINO_PIN_19          GPIO_PIN(PORT_A, 10)
#define ARDUINO_PIN_20          GPIO_PIN(PORT_B, 12)
#define ARDUINO_PIN_21          GPIO_PIN(PORT_B, 13)
#define ARDUINO_PIN_22          GPIO_PIN(PORT_B, 26)
#define ARDUINO_PIN_23          GPIO_PIN(PORT_A, 14)
#define ARDUINO_PIN_24          GPIO_PIN(PORT_A, 15)
#define ARDUINO_PIN_25          GPIO_PIN(PORT_D, 0)
#define ARDUINO_PIN_26          GPIO_PIN(PORT_D, 1)
#define ARDUINO_PIN_27          GPIO_PIN(PORT_D, 2)
#define ARDUINO_PIN_28          GPIO_PIN(PORT_D, 3)
#define ARDUINO_PIN_29          GPIO_PIN(PORT_D, 6)
#define ARDUINO_PIN_30          GPIO_PIN(PORT_D, 9)
#define ARDUINO_PIN_31          GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_32          GPIO_PIN(PORT_D, 10)
#define ARDUINO_PIN_33          GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_34          GPIO_PIN(PORT_C, 2)
#define ARDUINO_PIN_35          GPIO_PIN(PORT_C, 3)
#define ARDUINO_PIN_36          GPIO_PIN(PORT_C, 4)
#define ARDUINO_PIN_37          GPIO_PIN(PORT_C, 5)
#define ARDUINO_PIN_38          GPIO_PIN(PORT_C, 6)
#define ARDUINO_PIN_39          GPIO_PIN(PORT_C, 7)
#define ARDUINO_PIN_40          GPIO_PIN(PORT_C, 8)
#define ARDUINO_PIN_41          GPIO_PIN(PORT_C, 9)
#define ARDUINO_PIN_42          GPIO_PIN(PORT_A, 19)
#define ARDUINO_PIN_43          GPIO_PIN(PORT_A, 20)
#define ARDUINO_PIN_44          GPIO_PIN(PORT_C, 19)
#define ARDUINO_PIN_45          GPIO_PIN(PORT_C, 18)
#define ARDUINO_PIN_46          GPIO_PIN(PORT_C, 17)
#define ARDUINO_PIN_47          GPIO_PIN(PORT_C, 16)
#define ARDUINO_PIN_48          GPIO_PIN(PORT_C, 15)
#define ARDUINO_PIN_49          GPIO_PIN(PORT_C, 14)
#define ARDUINO_PIN_50          GPIO_PIN(PORT_C, 13)
#define ARDUINO_PIN_51          GPIO_PIN(PORT_C, 12)
#define ARDUINO_PIN_52          GPIO_PIN(PORT_B, 21)
#define ARDUINO_PIN_53          GPIO_PIN(PORT_B, 14)
#define ARDUINO_PIN_54          GPIO_PIN(PORT_A, 16)
#define ARDUINO_PIN_55          GPIO_PIN(PORT_A, 24)
#define ARDUINO_PIN_56          GPIO_PIN(PORT_A, 23)
#define ARDUINO_PIN_57          GPIO_PIN(PORT_A, 22)
#define ARDUINO_PIN_58          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_59          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_60          GPIO_PIN(PORT_A, 3)
#define ARDUINO_PIN_61          GPIO_PIN(PORT_A, 2)
#define ARDUINO_PIN_62          GPIO_PIN(PORT_B, 17)
#define ARDUINO_PIN_63          GPIO_PIN(PORT_B, 18)
#define ARDUINO_PIN_64          GPIO_PIN(PORT_B, 19)
#define ARDUINO_PIN_65          GPIO_PIN(PORT_B, 20)
#define ARDUINO_PIN_66          GPIO_PIN(PORT_B, 15)
#define ARDUINO_PIN_67          GPIO_PIN(PORT_B, 16)
#define ARDUINO_PIN_68          GPIO_PIN(PORT_A, 1)
#define ARDUINO_PIN_69          GPIO_PIN(PORT_A, 0)
#define ARDUINO_PIN_70          GPIO_PIN(PORT_A, 17)
#define ARDUINO_PIN_71          GPIO_PIN(PORT_A, 18)
#define ARDUINO_PIN_72          GPIO_PIN(PORT_C, 30)
#define ARDUINO_PIN_73          GPIO_PIN(PORT_A, 21)
#define ARDUINO_PIN_74          GPIO_PIN(PORT_A, 25)
#define ARDUINO_PIN_75          GPIO_PIN(PORT_A, 26)
#define ARDUINO_PIN_76          GPIO_PIN(PORT_A, 27)
#define ARDUINO_PIN_77          GPIO_PIN(PORT_A, 28)
#define ARDUINO_PIN_78          GPIO_PIN(PORT_B, 23)
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
