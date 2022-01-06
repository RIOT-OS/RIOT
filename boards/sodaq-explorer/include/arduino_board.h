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

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The builtin LED
 */
#define ARDUINO_LED         (13)

/**
 * @brief   On-board serial port mapping
 */
#ifndef ARDUINO_UART_DEV
#define ARDUINO_UART_DEV    UART_UNDEF
#endif

/**
 * @brief   Look-up table for the Arduino's digital pins
 */
static const gpio_t arduino_pinmap[] = {
    /* 0..1 - SERCOM/UART (Serial) */
    GPIO_PIN(PB, 31),
    GPIO_PIN(PB, 30),

    /* 2..15 Digital */
    GPIO_PIN(PA, 2),
    GPIO_PIN(PA, 3),
    GPIO_PIN(PB, 4),
    GPIO_PIN(PB, 6),
    GPIO_PIN(PB, 7),
    GPIO_PIN(PB, 8),
    GPIO_PIN(PB, 10),
    GPIO_PIN(PB, 11),
    GPIO_PIN(PA, 23),
    GPIO_PIN(PA, 20),
    GPIO_PIN(PA, 22),
    GPIO_PIN(PA, 21),
    GPIO_PIN(PA, 10),
    GPIO_PIN(PA, 11),

    /* 16..19 Other Digital */
    GPIO_PIN(PA, 12),
    GPIO_PIN(PB, 15),
    GPIO_PIN(PA, 13),
    GPIO_PIN(PA, 15),

    /* 20..28 A0..A8 */
    GPIO_PIN(PB,  0),
    GPIO_PIN(PB,  1),
    GPIO_PIN(PB,  2),
    GPIO_PIN(PB,  3),
    GPIO_PIN(PA,  8),
    GPIO_PIN(PA,  9),
    GPIO_PIN(PA,  4),
    GPIO_PIN(PA,  10),
    GPIO_PIN(PA,  11),

    /* 29..30 - SERCOM/UART (Serial1) */
    GPIO_PIN(PB, 14),
    GPIO_PIN(PB, 13),

    /* 31..32 - SERCOM/UART (Serial2) */
    GPIO_PIN(PA, 6),
    GPIO_PIN(PA, 5),

    /* 33..34 - I2C pins (SDA/SCL) */
    GPIO_PIN(PA, 16),
    GPIO_PIN(PA, 17),

    /* 35..36 - I2C1 pins (SDA/SCL) */
    GPIO_PIN(PA, 8),
    GPIO_PIN(PA, 9),

    /* 37..40 - SPI pins (ICSP: MISO, SS, MOSI, SCK) */
    /* Notice that SCK is sharing the same PA21 with LED_BUILTIN */
    GPIO_PIN(PA, 22),
    GPIO_PIN(PA, 23),
    GPIO_PIN(PA, 20),
    GPIO_PIN(PA, 21),

    /* 41..42 - USB */
    GPIO_PIN(PA, 24),
    GPIO_PIN(PA, 25),

    /* 43 DAC */
    GPIO_PIN(PA,  2),

    /* 44 Flash CS */
    GPIO_PIN(PB, 16),

    /* 45..46 LoRa, BT Reset */
    GPIO_PIN(PA,  7),
    GPIO_PIN(PB,  17),

    /* 47 Button */
    GPIO_PIN(PA,  14),

    /* 48 BAT_VOLT */
    GPIO_PIN(PB,  5),
};

/**
 * @brief   Look-up table for the Arduino's analog pins
 */
static const adc_t arduino_analog_map[] = {
    ADC_LINE(0),
    ADC_LINE(1),
    ADC_LINE(2),
    ADC_LINE(3),
    ADC_LINE(4),
    ADC_LINE(5),
    ADC_LINE(6),
    ADC_LINE(7),
    ADC_LINE(8),
    ADC_LINE(9),
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
