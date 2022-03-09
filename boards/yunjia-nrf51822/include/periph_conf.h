/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_yunjia-nrf51822
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Yunjia NRF51822 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_16_0.h"
#include "cfg_timer_012.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
/* UART pin configuration */
#define UART_PIN_RX         1
#define UART_PIN_TX         2
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = 19,
        .mosi = 17,
        .miso = 18
    },
    {
        .dev  = NRF_SPI1,
        .sclk = 22,
        .mosi = 20,
        .miso = 21
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name   I2C (TWI) configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev     = NRF_TWI0,
        .pin_scl = 23,
        .pin_sda = 24,
        .ppi     = 0,
        .speed   = I2C_SPEED_NORMAL
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   ADC configuration
 *
 * The configuration consists simply of a list of channels that should be used
 * @{
 */
static const adc_conf_t adc_config[] = {4, 5, 6, 7};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
