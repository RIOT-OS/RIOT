/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_remote-revb
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the RE-Mote board revision B
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cc2538_gpio.h"
#include "periph_cpu.h"
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
/**
 * @brief   Pre-calculated clock divider values based on a CLOCK_CORECLOCK (32MHz)
 *
 * Calculated with (CPSR * (SCR + 1)) = (CLOCK_CORECLOCK / bus_freq), where
 * 1 < CPSR < 255 and
 * 0 < SCR  < 256
 */
static const spi_clk_conf_t spi_clk_config[] = {
    { .cpsr = 10, .scr = 31 },  /* 100khz */
    { .cpsr =  2, .scr = 39 },  /* 400khz */
    { .cpsr =  2, .scr = 15 },  /* 1MHz */
    { .cpsr =  2, .scr =  2 },  /* ~4.5MHz */
    { .cpsr =  2, .scr =  1 }   /* ~10.7MHz */
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SSI0,
        .mosi_pin = GPIO_PB1,
        .miso_pin = GPIO_PB3,
        .sck_pin  = GPIO_PB2,
        .cs_pin   = GPIO_PB5
    },
    {
        .dev      = SSI1,
        .mosi_pin = GPIO_PC5,
        .miso_pin = GPIO_PC6,
        .sck_pin  = GPIO_PC4,
        .cs_pin   = GPIO_PA7
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
