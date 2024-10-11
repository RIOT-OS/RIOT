/*
 * Copyright (C) 2021 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32g0316-disco
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for STM32G0316-DISCO board
 *
 * @author      Dave VanKampen <david.vankampen@bissell.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
#endif /* PERIPH_CONF_H */
