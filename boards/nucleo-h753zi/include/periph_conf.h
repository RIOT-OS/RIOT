/*
 * SPDX-FileCopyrightText: 2024 Prime Controls, Inc.(R)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-h753zi
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-h753zi board
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#ifndef CONFIG_BOARD_HAS_LSE
#  define CONFIG_BOARD_HAS_LSE    1
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#  define CONFIG_BOARD_HAS_HSE    1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_timer_tim5_and_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @note    STM32H753ZI uses APB1L/APB2 buses (D2 Domain)
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1LENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn
    },
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_G, 9),
        .tx_pin     = GPIO_PIN(PORT_G, 14),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART6_IRQn
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1LENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    }
};

#define UART_0_ISR          (isr_usart3)
#define UART_1_ISR          (isr_usart6)
#define UART_2_ISR          (isr_usart2)
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_E, 9),  .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_E, 11), .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_E, 13), .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_E, 14), .cc_chan = 3} },
        .af       = GPIO_AF1,
        .bus      = APB2
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1LENR_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_D, 12), .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_D, 13), .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_D, 14), .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_D, 15), .cc_chan = 3} },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
