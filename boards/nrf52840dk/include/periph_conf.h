/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf52840dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF52840 DK
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include "kernel_defines.h"
#include "periph_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(1, 15),
        .mosi = GPIO_PIN(1, 13),
        .miso = GPIO_PIN(1, 14),
    },
    {
        .dev  = NRF_SPIM2,
        .sclk = GPIO_PIN(0, 19),
        .mosi = GPIO_PIN(0, 20),
        .miso = GPIO_PIN(0, 21),
    }
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UARTE0,
        .rx_pin     = GPIO_PIN(0, 8),
        .tx_pin     = GPIO_PIN(0, 6),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
    { /* Mapped to Arduino D0/D1 pins */
        .dev        = NRF_UARTE1,
        .rx_pin     = GPIO_PIN(1, 1),
        .tx_pin     = GPIO_PIN(1, 2),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE1_IRQn,
    },
};

#define UART_0_ISR          (isr_uart0)
#define UART_1_ISR          (isr_uarte1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
