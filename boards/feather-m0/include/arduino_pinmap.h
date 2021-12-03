/*
 * Copyright (C) 2021 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_feather-m0
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
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
 * @{
 */

#define ARDUINO_PIN_0           GPIO_PIN(PA, 11) /**< D0 (RX) */
#define ARDUINO_PIN_1           GPIO_PIN(PA, 10) /**< D1 (TX) */
#define ARDUINO_PIN_2           GPIO_PIN(PA, 14) /**< D2 */
#define ARDUINO_PIN_3           GPIO_PIN(PA, 9)  /**< D3 */
#define ARDUINO_PIN_4           GPIO_PIN(PA, 8)  /**< D4 */
#define ARDUINO_PIN_5           GPIO_PIN(PA, 15) /**< D5 */
#define ARDUINO_PIN_6           GPIO_PIN(PA, 20) /**< D6 */
#define ARDUINO_PIN_7           GPIO_PIN(PA, 21) /**< D7 */
#define ARDUINO_PIN_8           GPIO_PIN(PA, 6)  /**< D8 */
#define ARDUINO_PIN_9           GPIO_PIN(PA, 7)  /**< D9 */
#define ARDUINO_PIN_10          GPIO_PIN(PA, 18) /**< D10 */
#define ARDUINO_PIN_11          GPIO_PIN(PA, 16) /**< D11 */
#define ARDUINO_PIN_12          GPIO_PIN(PA, 19) /**< D12 */
#define ARDUINO_PIN_13          GPIO_PIN(PA, 17) /**< D13 (LED) */
#define ARDUINO_PIN_14          GPIO_PIN(PA, 2)  /**< D14 (A0) */
#define ARDUINO_PIN_15          GPIO_PIN(PB, 8)  /**< D15 (A1) */
#define ARDUINO_PIN_16          GPIO_PIN(PB, 9)  /**< D16 (A2) */
#define ARDUINO_PIN_17          GPIO_PIN(PA, 4)  /**< D17 (A3) */
#define ARDUINO_PIN_18          GPIO_PIN(PA, 5)  /**< D18 (A4) */
#define ARDUINO_PIN_19          GPIO_PIN(PB, 2)  /**< D19 (A5) */
#define ARDUINO_PIN_20          GPIO_PIN(PA, 22) /**< D20 */
#define ARDUINO_PIN_21          GPIO_PIN(PA, 23) /**< D21 */
#define ARDUINO_PIN_22          GPIO_PIN(PA, 12) /**< D22 */
#define ARDUINO_PIN_23          GPIO_PIN(PB, 10) /**< D23 */
#define ARDUINO_PIN_24          GPIO_PIN(PB, 11) /**< D24 */
#define ARDUINO_PIN_25          GPIO_PIN(PB, 3)  /**< D25 */
#define ARDUINO_PIN_26          GPIO_PIN(PA, 27) /**< D26 */
#define ARDUINO_PIN_27          GPIO_PIN(PA, 28) /**< D27 */
#define ARDUINO_PIN_28          GPIO_PIN(PA, 24) /**< D28 */
#define ARDUINO_PIN_29          GPIO_PIN(PA, 25) /**< D29 */
#define ARDUINO_PIN_30          GPIO_PIN(PB, 22) /**< D30 */
#define ARDUINO_PIN_31          GPIO_PIN(PB, 23) /**< D31 */
#define ARDUINO_PIN_32          GPIO_PIN(PA, 22) /**< D32 */
#define ARDUINO_PIN_33          GPIO_PIN(PA, 23) /**< D33 */
#define ARDUINO_PIN_34          GPIO_PIN(PA, 19) /**< D34 */
#define ARDUINO_PIN_35          GPIO_PIN(PA, 16) /**< D35 */
#define ARDUINO_PIN_36          GPIO_PIN(PA, 18) /**< D36 */
#define ARDUINO_PIN_37          GPIO_PIN(PA, 17) /**< D37 */
#define ARDUINO_PIN_38          GPIO_PIN(PA, 13) /**< D38 */
#define ARDUINO_PIN_39          GPIO_PIN(PA, 21) /**< D39 */
#define ARDUINO_PIN_40          GPIO_PIN(PA, 6)  /**< D40 */
#define ARDUINO_PIN_41          GPIO_PIN(PA, 7)  /**< D41 */
#define ARDUINO_PIN_42          GPIO_PIN(PA, 3)  /**< D42 */
#define ARDUINO_PIN_43          GPIO_PIN(PA, 2)  /**< D43 */
#define ARDUINO_PIN_44          GPIO_PIN(PA, 6)  /**< D44 */
#define ARDUINO_PIN_45          GPIO_PIN(PA, 7)  /**< D45 */

#define ARDUINO_PIN_A0          ARDUINO_PIN_14   /**< A0 */
#define ARDUINO_PIN_A1          ARDUINO_PIN_15   /**< A1 */
#define ARDUINO_PIN_A2          ARDUINO_PIN_16   /**< A2 */
#define ARDUINO_PIN_A3          ARDUINO_PIN_17   /**< A3 */
#define ARDUINO_PIN_A4          ARDUINO_PIN_18   /**< A4 */
#define ARDUINO_PIN_A5          ARDUINO_PIN_19   /**< A5 */
#define ARDUINO_PIN_A6          ARDUINO_PIN_8    /**< A6 */
#define ARDUINO_PIN_A7          ARDUINO_PIN_9    /**< A7 */
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)  /**< ADC 0 */
#define ARDUINO_A1              ADC_LINE(2)  /**< ADC 1 */
#define ARDUINO_A2              ADC_LINE(3)  /**< ADC 2 */
#define ARDUINO_A3              ADC_LINE(4)  /**< ADC 3 */
#define ARDUINO_A4              ADC_LINE(5)  /**< ADC 4 */
#define ARDUINO_A5              ADC_LINE(10) /**< ADC 5 */
#define ARDUINO_A6              ADC_LINE(6)  /**< ADC 6 */
#define ARDUINO_A7              ADC_LINE(7)  /**< ADC 7 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
