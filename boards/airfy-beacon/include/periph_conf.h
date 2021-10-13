/*
 * Copyright (C) 2014 Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_airfy-beacon
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Airfy Beacon board
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_16_1.h"
#include "cfg_timer_012.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    UART configuration
 *
 *          The CPU only supports one UART device, so we keep it simple
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_PIN_RX         17
#define UART_PIN_TX         18
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = 15,
        .mosi = 13,
        .miso = 14
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C (TWI) configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev     = NRF_TWI0,
        .pin_scl = 7,
        .pin_sda = 8,
        .ppi     = 0,
        .speed   = I2C_SPEED_NORMAL,
    },
    {
        .dev     = NRF_TWI1,
        .pin_scl = 9,
        .pin_sda = 10,
        .ppi     = 1,
        .speed   = I2C_SPEED_NORMAL,
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    ADC configuration
 *
 * The configuration consists simply of a list of channels that should be used
 * @{
 */
static const adc_conf_t adc_config[] = {3, 4, 5, 6};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
