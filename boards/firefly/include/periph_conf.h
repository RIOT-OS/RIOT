/*
 * Copyright (C) 2017 DAI Labor Technische Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_firefly
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Firefly  board revision A
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
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
#define I2C_NUMOF               1
#define I2C_0_EN                1
#define I2C_IRQ_PRIO            1

/* I2C 0 device configuration */
#define I2C_0_DEV               0
#define I2C_0_IRQ               I2C_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c
#define I2C_0_SCL_PIN           GPIO_PIN(PORT_C, 3)
#define I2C_0_SDA_PIN           GPIO_PIN(PORT_C, 2)

static const i2c_conf_t i2c_config[I2C_NUMOF] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = I2C_0_SCL_PIN,
        .sda_pin = I2C_0_SDA_PIN,
    },
};
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .num      = 0,
        .mosi_pin = GPIO_PIN(PORT_B, 1),
        .miso_pin = GPIO_PIN(PORT_B, 3),
        .sck_pin  = GPIO_PIN(PORT_B, 2),
        .cs_pin   = GPIO_UNDEF
    },
    {
        .num      = 1,
        .mosi_pin = GPIO_PIN(PORT_C, 5),
        .miso_pin = GPIO_PIN(PORT_C, 6),
        .sck_pin  = GPIO_PIN(PORT_C, 4),
        .cs_pin   = GPIO_PIN(PORT_A, 7)
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define SOC_ADC_ADCCON3_EREF  SOC_ADC_ADCCON3_EREF_AVDD5

static const adc_conf_t adc_config[] = {
    GPIO_PIN(PORT_A, 5),    /**< GPIO_PA5 = ADC1_PIN */
    GPIO_PIN(PORT_A, 4),    /**< GPIO_PA4 = ADC2_PIN */
    /* voltage divider with 5/3 relationship to allow 5V sensors */
    GPIO_PIN(PORT_A, 2),    /**< GPIO_PA2 = ADC3_PIN */
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
/** @} */


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
