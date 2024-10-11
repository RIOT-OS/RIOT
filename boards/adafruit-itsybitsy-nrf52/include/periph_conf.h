/*
 * Copyright (C) 2020 Bruno Chianca <brunobcf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-itsybitsy-nrf52
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Adafruit ItsyBitsy nRF52840
 *
 * @author      Bruno Chianca <brunobcf@gmail.com>
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
 *
 * This board does RX and TX defined as pins 0 and 1 on the board.
 *
 * @{
 */

static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 25),
        .tx_pin     = GPIO_PIN(0, 24),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          isr_uart0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(0, 13),
        .mosi = GPIO_PIN(0, 15),
        .miso = GPIO_PIN(0, 20),
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
        .scl = GPIO_PIN(0, 14),
        .sda = GPIO_PIN(0, 16),
        .speed = I2C_SPEED_NORMAL
    }
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
