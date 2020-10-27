/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f767zi
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f767zi board
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

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"
#include "cfg_usb_otg_fs.h"
#include "mii.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 4 },    /* DMA1 Stream 4 - USART3_TX */
    { .stream = 14 },   /* DMA2 Stream 6 - USART6_TX */
    { .stream = 6 },    /* DMA1 Stream 6 - USART2_TX */
    { .stream = 8 },    /* DMA2 Stream 0 - ETH_TX    */
};

#define DMA_0_ISR  isr_dma1_stream4
#define DMA_1_ISR  isr_dma2_stream6
#define DMA_2_ISR  isr_dma1_stream6
#define DMA_3_ISR  isr_dma2_stream0

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 0,
        .dma_chan   = 7
#endif
    },
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_G, 9),
        .tx_pin     = GPIO_PIN(PORT_G, 14),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 1,
        .dma_chan   = 5
#endif
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = 4
#endif
    }
};

#define UART_0_ISR          (isr_usart3)
#define UART_1_ISR          (isr_usart6)
#define UART_2_ISR          (isr_usart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        /* PA7 is the default MOSI pin, as it is required for compatibility with
         * Arduino(ish) shields. Sadly, it is also connected to the RMII_DV of
         * Ethernet PHY. We work around this by remapping the MOSI to PB5 when
         * the on-board Ethernet PHY is used.
         */
#ifdef MODULE_PERIPH_ETH
        .mosi_pin = GPIO_PIN(PORT_B, 5),
#else
        .mosi_pin = GPIO_PIN(PORT_A, 7),
#endif
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    {
        .dev      = SPI4,
        .mosi_pin = GPIO_PIN(PORT_E, 6),
        .miso_pin = GPIO_PIN(PORT_E, 5),
        .sclk_pin = GPIO_PIN(PORT_E, 2),
        .cs_pin   = GPIO_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI4EN,
        .apbbus   = APB2
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name ETH configuration
 * @{
 */
static const eth_conf_t eth_config = {
    .mode = RMII,
    .addr = { 0 },
    .speed = MII_BMCR_SPEED_100 | MII_BMCR_FULL_DPLX,
    .dma = 3,
    .dma_chan = 8,
    .phy_addr = 0x00,
    .pins = {
        GPIO_PIN(PORT_G, 13),
        GPIO_PIN(PORT_B, 13),
        GPIO_PIN(PORT_G, 11),
        GPIO_PIN(PORT_C, 4),
        GPIO_PIN(PORT_C, 5),
        GPIO_PIN(PORT_A, 7),
        GPIO_PIN(PORT_C, 1),
        GPIO_PIN(PORT_A, 2),
        GPIO_PIN(PORT_A, 1),
    }
};

#define ETH_DMA_ISR        isr_dma2_stream0

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
