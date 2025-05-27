/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nucleo-f746zg
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f746zg board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

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
    { .stream = 10 },   /* DMA2 Stream 2 - SPI1_RX   */
    { .stream = 13 },   /* DMA2 Stream 5 - SPI1_TX   */
    { .stream = 11 },   /* DMA2 Stream 3 - SPI4_RX   */
    { .stream = 12 },   /* DMA2 Stream 4 - SPI4_TX   */
    { .stream = 8 },    /* DMA2 Stream 0 - ETH_TX    */
};

#define DMA_0_ISR  isr_dma1_stream4
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
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_E,  9) /* D6  CN10-4  */, .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_E, 11) /* D5  CN10-6  */, .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_E, 13) /* D3  CN10-10 */, .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_E, 14) /* D38 CN10-28 */, .cc_chan = 3} },
        .af       = GPIO_AF1,
        .bus      = APB2
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_D, 12) /* D29 CN10-21 */, .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_D, 13) /* D28 CN10-19 */, .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_D, 14) /* D10  CN7-16 */, .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_D, 15) /* D9   CN7-18 */, .cc_chan = 3} },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
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
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 4,
        .tx_dma_chan = 3,
        .rx_dma   = 3,
        .rx_dma_chan = 3,
#endif
    },
    {
        .dev      = SPI4,
        .mosi_pin = GPIO_PIN(PORT_E, 6),
        .miso_pin = GPIO_PIN(PORT_E, 5),
        .sclk_pin = GPIO_PIN(PORT_E, 2),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI4EN,
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 6,
        .tx_dma_chan = 5,
        .rx_dma   = 5,
        .rx_dma_chan = 5,
#endif
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
    .speed = MII_BMCR_SPEED_100 | MII_BMCR_FULL_DPLX,
    .dma = 7,
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

/** @} */
