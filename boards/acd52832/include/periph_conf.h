/*
 * Copyright (C) 2016-2017 Freie Universität Berlin
 *                    2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_acd52832
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the ACD52832
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *
 * @{
 */
#define CLOCK_CORECLOCK     (64000000U)     /* fixed for all nRF52832 */
/** @} */

/**
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_PIN_RX         GPIO_PIN(0, 30)
#define UART_PIN_TX         GPIO_PIN(0, 31)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = 4,
        .mosi = 3,
        .miso = 13,
        .ppi = 0
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM1,
        .scl = 28,
        .sda = 29,
        .speed = I2C_SPEED_NORMAL
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
