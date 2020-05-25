/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ublox-c030-u201
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Ublox C030-U201 board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_timer_tim5.h"

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
 * maximum: 180MHz */
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
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4     /* max 45MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 4)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2     /* max 90MHz */
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
    { .stream = 9 },   /* DMA2 Stream 1 - SPI4_TX */
    { .stream = 8 },   /* DMA2 Stream 0 - SPI4_RX */
};

#define DMA_0_ISR           isr_dma2_stream1
#define DMA_1_ISR           isr_dma2_stream0

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF7,
        .rts_af     = GPIO_AF7,
#endif
#ifdef MODULE_PERIPH_DMA
        .dma        = DMA_STREAM_UNDEF,
        .dma_chan   = UINT8_MAX,
#endif
    },
    {   /* Modem UART */
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_PIN(PORT_D, 3),
        .rts_pin    = GPIO_PIN(PORT_D, 4),
        .cts_af     = GPIO_AF7,
        .rts_af     = GPIO_AF7,
#endif
#ifdef MODULE_PERIPH_DMA
        .dma        = DMA_STREAM_UNDEF,
        .dma_chan   = UINT8_MAX,
#endif
    },
    {   /* GPS UART */
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_C, 7),
        .tx_pin     = GPIO_PIN(PORT_C, 6),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
#ifdef MODULE_PERIPH_DMA
        .dma        = DMA_STREAM_UNDEF,
        .dma_chan   = UINT8_MAX,
#endif
    },
    {   /* Arduino Port UART */
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF7,
        .rts_af     = GPIO_AF7,
#endif
#ifdef MODULE_PERIPH_DMA
        .dma        = DMA_STREAM_UNDEF,
        .dma_chan   = UINT8_MAX,
#endif
    },
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart2)
#define UART_2_ISR          (isr_usart6)
#define UART_3_ISR          (isr_usart3)

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
        .dev            = SPI4,
        .mosi_pin       = GPIO_PIN(PORT_E, 6),
        .miso_pin       = GPIO_PIN(PORT_E, 5),
        .sclk_pin       = GPIO_PIN(PORT_E, 2),
        .cs_pin         = GPIO_PIN(PORT_E, 11),
        .mosi_af        = GPIO_AF5,
        .miso_af        = GPIO_AF5,
        .sclk_af        = GPIO_AF5,
        .cs_af          = GPIO_AF5,
        .rccmask        = RCC_APB2ENR_SPI4EN,
        .apbbus         = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma         = 0,
        .tx_dma_chan    = 4,
        .rx_dma         = 1,
        .rx_dma_chan    = 4,
#endif
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 6),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C1_EV_IRQn
    },
    {
        .dev            = I2C3,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 8),
        .sda_pin        = GPIO_PIN(PORT_C, 9),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C3EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C3_EV_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_ev
#define I2C_1_ISR           isr_i2c3_ev

#define I2C_NUMOF ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32F437 order.  Instead, we
 * just define 6 ADC channels, for the
 * Arduino header pins A0-A5
 *
 * @{
 */
#define ADC_NUMOF          (6U)
#define ADC_CONFIG {             \
    {GPIO_PIN(PORT_A, 3), 0, 3}, \
    {GPIO_PIN(PORT_C, 0), 0, 10}, \
    {GPIO_PIN(PORT_C, 3), 0, 4}, \
    {GPIO_PIN(PORT_A, 4), 0, 14}, \
    {GPIO_PIN(PORT_B, 7), 0, 7}, \
    {GPIO_PIN(PORT_B, 6), 0, 6}, \
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
