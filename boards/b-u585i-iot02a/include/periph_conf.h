/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_b-u585i-iot02a
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the ST B-U585I-IOT02A board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_timer_tim5.h"
#include "cfg_usb_otg_fs_u5.h"

#ifdef __cplusplus
extern "C" {
#endif

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
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    { /* Connected to Arduino D0/D1 and STMOD+2 */
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR1_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    { /* Connected to STMOD+1 */
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    { /* Connected to Wireless */
        .dev        = UART4,
        .rcc_mask   = RCC_APB1ENR1_UART4EN,
        .rx_pin     = GPIO_PIN(PORT_C, 11),
        .tx_pin     = GPIO_PIN(PORT_C, 10),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = UART4_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart3)
#define UART_2_ISR          (isr_usart2)
#define UART_3_ISR          (isr_uart4)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev            = SPI1,
        .mosi_pin       = GPIO_PIN(PORT_E, 15),     /* Arduino D11 */
        .miso_pin       = GPIO_PIN(PORT_E, 14),     /* Arduino D12 */
        .sclk_pin       = GPIO_PIN(PORT_E, 13),     /* Arduino D13 */
        .cs_pin         = GPIO_UNDEF,
        .mosi_af        = GPIO_AF5,
        .miso_af        = GPIO_AF5,
        .sclk_af        = GPIO_AF5,
        .cs_af          = GPIO_AF5,
        .rccmask        = RCC_APB2ENR_SPI1EN,
        .apbbus         = APB2,
    },
    {   /* Connected to wireless */
        .dev            = SPI2,
        .mosi_pin       = GPIO_PIN(PORT_D, 4),
        .miso_pin       = GPIO_PIN(PORT_D, 3),
        .sclk_pin       = GPIO_PIN(PORT_D, 1),
        .cs_pin         = GPIO_PIN(PORT_B, 12),
        .mosi_af        = GPIO_AF5,
        .miso_af        = GPIO_AF5,
        .sclk_af        = GPIO_AF5,
        .cs_af          = GPIO_AF5,
        .rccmask        = RCC_APB1ENR1_SPI2EN,
        .apbbus         = APB1,
    },
    {   /* Connected to STMOD+ 2 */
        .dev            = SPI3,
        .mosi_pin       = GPIO_PIN(PORT_D, 6),
        .miso_pin       = GPIO_PIN(PORT_G, 10),
        .sclk_pin       = GPIO_PIN(PORT_G, 9),
        .cs_pin         = GPIO_PIN(PORT_G, 12),
        .mosi_af        = GPIO_AF5,
        .miso_af        = GPIO_AF5,
        .sclk_af        = GPIO_AF5,
        .cs_af          = GPIO_AF5,
        .rccmask        = RCC_APB3ENR_SPI3EN,
        .apbbus         = APB3,
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
        .rcc_mask       = RCC_APB1ENR1_I2C1EN,
        .rcc_sw_mask    = RCC_CCIPR1_I2C1SEL_1,
        .irqn           = I2C1_ER_IRQn,
    },
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_H, 4),
        .sda_pin        = GPIO_PIN(PORT_H, 5),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C2EN,
        .rcc_sw_mask    = RCC_CCIPR1_I2C2SEL_1,
        .irqn           = I2C2_ER_IRQn,
    },
};

#define I2C_0_ISR           isr_i2c1_er
#define I2C_1_ISR           isr_i2c2_er
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
