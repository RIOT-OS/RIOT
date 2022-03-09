/*
 * Copyright (C)  2020 Kees Bakker
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-sara-sff
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
 * @brief   The green of the RGB led is used as "the builtin led"
 */
#define ARDUINO_LED         (15)

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
    /* 0..3 Main IO Pins (D0-D3) Digital Properties */
    GPIO_PIN(PA, 2),
    GPIO_PIN(PA, 3),
    GPIO_PIN(PB, 8),
    GPIO_PIN(PB, 9),

    /* 4..5 Other Digital Pins */
    GPIO_PIN(PA, 21),
    GPIO_PIN(PA, 20),

    /* 6..13 Main IO Pins (D6-D13) Digital Properties */
    GPIO_PIN(PA, 6),
    GPIO_PIN(PA, 7),
    GPIO_PIN(PA, 8),
    GPIO_PIN(PA, 9),
    GPIO_PIN(PA, 10),
    GPIO_PIN(PA, 11),
    GPIO_PIN(PB, 2),
    GPIO_PIN(PB, 3),

    /* 14..21 Other Digital Pins */
    GPIO_PIN(PA, 15),
    GPIO_PIN(PB, 10),
    GPIO_PIN(PB, 11),
    GPIO_PIN(PA, 14),
    GPIO_PIN(PA, 18),
    GPIO_PIN(PA, 16),
    GPIO_PIN(PB, 22),
    GPIO_PIN(PA, 17),

    /* 22..33 Main IO Pins Analog Properties */
    GPIO_PIN(PA, 2),
    GPIO_PIN(PA, 3),
    GPIO_PIN(PB, 8),
    GPIO_PIN(PB, 9),
    GPIO_PIN(PA, 6),
    GPIO_PIN(PA, 7),
    GPIO_PIN(PA, 8),
    GPIO_PIN(PA, 9),
    GPIO_PIN(PA, 10),
    GPIO_PIN(PA, 11),
    GPIO_PIN(PB, 2),
    GPIO_PIN(PB, 3),

    /* 34..36 Other Analog Pins */
    GPIO_PIN(PA, 2),
    GPIO_PIN(PA, 3),
    GPIO_PIN(PA, 5),

    /* 37..38 USB Pins */
    GPIO_PIN(PA, 24),
    GPIO_PIN(PA, 25),

    /* 39..40 Serial */
    GPIO_PIN(PB, 3),
    GPIO_PIN(PB, 2),

    /* 41..42 Serial1 */
    GPIO_PIN(PA, 13),
    GPIO_PIN(PA, 12),

    /* 43..46 SPI  */
    GPIO_PIN(PA, 8),
    GPIO_PIN(PA, 9),
    GPIO_PIN(PA, 10),
    GPIO_PIN(PA, 11),

    /* 47..48 I2C */
    GPIO_PIN(PA, 22),
    GPIO_PIN(PA, 23),

    /* 49 */
    GPIO_PIN(PA,  4),

    /* 50 MAG_INT */
    GPIO_PIN(PA, 19),

    /* 51 SARA_TX_ENABLE */
    GPIO_PIN(PA, 27),
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
    ADC_LINE(10),
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
