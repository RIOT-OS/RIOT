/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qn9080dk
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for QN9080DK base board board
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "macros/units.h"
#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_CORECLOCK
/* Using 32MHz internal oscillator as default clock source */
#define CLOCK_CORECLOCK     MHZ(32)     /**< System core clock frequency in Hz */
#endif
/** @} */

/**
 * @name ADC configuration
 *
 * Names "An" are as described in the "Analog In" header on the PCB. All "An"
 * inputs are configured referenced to 1.8V.
 * @{
 */
static const adc_conf_t adc_config[] = {
    ADC_CHANNEL_ADC7_VINN | ADC_VINN_AVSS | ADC_VREF_GAIN_X15, /* A0 */
    ADC_CHANNEL_ADC6_VINN | ADC_VINN_AVSS | ADC_VREF_GAIN_X15, /* A1 */
    ADC_CHANNEL_ADC4_VINN | ADC_VINN_AVSS | ADC_VREF_GAIN_X15, /* A2 */
    ADC_CHANNEL_ADC5_VINN | ADC_VINN_AVSS | ADC_VREF_GAIN_X15, /* A3 */
    ADC_CHANNEL_ADC1_VINN | ADC_VINN_AVSS | ADC_VREF_GAIN_X15, /* A4 */
    ADC_CHANNEL_ADC0_VINN | ADC_VINN_AVSS | ADC_VREF_GAIN_X15, /* A5 */
    ADC_CHANNEL_ADC6_ADC7 | ADC_VREF_GAIN_X15,                 /* A1-A0 */
    ADC_CHANNEL_ADC4_ADC5 | ADC_VREF_GAIN_X15,                 /* A2-A3 */
    ADC_CHANNEL_ADC0_ADC1 | ADC_VREF_GAIN_X15,                 /* A5-A4 */
    ADC_CHANNEL_TEMP,                                          /* temperature */
    ADC_CHANNEL_VCC4_VINN | ADC_VINN_AVSS,                     /* Vcc/4 */
};
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev          = I2C0,   /* Flexcomm 1 */
        .pin_scl      = GPIO_PIN(PORT_A, 6),
        .pin_sda      = GPIO_PIN(PORT_A, 7),
        .speed        = I2C_SPEED_FAST,
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev            = SPI0,  /* Flexcomm 2 */
        .cipo_pin       = GPIO_PIN(PORT_A, 5),
        .copi_pin       = GPIO_PIN(PORT_A, 4),
        .clk_pin        = GPIO_PIN(PORT_A, 30),
        .cs_pin         = {
            GPIO_PIN(PORT_A, 3),  /* MX25R2035F CS# connected here. */
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev          = USART0, /* Flexcomm 0 */
        .rx_pin       = GPIO_PIN(PORT_A, 17),
        .tx_pin       = GPIO_PIN(PORT_A, 16),
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
#define TIMER_NUMOF           4
/** @} */

/* put here the board peripherals definitions:
   - Available clocks
   - PWMs
   - SPIs
   - ADC
   - RTC
   - RTT
   etc
 */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
