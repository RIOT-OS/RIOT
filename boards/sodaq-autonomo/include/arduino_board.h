/*
 * Copyright (C)  2019 Kees Bakker
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-autonomo
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
#include "periph/pwm.h"

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
    GPIO_PIN(PA, 9),
    GPIO_PIN(PA, 10),

    /* 2..15 Digital */
    GPIO_PIN(PA, 11),
    GPIO_PIN(PB, 10),
    GPIO_PIN(PB, 11),
    GPIO_PIN(PB, 12),
    GPIO_PIN(PB, 13),
    GPIO_PIN(PB, 14),
    GPIO_PIN(PB, 15),
    GPIO_PIN(PA, 14),
    GPIO_PIN(PA, 15),
    GPIO_PIN(PA, 16),
    GPIO_PIN(PA, 17),
    GPIO_PIN(PA, 18),
    GPIO_PIN(PA, 19),
    GPIO_PIN(PB, 16),

    /* 16..18 Other Digital */
    GPIO_PIN(PA, 8),
    GPIO_PIN(PA, 28),
    GPIO_PIN(PB, 17),

    /* 19..32 A0..A13 */
    GPIO_PIN(PA,  2),
    GPIO_PIN(PA,  6),
    GPIO_PIN(PA,  5),
    GPIO_PIN(PA,  4),
    GPIO_PIN(PB,  9),
    GPIO_PIN(PB,  8),
    GPIO_PIN(PB,  7),
    GPIO_PIN(PB,  6),
    GPIO_PIN(PB,  5),
    GPIO_PIN(PB,  4),
    GPIO_PIN(PA,  7),
    GPIO_PIN(PB,  3),
    GPIO_PIN(PB,  2),
    GPIO_PIN(PB,  1),

    /* 33-35 Other Analog + DAC */
    GPIO_PIN(PB,  0),
    GPIO_PIN(PA,  3),
    GPIO_PIN(PA,  2),

    /* 36..39 - SERCOM/UART (Serial1) */
    GPIO_PIN(PB, 30),
    GPIO_PIN(PB, 31),
    GPIO_PIN(PB, 22),
    GPIO_PIN(PB, 23),

    /* 40..41 - I2C pins (SDA/SCL) */
    GPIO_PIN(PA, 12),
    GPIO_PIN(PA, 13),

    /* 42..45 - SPI pins (ICSP: MISO, SS, MOSI, SCK) */
    GPIO_PIN(PA, 22),
    GPIO_PIN(PA, 23),
    GPIO_PIN(PA, 20),
    GPIO_PIN(PA, 21),

    /* 46 - SD CARD CS */
    GPIO_PIN(PA, 27),

    /* 47..48 - USB */
    GPIO_PIN(PA, 24),
    GPIO_PIN(PA, 25),

    /* 49..50 - Serial2 (alternative use for D6/D7) */
    GPIO_PIN(PB, 13),
    GPIO_PIN(PB, 14),

    /* 51..52 - Serial3 (alternative use for D12/D13) */
    GPIO_PIN(PA, 17),
    GPIO_PIN(PA, 18),

    /* 53..56 - SPI1 (alternative use for D5..D8) */
    GPIO_PIN(PB, 12),
    GPIO_PIN(PB, 13),
    GPIO_PIN(PB, 14),
    GPIO_PIN(PB, 15),
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
    ADC_LINE(11),
    ADC_LINE(12),
    ADC_LINE(13),
    ADC_LINE(14),
};

/**
 * @brief   PWM frequency
 */
#define ARDUINO_PWM_FREQU       (732U)

/**
 * @brief   List of PWM GPIO mappings
 */
static const arduino_pwm_t arduino_pwm_list[] = {
    { .pin = 20, .dev = PWM_DEV(0), .chan = 0 },
    { .pin = 29, .dev = PWM_DEV(0), .chan = 1 },
    { .pin = 11, .dev = PWM_DEV(1), .chan = 0 },
    { .pin = 13, .dev = PWM_DEV(1), .chan = 1 },
    { .pin = 14, .dev = PWM_DEV(1), .chan = 2 },
};

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
