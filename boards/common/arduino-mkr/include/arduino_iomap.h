/*
 * Copyright (C)  2016-2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_arduino-mkr
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is connected to pin D6 on Arduino MKR derivatives
 */
#define ARDUINO_LED         (6U)

/**
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0       GPIO_PIN(PA, 22)    /**< TC4-W0 */
#define ARDUINO_PIN_1       GPIO_PIN(PA, 23)    /**< TC4-W1 */
#define ARDUINO_PIN_2       GPIO_PIN(PA, 10)    /**< TCC0-W2 */
#define ARDUINO_PIN_3       GPIO_PIN(PA, 11)    /**< TCC0-W3 */
#define ARDUINO_PIN_4       GPIO_PIN(PB, 10)    /**< TCC0-W4 */
#define ARDUINO_PIN_5       GPIO_PIN(PB, 11)    /**< TCC0-W5 */
#define ARDUINO_PIN_6       GPIO_PIN(PA, 20)    /**< TCC0-W6, on-board LED */
#define ARDUINO_PIN_7       GPIO_PIN(PA, 21)    /**< TCC0-W7 */

#define ARDUINO_PIN_8       GPIO_PIN(PA, 16)    /**< SERCOM1-MOSI */
#define ARDUINO_PIN_9       GPIO_PIN(PA, 17)    /**< SERCOM1-SCK */
#define ARDUINO_PIN_10      GPIO_PIN(PA, 19)    /**< SERCOM1-MISO */
#define ARDUINO_PIN_11      GPIO_PIN(PA, 8)     /**< SERCOM0-SDA, on-board pull-up */
#define ARDUINO_PIN_12      GPIO_PIN(PA, 9)     /**< SERCOM0-SCL, on-board pull-up */
#define ARDUINO_PIN_13      GPIO_PIN(PB, 23)    /**< SERCOM5-RX from MCU */
#define ARDUINO_PIN_14      GPIO_PIN(PB, 22)    /**< SERCOM5-TX from MCU */

/* analog pins as digital pin: */
#define ARDUINO_PIN_15      GPIO_PIN(PA, 2)     /**< AIN0, DAC0 */
#define ARDUINO_PIN_16      GPIO_PIN(PB, 2)     /**< AIN10 */
#define ARDUINO_PIN_17      GPIO_PIN(PB, 3)     /**< AIN11 */
#define ARDUINO_PIN_18      GPIO_PIN(PA, 4)     /**< AIN4 */
#define ARDUINO_PIN_19      GPIO_PIN(PA, 5)     /**< AIN5 */
#define ARDUINO_PIN_20      GPIO_PIN(PA, 6)     /**< AIN6 */
#define ARDUINO_PIN_21      GPIO_PIN(PA, 7)     /**< AIN7 */

#define ARDUINO_PIN_LAST        21
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0      ARDUINO_PIN_15
#define ARDUINO_PIN_A1      ARDUINO_PIN_16
#define ARDUINO_PIN_A2      ARDUINO_PIN_17
#define ARDUINO_PIN_A3      ARDUINO_PIN_18
#define ARDUINO_PIN_A4      ARDUINO_PIN_19
#define ARDUINO_PIN_A5      ARDUINO_PIN_20
#define ARDUINO_PIN_A6      ARDUINO_PIN_21
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)
#define ARDUINO_A1              ADC_LINE(1)
#define ARDUINO_A2              ADC_LINE(2)
#define ARDUINO_A3              ADC_LINE(3)
#define ARDUINO_A4              ADC_LINE(4)
#define ARDUINO_A5              ADC_LINE(5)
#define ARDUINO_A6              ADC_LINE(6)

#define ARDUINO_ANALOG_PIN_LAST 6
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM dev and channel pairs
 * @{
 */
/**
 * @brief   PWM frequency
 */
#define ARDUINO_PWM_FREQU       (732U)

#define ARDUINO_PIN_2_PWM_DEV   PWM_DEV(0)
#define ARDUINO_PIN_2_PWM_CHAN  0

#define ARDUINO_PIN_3_PWM_DEV   PWM_DEV(0)
#define ARDUINO_PIN_3_PWM_CHAN  1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_IOMAP_H */
/** @} */
