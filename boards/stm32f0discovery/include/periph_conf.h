/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f0discovery
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F0discovery board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

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
        .dev      = TIM2,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .rx_af      = GPIO_AF0,
        .tx_af      = GPIO_AF0,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart2)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @brief   ADC configuration
 *
 * We need to configure the following values:
 * [ pin, channel ]
 * @{
 */
#define ADC_CONFIG {            \
    { GPIO_PIN(PORT_C, 0), 10 },\
    { GPIO_PIN(PORT_C, 1), 11 },\
    { GPIO_PIN(PORT_C, 2), 12 },\
    { GPIO_PIN(PORT_C, 3), 13 },\
    { GPIO_PIN(PORT_C, 4), 14 },\
    { GPIO_PIN(PORT_C, 5), 15 } \
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
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            1
#define SPI_IRQ_PRIO        1

/* SPI 0 device config */
#define SPI_0_DEV           SPI1
#define SPI_0_CLKEN()       (periph_clk_en(APB2, RCC_APB2ENR_SPI1EN))
#define SPI_0_CLKDIS()      (periph_clk_dis(APB2, RCC_APB2ENR_SPI1EN))
#define SPI_0_IRQ           SPI1_IRQn
#define SPI_0_ISR           isr_spi1
/* SPI 1 pin configuration */
#define SPI_0_PORT          GPIOA
#define SPI_0_PORT_CLKEN()  (periph_clk_en(AHB, RCC_AHBENR_GPIOAEN))
#define SPI_0_PIN_SCK       5
#define SPI_0_PIN_MISO      6
#define SPI_0_PIN_MOSI      7
#define SPI_0_PIN_AF        0

/* SPI 1 device config */
#define SPI_1_DEV           SPI2
#define SPI_1_CLKEN()       (periph_clk_en(APB1, RCC_APB1ENR_SPI2EN))
#define SPI_1_CLKDIS()      (periph_clk_dis(APB1, RCC_APB1ENR_SPI2EN))
#define SPI_1_IRQ           SPI2_IRQn
#define SPI_1_ISR           isr_spi2
/* SPI 1 pin configuration */
#define SPI_1_PORT          GPIOB
#define SPI_1_PORT_CLKEN()  (periph_clk_en(AHB, RCC_AHBENR_GPIOBEN))
#define SPI_1_PIN_SCK       13
#define SPI_1_PIN_MISO      14
#define SPI_1_PIN_MOSI      15
#define SPI_1_PIN_AF        0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
