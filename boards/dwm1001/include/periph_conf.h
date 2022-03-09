/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_dwm1001
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the DWM1001 dev board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_PIN_RX         GPIO_PIN(0, 11)
#define UART_PIN_TX         GPIO_PIN(0, 5)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(0, 4),
        .mosi = GPIO_PIN(0, 6),
        .miso = GPIO_PIN(0, 7),
        .ppi = 0,
    },
    {   /* Connected to the DWM1001 UWB transceiver */
        .dev  = NRF_SPIM1,
        .sclk = GPIO_PIN(0, 16),
        .mosi = GPIO_PIN(0, 20),
        .miso = GPIO_PIN(0, 18),
        .ppi = 0,
    },
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
        .scl = GPIO_PIN(0, 28),
        .sda = GPIO_PIN(0, 29),
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
