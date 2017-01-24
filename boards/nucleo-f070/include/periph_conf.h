/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f070
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f070 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Aabdie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_HSE           (8000000U)          /* external oscillator */
#define CLOCK_CORECLOCK     (48000000U)         /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_MUL       (CLOCK_CORECLOCK / CLOCK_HSE)

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @brief   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM14,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB1ENR_TIM14EN,
        .bus      = APB1,
        .irqn     = TIM14_IRQn
    },
    {
        .dev      = TIM16,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB2ENR_TIM16EN,
        .bus      = APB2,
        .irqn     = TIM16_IRQn
    },
    {
        .dev      = TIM17,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB2ENR_TIM17EN,
        .bus      = APB2,
        .irqn     = TIM17_IRQn
    }
};

#define TIMER_0_ISR         isr_tim14
#define TIMER_1_ISR         isr_tim16
#define TIMER_2_ISR         isr_tim17


#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB2,
        .irqn       = USART1_IRQn
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_C, 11),
        .tx_pin     = GPIO_PIN(PORT_C, 10),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB1,
        .irqn       = USART3_4_IRQn
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)
#define UART_2_ISR          (isr_usart3_8)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @brief   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 4) /* D5 */, .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_B, 5) /* D4 */, .cc_chan = 1 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 } },
        .af       = GPIO_AF1,
        .bus      = APB1
    },
    {
        .dev      = TIM15,
        .rcc_mask = RCC_APB2ENR_TIM15EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 14), .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_B, 15), .cc_chan = 1 },
                      { .pin = GPIO_UNDEF,           .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,           .cc_chan = 0 } },
        .af       = GPIO_AF1,
        .bus      = APB2
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

/**
 * @brief   ADC configuration
 * @{
 */
#define ADC_CONFIG {            \
    { GPIO_PIN(PORT_A, 0), 0 },\
    { GPIO_PIN(PORT_A, 1), 1 },\
    { GPIO_PIN(PORT_A, 4), 4 },\
    { GPIO_PIN(PORT_B, 0), 8 },\
    { GPIO_PIN(PORT_C, 1), 11 },\
    { GPIO_PIN(PORT_C, 0), 10 } \
}

#define ADC_NUMOF           (6)
/** @} */

/**
 * @brief   DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

/**
 * @name RTC configuration
 * @{
 */
/**
 * Nucleos with MB1136 C-02 or MB1136 C-03 -sticker on it have the required LSE
 * oscillator provided on the X2 slot.
 * See Nucleo User Manual UM1724 section 5.6.2.
 */
#define RTC_NUMOF           (1U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
