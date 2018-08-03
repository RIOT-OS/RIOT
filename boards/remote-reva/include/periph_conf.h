/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_remote-reva
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the RE-Mote board revision A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name I2C configuration
 * @{
 */
#define I2C_IRQ_PRIO            1

static const i2c_conf_t i2c_config[] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = GPIO_PIN(2, 3),  /**< GPIO_PC3 */
        .sda_pin = GPIO_PIN(2, 2)   /**< GPIO_PC2 */
    },
};

#define I2C_NUMOF               (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .num      = 0,
        .mosi_pin = GPIO_PIN(1, 1),
        .miso_pin = GPIO_PIN(1, 3),
        .sck_pin  = GPIO_PIN(1, 2),
        .cs_pin   = GPIO_PIN(1, 5)
    },
    {
        .num      = 1,
        .mosi_pin = GPIO_PIN(2, 5),
        .miso_pin = GPIO_PIN(2, 6),
        .sck_pin  = GPIO_PIN(2, 4),
        .cs_pin   = GPIO_PIN(0, 7)
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define SOC_ADC_ADCCON_REF  SOC_ADC_ADCCON_REF_AVDD5

static const adc_conf_t adc_config[] = {
    GPIO_PIN(0, 5), /**< GPIO_PA5 = ADC1_PIN */
    GPIO_PIN(0, 4), /**< GPIO_PA4 = ADC2_PIN */
    /* voltage divider with 5/3 relationship to allow 5V sensors */
    GPIO_PIN(0, 2), /**< GPIO_PA2 = ADC3_PIN */
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
/** @} */



#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
