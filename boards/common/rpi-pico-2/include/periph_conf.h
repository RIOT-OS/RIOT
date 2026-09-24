/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         boards_rpi_pico_2
 * @{
 *
 * @file
 * @brief           Board periph definitions for the Raspberry Pi Pico 2
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include <stdint.h>

#include "RP2350.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Configuration details for an UART interface
 */
typedef struct {
    UART0_Type *dev;    /**< Base address of the I/O registers of the device */
    gpio_t rx_pin;      /**< GPIO pin to use for RX */
    gpio_t tx_pin;      /**< GPIO pin to use for TX */
    IRQn_Type irqn;     /**< IRQ number of the UART interface */
} uart_conf_t;

static const uart_conf_t uart_config[] = {
    {
        .dev = UART0,
        .rx_pin = GPIO_PIN(0, 1),
        .tx_pin = GPIO_PIN(0, 0),
        .irqn = UART0_IRQ_IRQn
    },
    {
        .dev = UART1,
        .rx_pin = GPIO_PIN(0, 9),
        .tx_pin = GPIO_PIN(0, 8),
        .irqn = UART1_IRQ_IRQn
    }
};

#define UART_0_ISR      (isr_uart0)
#define UART_1_ISR      (isr_uart1)

#define UART_NUMOF      ARRAY_SIZE(uart_config)

/**
 * @brief   Timer configuration
 *
 * The entries have to be ordered by hardware instance, see
 * @ref timer_conf_t for details.
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIMER0,
        .irqn_base = TIMER0_IRQ_0_IRQn,
        .ch_numof = TIMER_CHANNEL_NUMOF,
    },
    {
        .dev = TIMER1,
        .irqn_base = TIMER1_IRQ_0_IRQn,
        .ch_numof = TIMER_CHANNEL_NUMOF
    }
};

#define TIMER_NUMOF      2

#ifdef __cplusplus
}
#endif

/** @} */
