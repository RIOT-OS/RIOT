/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
        .dev  = NRF_SPIM2,
        .sclk = GPIO_PIN(0, 8),
        .mosi = GPIO_PIN(0, 4),
        .miso = GPIO_PIN(0, 6),
    }/*, XXX: apparently, the second SPI is not routed in Pulga X V1.0
    {
        .dev  = NRF_SPIM2,
        .sclk = GPIO_PIN(0, 22),
        .mosi = GPIO_PIN(0, 17),
        .miso = GPIO_PIN(0, 20),
    }*/
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/* Config for which pins are used for uart */
#ifndef PULGA_UART0_RX
#define PULGA_UART0_RX GPIO_PIN(0,25)
#endif
#ifndef PULGA_UART0_TX
#define PULGA_UART0_TX GPIO_PIN(0,28)
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UARTE0,
        .rx_pin     = PULGA_UART0_RX,
        .tx_pin     = PULGA_UART0_TX,
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_UART0_IRQn,
    },
    {
        .dev        = NRF_UARTE1,
        // GPS Base pins
        .rx_pin     = GPIO_PIN(0,7),
        .tx_pin     = GPIO_PIN(0,5),
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

#endif /* PERIPH_CONF_H */
/** @} */
