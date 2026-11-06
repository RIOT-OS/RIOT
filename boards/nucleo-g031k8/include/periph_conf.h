/*
 * SPDX-FileCopyrightText: 2026 Hudson C. Dalpra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-g031k8
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for STM32 Nucleo-G031K8 board
 *
 * @author      Hudson C. Dalpra <dalpra.hcd@gmail.com>
 */

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM3,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APBENR1_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    }
};

#define TIMER_0_ISR         isr_tim3

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {   /* Connected to the ST-Link */
        .dev        = USART2,
        .rcc_mask   = RCC_APBENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
    },
    {   /* Arduino pinout on D0/D1 */
        .dev        = USART1,
        .rcc_mask   = RCC_APBENR2_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .rx_af      = GPIO_AF0,
        .tx_af      = GPIO_AF0,
        .bus        = APB12,
        .irqn       = USART1_IRQn,
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev            = SPI1,
        .mosi_pin       = GPIO_PIN(PORT_B, 5),  /* Arduino D11 */
        .miso_pin       = GPIO_PIN(PORT_B, 4),  /* Arduino D12 */
        .sclk_pin       = GPIO_PIN(PORT_B, 3),  /* Arduino D13 */
        .cs_pin         = GPIO_UNDEF,
        .mosi_af        = GPIO_AF0,
        .miso_af        = GPIO_AF0,
        .sclk_af        = GPIO_AF0,
        .cs_af          = GPIO_AF0,
        .rccmask        = RCC_APBENR2_SPI1EN,
        .apbbus         = APB12,
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 9),   /* Arduino D5 */
        .sda_pin        = GPIO_PIN(PORT_A, 10),  /* Arduino D4 */
        .scl_af         = GPIO_AF6,
        .sda_af         = GPIO_AF6,
        .bus            = APB1,
        .rcc_mask       = RCC_APBENR1_I2C1EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C1SEL_1,   /* HSI (16 MHz) */
        .irqn           = I2C1_IRQn,
    },
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 11),  /* Arduino A5 */
        .sda_pin        = GPIO_PIN(PORT_A, 12),  /* Arduino A4 */
        .scl_af         = GPIO_AF6,
        .sda_af         = GPIO_AF6,
        .bus            = APB1,
        .rcc_mask       = RCC_APBENR1_I2C2EN,
        /* I2C2 has no kernel clock mux, it is always clocked from PCLK */
        .irqn           = I2C2_IRQn,
    }
};

#define I2C_0_ISR           isr_i2c1
#define I2C_1_ISR           isr_i2c2

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
