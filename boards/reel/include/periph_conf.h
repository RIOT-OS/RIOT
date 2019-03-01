/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_reel
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Phytec 'reel board'
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0,8),
        .tx_pin     = GPIO_PIN(0,6),
        .rts_pin    = (uint8_t)GPIO_UNDEF,
        .cts_pin    = (uint8_t)GPIO_UNDEF,
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = GPIO_PIN(0, 19),
        .mosi = GPIO_PIN(0, 20),
        .miso = GPIO_PIN(0, 21),
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM1,
        .scl = GPIO_PIN(0, 27),
        .sda = GPIO_PIN(0, 26),
        .speed = I2C_SPEED_NORMAL
    }
};
#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
