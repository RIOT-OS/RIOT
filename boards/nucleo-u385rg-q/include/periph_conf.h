/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-u385rg
 * @{
 *
 * @file
 * @brief       Minimal peripheral configuration for STM32U385 (bring-up)
 * 
 * @author      Adarsh Nair Mullachery
 */

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* No UART, SPI, I2C, PWM, USB yet */

/** @} */
/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIM2,
        .max = 0xffffffff,              /* 32-bit timer */
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
        .bus = APB1,
        .irqn = TIM2_IRQn,
    }
};

#define TIMER_NUMOF        ARRAY_SIZE(timer_config)

#define TIMER_0_ISR        isr_tim2
/** @} */

static const uart_conf_t uart_config[] = {
    {
        .dev = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin = GPIO_PIN(PORT_A, 10),
        .tx_pin = GPIO_PIN(PORT_A, 9),
        .rx_af = GPIO_AF7,
        .tx_af = GPIO_AF7,
        .bus = APB2,
        .irqn = USART1_IRQn,
        .type = STM32_USART,
        .clk_src = 0, /* Use APB clock */
    }
};

#define UART_0_ISR isr_usart1
#define UART_NUMOF (1)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
