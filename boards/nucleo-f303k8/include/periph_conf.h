/*
 * Copyright (C) 2017   Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f303k8 STM32 Nucleo-F303K8
 * @ingroup     boards_common_nucleo32
 * @brief       Support for the STM32 Nucleo-F303K8
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f303k8 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 *
 * @note    This is auto-generated from
 *          `cpu/stm32_common/dist/clk_conf/clk_conf.c`
 * @{
 */
/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 72MHz */
#define CLOCK_CORECLOCK     (64000000U)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#define CLOCK_HSE           (0U)
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#define CLOCK_LSE           (0)
/* peripheral clock setup */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2     /* max 36MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* max 72MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)

/* PLL factors */
#define CLOCK_PLL_PREDIV     (2)
#define CLOCK_PLL_MUL        (16)
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
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 15),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A,  9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 0) /* D3 */, .cc_chan = 2 },
                      { .pin = GPIO_PIN(PORT_B, 1) /* D6 */, .cc_chan = 3 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 } },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 8) /* D9 */, .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 } },
        .af       = GPIO_AF6,
        .bus      = APB2
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

/**
 * @name   SPI configuration
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 32000000Hz */
        7,  /* -> 125000Hz */
        5,  /* -> 500000Hz */
        4,  /* -> 1000000Hz */
        2,  /* -> 4000000Hz */
        1   /* -> 8000000Hz */
    },
    {       /* for APB2 @ 64000000Hz */
        7,  /* -> 250000Hz */
        6,  /* -> 500000Hz */
        5,  /* -> 1000000Hz */
        3,  /* -> 4000000Hz */
        2   /* -> 8000000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_B, 4),
        .sclk_pin = GPIO_PIN(PORT_B, 3),
        .cs_pin   = GPIO_UNDEF,
        .af       = GPIO_AF0,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name RTC configuration
 * @{
 */
#define RTC_NUMOF           (0U)
/** @} */

/**
 * @name   ADC configuration
 * @{
 */
#define ADC_NUMOF (0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
