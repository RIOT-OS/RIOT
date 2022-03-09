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
#define ARDUINO_PIN_5           GPIO_PIN(PA, 15) /**< D5 */
#define ARDUINO_PIN_6           GPIO_PIN(PA, 20) /**< D6 */
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
#define ARDUINO_PIN_20          GPIO_PIN(PA, 22) /**< D20 (I2C SDA) */
#define ARDUINO_PIN_21          GPIO_PIN(PA, 23) /**< D21 (I2C SCL) */
#define ARDUINO_PIN_22          GPIO_PIN(PA, 12) /**< D22 (SPI MISO) */
#define ARDUINO_PIN_23          GPIO_PIN(PB, 10) /**< D23 (SPI MOSI) */
#define ARDUINO_PIN_24          GPIO_PIN(PB, 11) /**< D24 (SPI SCK) */

#if defined(BOARD_FEATHER_M0_WIFI)
#define ARDUINO_PIN_2           GPIO_PIN(PA, 14) /**< D2 (WINC_CHIP_EN) */
#define ARDUINO_PIN_3           GPIO_UNDEF       /**< D3 */
#define ARDUINO_PIN_4           GPIO_PIN(PA, 8)  /**< D4 (WINC_RST) */
#define ARDUINO_PIN_7           GPIO_PIN(PA, 21) /**< D7 (WINC_IRQ) */
#define ARDUINO_PIN_8           GPIO_PIN(PA, 6)  /**< D8 (WINC_CS) */
#elif defined(BOARD_FEATHER_M0_LORA)
#define ARDUINO_PIN_2           GPIO_UNDEF       /**< D2 */
#define ARDUINO_PIN_3           GPIO_PIN(PA, 9)  /**< D3 (LORA_IRQ) */
#define ARDUINO_PIN_4           GPIO_PIN(PA, 8)  /**< D4 (LORA_RST) */
#define ARDUINO_PIN_7           GPIO_UNDEF       /**< D7 */
#define ARDUINO_PIN_8           GPIO_PIN(PA, 6)  /**< D8 (LORA_CS) */
#else
#define ARDUINO_PIN_2           GPIO_UNDEF       /**< D2 */
#define ARDUINO_PIN_3           GPIO_UNDEF       /**< D3 */
#define ARDUINO_PIN_4           GPIO_UNDEF       /**< D4 */
#define ARDUINO_PIN_7           GPIO_UNDEF       /**< D7 */
#define ARDUINO_PIN_8           GPIO_UNDEF       /**< D8 */
#endif

#define ARDUINO_PIN_A0          ARDUINO_PIN_14   /**< A0 */
#define ARDUINO_PIN_A1          ARDUINO_PIN_15   /**< A1 */
#define ARDUINO_PIN_A2          ARDUINO_PIN_16   /**< A2 */
#define ARDUINO_PIN_A3          ARDUINO_PIN_17   /**< A3 */
#define ARDUINO_PIN_A4          ARDUINO_PIN_18   /**< A4 */
#define ARDUINO_PIN_A5          ARDUINO_PIN_19   /**< A5 */
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
#define ARDUINO_A7              ADC_LINE(7)  /**< ADC 7 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */
