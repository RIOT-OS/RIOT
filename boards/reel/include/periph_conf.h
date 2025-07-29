/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_i2c_default.h"
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
        .sclk = GPIO_PIN(0, 19),
        .mosi = GPIO_PIN(0, 20),
        .miso = GPIO_PIN(0, 21),
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
