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

/* This board provides an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE    1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE    1
#endif

/* The HSE provides a 12MHz clock */
#ifndef CONFIG_CLOCK_HSE
#define CONFIG_CLOCK_HSE               MHZ(12)
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_timer_tim5.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @{
 */
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
 * Arduino header pins A0-A5 and the internal VBAT channel.
 *
 * @{
 */
static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_A, 3), 0, 3},
    {GPIO_PIN(PORT_C, 0), 0, 10},
    {GPIO_PIN(PORT_C, 3), 0, 4},
    {GPIO_PIN(PORT_A, 4), 0, 14},
    {GPIO_PIN(PORT_B, 7), 0, 7},
    {GPIO_PIN(PORT_B, 6), 0, 6},
    {GPIO_UNDEF, 0, 18}, /* VBAT */
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
