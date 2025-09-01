/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include "cfg_clock_32_1.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 11),
        .tx_pin     = GPIO_PIN(0, 5),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define UART_0_ISR          (isr_uart0)
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

/** @} */
