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
        .cs_pin   = SPI_CS_UNDEF,
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
        .irqn           = I2C1_ER_IRQn,
    },
    {
        .dev            = I2C3,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_H, 7),
        .sda_pin        = GPIO_PIN(PORT_H, 8),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C3EN,
        .irqn           = I2C3_ER_IRQn,
    },
};

#define I2C_0_ISR           isr_i2c1_er
#define I2C_1_ISR           isr_i2c3_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
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

/**
 * @name LTDC configuration
 * @{
 */
/** LTDC static configuration struct */
static const ltdc_conf_t ltdc_config = {
    .bus        = APB2,
    .rcc_mask   = RCC_APB2ENR_LTDCEN,
    .clk_pin    = { .pin = GPIO_PIN(PORT_I, 14), .af = GPIO_AF14, },
    .de_pin     = { .pin = GPIO_PIN(PORT_K, 7), .af = GPIO_AF14, },
    .hsync_pin  = { .pin = GPIO_PIN(PORT_I, 10), .af = GPIO_AF14, },
    .vsync_pin  = { .pin = GPIO_PIN(PORT_I, 9), .af = GPIO_AF14, },
    .r_pin      = {
        { .pin = GPIO_PIN(PORT_I, 15), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 0), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 1), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 2), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 3), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 4), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 5), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 6), .af = GPIO_AF14, },
    },
    .g_pin      = {
        { .pin = GPIO_PIN(PORT_J, 7), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 8), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 9), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 10), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 11), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 0), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 1), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 2), .af = GPIO_AF14, },
    },
    .b_pin      = {
        { .pin = GPIO_PIN(PORT_E, 4), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 13), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 14), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_J, 15), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_G, 12), .af = GPIO_AF9, },
        { .pin = GPIO_PIN(PORT_K, 4), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 5), .af = GPIO_AF14, },
        { .pin = GPIO_PIN(PORT_K, 6), .af = GPIO_AF14, },
    },
    /* values below come from STM32CubeF7 code and differ from the typical
     * values mentioned in the RK043FN48H datasheet. Both sets of values work
     * with the display.
     * See the discussion in https://community.st.com/s/question/0D50X0000BOvdWP/how-to-set-displays-parameters-
     */
    .hsync      = 41,
    .vsync      = 10,
    .hbp        = 13,
    .hfp        = 32,
    .vbp        = 2,
    .vfp        = 2,
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
