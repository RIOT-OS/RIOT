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
 * @name   Timer configuration
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
 * @name   UART configuration
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
 * @name   ADC configuration
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
 * @name   DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 48000000Hz */
        7,  /* -> 187500Hz */
        6,  /* -> 375000Hz */
        5,  /* -> 750000Hz */
        2,  /* -> 6000000Hz */
        1   /* -> 12000000Hz */
    },
    {       /* for APB2 @ 48000000Hz */
        7,  /* -> 187500Hz */
        6,  /* -> 375000Hz */
        5,  /* -> 750000Hz */
        2,  /* -> 6000000Hz */
        1   /* -> 12000000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_UNDEF,
        .af       = GPIO_AF0,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = GPIO_UNDEF,
        .af       = GPIO_AF0,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
