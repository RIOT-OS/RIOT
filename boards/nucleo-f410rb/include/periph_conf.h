/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f410rb STM32 Nucleo-F410RB
 * @ingroup     boards_common_nucleo64
 * @brief       Support for the STM32 Nucleo-F410RB
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the nucleo-f410rb board
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
 * maximum: 100MHz */
#define CLOCK_CORECLOCK     (96000000U)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#define CLOCK_HSE           (8000000U)
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#define CLOCK_LSE           (1)
/* peripheral clock setup */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2     /* max 50MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* max 100MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)

/* Main PLL factors */
#define CLOCK_PLL_M          (4)
#define CLOCK_PLL_N          (192)
#define CLOCK_PLL_P          (4)
#define CLOCK_PLL_Q          (8)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    }
};

#define TIMER_0_ISR         isr_tim5

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
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 6,
        .dma_chan   = 4
#endif
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 5,
        .dma_chan   = 4
#endif
    }
};

/* assign ISR vector names */
#define UART_0_ISR          (isr_usart2)
#define UART_0_DMA_ISR      (isr_dma1_stream6)
#define UART_1_ISR          (isr_usart1)
#define UART_1_DMA_ISR      (isr_dma1_stream5)

/* deduct number of defined UART interfaces */
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name   SPI configuration
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
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
    {       /* for APB2 @ 96000000Hz */
        7,  /* -> 375000Hz */
        7,  /* -> 375000Hz */
        6,  /* -> 750000Hz */
        3,  /* -> 6000000Hz */
        2   /* -> 12000000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_A, 4),
        .af       = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C1_EV_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_ev

#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name   ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32F410 order.  Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5
 *
 * @{
 */
#define ADC_NUMOF          (6U)
#define ADC_CONFIG {             \
    {GPIO_PIN(PORT_A, 0), 0, 0}, \
    {GPIO_PIN(PORT_A, 1), 0, 1}, \
    {GPIO_PIN(PORT_A, 4), 0, 4}, \
    {GPIO_PIN(PORT_B, 0), 0, 8}, \
    {GPIO_PIN(PORT_C, 1), 0, 11}, \
    {GPIO_PIN(PORT_C, 0), 0, 10}, \
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
