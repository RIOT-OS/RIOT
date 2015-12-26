/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mega2560
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
 *
 * @note    Some pins (e.g. 10, 11) are not mapped, as they are not usable as
 *          GPIO, as they are pins for voltage supply, reset, etc.
 */
#define ARDUINO_PIN_1           GPIO_PIN(PORT_G, 5)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_E, 0)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_E, 1)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_E, 2)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_E, 3)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_E, 4)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_E, 5)
#define ARDUINO_PIN_8           GPIO_PIN(PORT_E, 6)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_E, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_H, 0)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_H, 1)
#define ARDUINO_PIN_14          GPIO_PIN(PORT_H, 2)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_H, 3)
#define ARDUINO_PIN_16          GPIO_PIN(PORT_H, 4)
#define ARDUINO_PIN_17          GPIO_PIN(PORT_H, 5)
#define ARDUINO_PIN_18          GPIO_PIN(PORT_H, 6)
#define ARDUINO_PIN_19          GPIO_PIN(PORT_B, 0)
#define ARDUINO_PIN_20          GPIO_PIN(PORT_B, 1)
#define ARDUINO_PIN_21          GPIO_PIN(PORT_B, 2)
#define ARDUINO_PIN_22          GPIO_PIN(PORT_B, 3)
#define ARDUINO_PIN_23          GPIO_PIN(PORT_B, 4)
#define ARDUINO_PIN_24          GPIO_PIN(PORT_B, 5)
#define ARDUINO_PIN_25          GPIO_PIN(PORT_B, 6)
#define ARDUINO_PIN_26          GPIO_PIN(PORT_B, 7)
#define ARDUINO_PIN_27          GPIO_PIN(PORT_H, 7)
#define ARDUINO_PIN_28          GPIO_PIN(PORT_G, 3)
#define ARDUINO_PIN_29          GPIO_PIN(PORT_G, 4)
#define ARDUINO_PIN_35          GPIO_PIN(PORT_L, 0)
#define ARDUINO_PIN_36          GPIO_PIN(PORT_L, 1)
#define ARDUINO_PIN_37          GPIO_PIN(PORT_L, 2)
#define ARDUINO_PIN_38          GPIO_PIN(PORT_L, 3)
#define ARDUINO_PIN_39          GPIO_PIN(PORT_L, 4)
#define ARDUINO_PIN_40          GPIO_PIN(PORT_L, 5)
#define ARDUINO_PIN_41          GPIO_PIN(PORT_L, 6)
#define ARDUINO_PIN_42          GPIO_PIN(PORT_L, 7)
#define ARDUINO_PIN_43          GPIO_PIN(PORT_D, 0)
#define ARDUINO_PIN_44          GPIO_PIN(PORT_D, 1)
#define ARDUINO_PIN_45          GPIO_PIN(PORT_D, 2)
#define ARDUINO_PIN_46          GPIO_PIN(PORT_D, 3)
#define ARDUINO_PIN_47          GPIO_PIN(PORT_D, 4)
#define ARDUINO_PIN_48          GPIO_PIN(PORT_D, 5)
#define ARDUINO_PIN_49          GPIO_PIN(PORT_D, 6)
#define ARDUINO_PIN_50          GPIO_PIN(PORT_D, 7)
#define ARDUINO_PIN_51          GPIO_PIN(PORT_G, 0)
#define ARDUINO_PIN_52          GPIO_PIN(PORT_G, 1)
#define ARDUINO_PIN_53          GPIO_PIN(PORT_C, 0)
#define ARDUINO_PIN_54          GPIO_PIN(PORT_C, 1)
#define ARDUINO_PIN_55          GPIO_PIN(PORT_C, 2)
#define ARDUINO_PIN_56          GPIO_PIN(PORT_C, 3)
#define ARDUINO_PIN_57          GPIO_PIN(PORT_C, 4)
#define ARDUINO_PIN_58          GPIO_PIN(PORT_C, 5)
#define ARDUINO_PIN_59          GPIO_PIN(PORT_C, 6)
#define ARDUINO_PIN_60          GPIO_PIN(PORT_C, 7)
#define ARDUINO_PIN_63          GPIO_PIN(PORT_J, 0)
#define ARDUINO_PIN_64          GPIO_PIN(PORT_J, 1)
#define ARDUINO_PIN_65          GPIO_PIN(PORT_J, 2)
#define ARDUINO_PIN_66          GPIO_PIN(PORT_J, 3)
#define ARDUINO_PIN_67          GPIO_PIN(PORT_J, 4)
#define ARDUINO_PIN_68          GPIO_PIN(PORT_J, 5)
#define ARDUINO_PIN_69          GPIO_PIN(PORT_J, 6)
#define ARDUINO_PIN_70          GPIO_PIN(PORT_G, 2)
#define ARDUINO_PIN_71          GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_72          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_73          GPIO_PIN(PORT_A, 5)
#define ARDUINO_PIN_74          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_75          GPIO_PIN(PORT_A, 3)
#define ARDUINO_PIN_76          GPIO_PIN(PORT_A, 2)
#define ARDUINO_PIN_77          GPIO_PIN(PORT_A, 1)
#define ARDUINO_PIN_78          GPIO_PIN(PORT_A, 0)
#define ARDUINO_PIN_79          GPIO_PIN(PORT_J, 7)
#define ARDUINO_PIN_82          GPIO_PIN(PORT_K, 7)
#define ARDUINO_PIN_83          GPIO_PIN(PORT_K, 6)
#define ARDUINO_PIN_84          GPIO_PIN(PORT_K, 5)
#define ARDUINO_PIN_85          GPIO_PIN(PORT_K, 4)
#define ARDUINO_PIN_86          GPIO_PIN(PORT_K, 3)
#define ARDUINO_PIN_87          GPIO_PIN(PORT_K, 2)
#define ARDUINO_PIN_88          GPIO_PIN(PORT_K, 1)
#define ARDUINO_PIN_89          GPIO_PIN(PORT_K, 0)
#define ARDUINO_PIN_90          GPIO_PIN(PORT_F, 7)
#define ARDUINO_PIN_91          GPIO_PIN(PORT_F, 6)
#define ARDUINO_PIN_92          GPIO_PIN(PORT_F, 5)
#define ARDUINO_PIN_93          GPIO_PIN(PORT_F, 4)
#define ARDUINO_PIN_94          GPIO_PIN(PORT_F, 3)
#define ARDUINO_PIN_95          GPIO_PIN(PORT_F, 2)
#define ARDUINO_PIN_96          GPIO_PIN(PORT_F, 1)
#define ARDUINO_PIN_97          GPIO_PIN(PORT_F, 0)
/** @ */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
