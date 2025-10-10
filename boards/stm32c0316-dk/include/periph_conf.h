/*
 * SPDX-FileCopyrightText: 2024 BISSELL Homecare, Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_stm32c0316-dk
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for STM32C0316-DK board
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
 */

#pragma once

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM3,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APBENR1_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    }
};

#define TIMER_0_ISR         isr_tim3

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APBENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
    }
};

#define UART_0_ISR          (isr_usart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
