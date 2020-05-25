/*
 * Copyright (C) 2019 Inria
 *               2019 Freie Universität Berln
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pyboard
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the pyboard board
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_usb_otg_fs.h"

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
 * maximum: 168MHz */
#define CLOCK_CORECLOCK     (168000000U)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#define CLOCK_HSE           (12000000U)
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#define CLOCK_LSE           (1U)
/* peripheral clock setup */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4     /* max 42MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 4)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2     /* max 84MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 2)

/* Main PLL factors */
#define CLOCK_PLL_M          (6)
#define CLOCK_PLL_N          (168)
#define CLOCK_PLL_P          (2)
#define CLOCK_PLL_Q          (7)
/** @} */

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 11 },   /* DMA2 Stream 3 - SPI1_TX */
    { .stream = 10 },   /* DMA2 Stream 2 - SPI1_RX */
};

#define DMA_0_ISR           isr_dma2_stream3
#define DMA_1_ISR           isr_dma2_stream2

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    Timer configuration
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

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = DMA_STREAM_UNDEF,
        .dma_chan   = UINT8_MAX,
#endif
    },
};

#define UART_0_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 42000000Hz */
        7,  /* -> 164062Hz */
        6,  /* -> 328125Hz */
        4,  /* -> 1312500Hz */
        2,  /* -> 5250000Hz */
        1   /* -> 10500000Hz */
    },
    {       /* for APB2 @ 84000000Hz */
        7,  /* -> 328125Hz */
        7,  /* -> 328125Hz */
        5,  /* -> 1312500Hz */
        3,  /* -> 5250000Hz */
        2   /* -> 10500000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev            = SPI1,
        .mosi_pin       = GPIO_PIN(PORT_A, 7),
        .miso_pin       = GPIO_PIN(PORT_A, 6),
        .sclk_pin       = GPIO_PIN(PORT_A, 5),
        .cs_pin         = GPIO_UNDEF,
        .mosi_af        = GPIO_AF5,
        .miso_af        = GPIO_AF5,
        .sclk_af        = GPIO_AF5,
        .cs_af          = GPIO_AF5,
        .rccmask        = RCC_APB2ENR_SPI1EN,
        .apbbus         = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma         = 0,
        .tx_dma_chan    = 3,
        .rx_dma         = 1,
        .rx_dma_chan    = 3,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 10),
        .sda_pin        = GPIO_PIN(PORT_B, 11),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C2EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C2_ER_IRQn,
    },
};

#define I2C_0_ISR           isr_i2c2_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
