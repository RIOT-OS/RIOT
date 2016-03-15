/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_spark-core
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the spark-core board
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
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
 **/
#define CLOCK_HSE           (8000000U)             /* frequency of external oscillator */
#define CLOCK_CORECLOCK     (72000000U)             /* targeted core clock frequency */
/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSE / PLL_HSE_DIV * PLL_HSE_MUL */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLXTPRE_HSE  /* not divided */
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMULL9
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 72MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 72MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2     /* APB1 clock -> 36MHz */
/* resulting bus clocks */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB2          (CLOCK_CORECLOCK)
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_2
/** @} */

 /**
  * @brief   DAC configuration
  * @{
  */
 #define DAC_NUMOF           (0)
 /** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (0)
/** @} */

/**
 * @brief   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
    {
        .dev      = TIM3,
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
 * @brief UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev     = USART2,
        .rx_pin  = GPIO_PIN(PORT_A, 3),
        .tx_pin  = GPIO_PIN(PORT_A, 2),
        .rcc_pin = RCC_APB1ENR_USART2EN,
        .bus     = APB1,
        .irqn    = USART2_IRQn
    }
};

#define UART_0_ISR          isr_usart2

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)
#define SPI_0_EN            1

/* SPI 0 device configuration */
#define SPI_0_DEV           SPI1
#define SPI_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_BUS_DIV       0   /* 1 -> SPI runs with full CPU clock, 0 -> half CPU clock */
/* SPI 0 pin configuration */
#define SPI_0_CLK_PIN       GPIO_PIN(PORT_B,15)
#define SPI_0_MOSI_PIN      GPIO_PIN(PORT_B,17)
#define SPI_0_MISO_PIN      GPIO_PIN(PORT_B,16)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
