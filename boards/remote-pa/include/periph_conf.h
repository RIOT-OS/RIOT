/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_remote-pa
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Re-Mote board prototype A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clk_default.h"
#include "cfg_timer_default.h"
#include "cfg_uart_default.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_IRQ_PRIO            1

static const i2c_conf_t i2c_config[] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = GPIO_PIN(1, 1),  /**< GPIO_PB1 */
        .sda_pin = GPIO_PIN(1, 0)   /**< GPIO_PB0 */
    },
};

#define I2C_NUMOF               ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .num      = 0,
        .mosi_pin = GPIO_PIN(3, 0),
        .miso_pin = GPIO_PIN(2, 4),
        .sck_pin  = GPIO_PIN(3, 1),
        .cs_pin   = GPIO_PIN(3, 3)
    },
    {
        .num      = 1,
        .mosi_pin = GPIO_PIN(2, 7),
        .miso_pin = GPIO_PIN(0, 4),
        .sck_pin  = GPIO_PIN(1 ,5),
        .cs_pin   = SPI_CS_UNDEF,
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define SOC_ADC_ADCCON3_EREF  SOC_ADC_ADCCON3_EREF_AVDD5

static const adc_conf_t adc_config[] = {
    GPIO_PIN(0, 6), /**< GPIO_PA6 = ADC2_PIN */
    GPIO_PIN(0, 7), /**< GPIO_PA7 = ADC3_PIN */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
