/*
 * Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dk_modded
 * @{
 *
 * @file
 * @brief       Minimal peripheral set for the nRF52840 DK with quadrature
 *              decoder enabled
 *
 * @author      Koen Zandberg <koen@bergzand.net>
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
 * @name QDEC configuration
 */
static const qdec_conf_t qdec_config[] = {
    {
        .a_pin  = GPIO_PIN(0, 11),  /* Button 1 */
        .b_pin  = GPIO_PIN(0, 12),  /* Button 2 */
        .led_pin = GPIO_PIN(0, 13), /* And the first LED */
        .sample_period = QDEC_SAMPLEPER_SAMPLEPER_128us,
        .debounce_filter = true,
    },
};
#define QDEC_NUMOF          ARRAY_SIZE(qdec_config)
/** @} **/

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
};

#define UART_0_ISR          (isr_uart0)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
