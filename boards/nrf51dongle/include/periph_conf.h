/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nrf51dongle
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Nordic nRF51 Dongle
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "periph_cpu.h"
#include "cfg_clock_16_1.h"
#include "cfg_timer_012.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UART0,
        .rx_pin     = GPIO_PIN(0, 11),
        .tx_pin     = GPIO_PIN(0, 9),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_PIN(0, 8),
        .cts_pin    = GPIO_PIN(0, 10),
#endif
        .irqn       = UART0_IRQn,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define UART_0_ISR          isr_uart0
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
