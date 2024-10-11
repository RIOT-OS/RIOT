/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_b-l072z-lrwan1
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the ST B-L072Z-LRWAN1 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Thibault Tisserand <gzordrai@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 1  }, /* channel 2 */
    { .stream = 2  }, /* channel 3 */
    { .stream = 3  }, /* channel 4 */
    { .stream = 4  }, /* channel 5 */
    { .stream = 5  }, /* channel 6 */
};

#define DMA_SHARED_ISR_0            isr_dma1_channel2_3
#define DMA_SHARED_ISR_0_STREAMS    { 0, 1 } /* Indexes 0 and 1 of dma_config share the same isr */
#define DMA_SHARED_ISR_1            isr_dma1_channel4_5_6_7
#define DMA_SHARED_ISR_1_STREAMS    { 2, 3, 4 } /* Indexes 2, 3 and 4 of dma_config share the same isr */

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF4,
        .tx_af      = GPIO_AF4,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = 4,
#endif
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF4,
        .tx_af      = GPIO_AF4,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
#ifdef MODULE_PERIPH_DMA
        .dma        = 0,
        .dma_chan   = 3,
#endif
    },
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF0,
        .miso_af  = GPIO_AF0,
        .sclk_af  = GPIO_AF0,
        .cs_af    = GPIO_AF0,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 3,
        .tx_dma_chan = 2,
        .rx_dma   = 2,
        .rx_dma_chan = 2,
#endif
    },
    {
        .dev      = SPI1, /* connected to SX1276 */
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_B, 3),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF0,
        .miso_af  = GPIO_AF0,
        .sclk_af  = GPIO_AF0,
        .cs_af    = GPIO_AF0,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 1,
        .tx_dma_chan = 1,
        .rx_dma   = 0,
        .rx_dma_chan = 1,
#endif
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { GPIO_PIN(PORT_A, 0), 0 },
    { GPIO_PIN(PORT_A, 2), 2 },
    { GPIO_PIN(PORT_A, 3), 3 },
    { GPIO_PIN(PORT_A, 4), 4 },
    { GPIO_PIN(PORT_A, 5), 5 }
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
