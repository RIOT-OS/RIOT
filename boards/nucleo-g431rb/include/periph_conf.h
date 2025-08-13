/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-g431rb
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-g431rb board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif
/* This board provides a 24MHz HSE oscillator */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            1
#endif
/* By default, configure a 170MHz SYSCLK with PLL using HSE as input clock */
#ifndef CONFIG_CLOCK_PLL_M
#define CONFIG_CLOCK_PLL_M              (6)
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF12,
        .tx_af      = GPIO_AF12,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0, /* Use APB clock */
    },
    { /* Connected to Arduino D0/D1 */
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_C, 5),
        .tx_pin     = GPIO_PIN(PORT_C, 4),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
};

#define UART_0_ISR          (isr_lpuart1)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev            = SPI1,
        .mosi_pin       = GPIO_PIN(PORT_A, 7),  /* Arduino D11 */
        .miso_pin       = GPIO_PIN(PORT_A, 6),  /* Arduino D12 */
        .sclk_pin       = GPIO_PIN(PORT_A, 5),  /* Arduino D13 */
        .cs_pin         = GPIO_UNDEF,
        .mosi_af        = GPIO_AF5,
        .miso_af        = GPIO_AF5,
        .sclk_af        = GPIO_AF5,
        .cs_af          = GPIO_AF5,
        .rccmask        = RCC_APB2ENR_SPI1EN,
        .apbbus         = APB2,
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR1_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_C, 6), .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7), .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_C, 8), .cc_chan = 2 },
                      { .pin = GPIO_PIN(PORT_C, 9), .cc_chan = 3 } },
        .af       = GPIO_AF2,
        .bus      = APB1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
