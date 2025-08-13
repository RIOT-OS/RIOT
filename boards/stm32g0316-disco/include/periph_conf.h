/*
 * SPDX-FileCopyrightText: 2021 BISSELL Homecare, Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32g0316-disco
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for STM32G0316-DISCO board
 *
 * @author      Dave VanKampen <david.vankampen@bissell.com>
 */

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
        .dev      = TIM1,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APBENR2_TIM1EN,
        .bus      = APB12,
        .irqn     = TIM1_CC_IRQn
    }
};

#define TIMER_0_ISR         isr_tim1_cc
#define TIMER_0_MAX_VALUE   0xffff

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APBENR2_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .rx_af      = GPIO_AF0,
        .tx_af      = GPIO_AF0,
        .bus        = APB12,
        .irqn       = USART1_IRQn,
    }
};

#define UART_0_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
