/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f746g-disco
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for STM32F746G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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

/* The HSE provides a 25MHz clock */
#define CLOCK_HSE               MHZ(25)

#include <stdint.h>

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"
#include "mii.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 15 },   /* DMA2 Stream 7 - USART1_TX */
    { .stream = 14 },   /* DMA2 Stream 6 - USART6_TX */
    { .stream = 6 },    /* DMA1 Stream 6 - USART2_TX */
    { .stream = 3 },    /* DMA1 Stream 3 - SPI2_RX   */
    { .stream = 4 },    /* DMA1 Stream 4 - SPI2_TX   */
    { .stream = 11 },   /* DMA2 Stream 3 - SPI4_RX   */
    { .stream = 12 },   /* DMA2 Stream 4 - SPI4_TX   */
    { .stream = 8 },    /* DMA2 Stream 0 - ETH_TX    */
};

#define DMA_0_ISR  isr_dma2_stream7
#define DMA_1_ISR  isr_dma2_stream6
#define DMA_2_ISR  isr_dma1_stream6

#define DMA_3_ISR  isr_dma2_stream2
#define DMA_4_ISR  isr_dma2_stream5
#define DMA_5_ISR  isr_dma2_stream3
#define DMA_6_ISR  isr_dma2_stream4

#define DMA_7_ISR  isr_dma2_stream0

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
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = 0,
        .dma_chan   = 4
#endif
    },
    {  /* Arduino connectors */
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_C, 6),
        .tx_pin     = GPIO_PIN(PORT_C, 7),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = 1,
        .dma_chan   = 5
#endif
    },
};

#define UART_0_ISR          (isr_usart1)
#define UART_0_DMA_ISR      (isr_dma2_stream7)
#define UART_6_ISR          (isr_usart6)
#define UART_6_DMA_ISR      (isr_dma2_stream6)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_I, 1),
        .cs_pin   = GPIO_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 4,
        .tx_dma_chan = 0,
        .rx_dma   = 3,
        .rx_dma_chan = 0,
#endif
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name ETH configuration
 * @{
 */
static const eth_conf_t eth_config = {
    .mode = RMII,
    .speed = MII_BMCR_SPEED_100 | MII_BMCR_FULL_DPLX,
    .dma = 7,
    .dma_chan = 8,
    .phy_addr = 0x00,
    .pins = {
        GPIO_PIN(PORT_G, 13),   /* TXD0 */
        GPIO_PIN(PORT_G, 14),   /* TXD1 */
        GPIO_PIN(PORT_G, 11),   /* TX_EN */
        GPIO_PIN(PORT_C, 4),    /* RXD0 */
        GPIO_PIN(PORT_C, 5),    /* RXD1 */
        GPIO_PIN(PORT_A, 7),    /* CRS_DV */
        GPIO_PIN(PORT_C, 1),    /* MDC */
        GPIO_PIN(PORT_A, 2),    /* MDIO */
        GPIO_PIN(PORT_A, 1),    /* REF_CLK */
    }
};

#define ETH_DMA_ISR        isr_dma2_stream0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
