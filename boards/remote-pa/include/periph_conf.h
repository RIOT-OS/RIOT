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
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_0_EN                1
#define I2C_IRQ_PRIO            1

/* I2C 0 device configuration */
#define I2C_0_DEV               0
#define I2C_0_IRQ               I2C_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c

static const i2c_conf_t i2c_config[] = {
    {
        .scl_pin = GPIO_PIN(1, 1),  /**< GPIO_PB1 */
        .sda_pin = GPIO_PIN(1, 0)   /**< GPIO_PB0 */
    },
};

#define I2C_NUMOF               (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

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

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SSI0,
        .mosi_pin = GPIO_PIN(3, 0),     /**< GPIO_PD0 */
        .miso_pin = GPIO_PIN(2, 4),     /**< GPIO_PC4 */
        .sck_pin  = GPIO_PIN(3, 1),     /**< GPIO_PD1 */
        .cs_pin   = GPIO_PIN(3, 3)      /**< GPIO_PD3 */
    },
    {
        .dev      = SSI1,
        .mosi_pin = GPIO_PIN(2, 7),     /**< GPIO_PC7 */
        .miso_pin = GPIO_PIN(0, 4),     /**< GPIO_PA4 */
        .sck_pin  = GPIO_PIN(1, 5),     /**< GPIO_PB5 */
        .cs_pin   = GPIO_UNDEF
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
