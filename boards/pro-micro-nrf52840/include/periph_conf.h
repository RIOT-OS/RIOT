/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_pro-micro-nrf52840
 * @{
 *
 * @file
 * @brief       Peripheral configuration for Pro Micro nRF52840-compatible
 *              development boards
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 */

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
        .rx_pin     = GPIO_PIN(0, 8),
        .tx_pin     = GPIO_PIN(0, 6),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(1, 13),
        .mosi = GPIO_PIN(0, 10),
        .miso = GPIO_PIN(1, 11),
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
        .dev = NRF_TWIM0,
        .scl = GPIO_PIN(0, 20),
        .sda = GPIO_PIN(0, 17),
        .speed = I2C_SPEED_NORMAL
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
