/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-u385rg-q
 * @{
 *
 * @file
 * @brief       Minimal peripheral configuration for STM32U385 (bring-up)
 *
 * @author      Adarsh Nair Mullachery <adarsh.mullachery@tuhh.de>
 */

/* Nucleo board provides 32.768 kHz LSE for RTC (same as other Nucleo-U boards) */
#ifndef CONFIG_BOARD_HAS_LSE
#  define CONFIG_BOARD_HAS_LSE 1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 * @note    TIM2 is reserved for PWM (User LED / TIM2_CH1 on PA5).
 *          TIM3 is used here as the RIOT timer backend (general-purpose timer).
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIM3,
        .max = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM3EN,
        .bus = APB1,
        .irqn = TIM3_IRQn,
    }
};

#define TIMER_NUMOF        ARRAY_SIZE(timer_config)

#define TIMER_0_ISR        isr_tim3
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        /* ST-Link Virtual COM Port (STDIO, UART_DEV(0)) — USART1 on PA9/PA10 */
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    {
        /* LPUART1 on PA2/PA3 */
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB3ENR_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB3,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0,
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_lpuart1)
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
