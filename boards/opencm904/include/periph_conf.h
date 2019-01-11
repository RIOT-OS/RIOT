/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_opencm904
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the opencm904 board
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock system configuration
 * @{
 **/
/* high speed clock configuration:
 * 0 := use internal HSI oscillator (always 8MHz)
 * HSE frequency value := use external HSE oscillator with given freq [in Hz]
 *                        must be 4000000 <= value <= 16000000 */
#define CLOCK_HSE           (8000000U)
/* low speed clock configuration:
 * 0 := use internal LSI oscillator (~40kHz)
 * 1 := use extern LSE oscillator, always 32.768kHz */
#define CLOCK_LSE           (0)
/* targeted system clock speed [in Hz], must be <= 72MHz */
#define CLOCK_CORECLOCK     (72000000U)
/* PLL configuration, set both values to zero to disable PLL usage. The values
 * must be set to satisfy the following equation:
 * CORECLOCK := CLOCK_SOURCE / PLL_DIV * PLL_MUL
 * with
 * 1 <= CLOCK_PLL_DIV <= 2
 * 2 <= CLOCK_PLL_MUL <= 17 */
#define CLOCK_PLL_DIV       (1)
#define CLOCK_PLL_MUL       (9)
/* AHB and APBx bus clock configuration, keep in mind the following constraints:
 * ABP1 <= 36MHz
 */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
    {
        .dev      = TIM3,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim3

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .bus        = APB1,
        .irqn       = USART2_IRQn
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .bus        = APB2,
        .irqn       = USART1_IRQn
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_B, 11),
        .tx_pin     = GPIO_PIN(PORT_B, 10),
        .bus        = APB1,
        .irqn       = USART3_IRQn
    }
};

#define UART_0_ISR          isr_usart2
#define UART_1_ISR          isr_usart1
#define UART_2_ISR          isr_usart3

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
