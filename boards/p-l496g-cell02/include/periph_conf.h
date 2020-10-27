/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_p-l496g-cell02
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the P-L496G-CELL02 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF7,
        .rts_af     = GPIO_AF7,
#endif
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    {   /* Arduino pinout RX/TX pins on D0/D1 */
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_G, 8),
        .tx_pin     = GPIO_PIN(PORT_G, 7),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF7,
        .rts_af     = GPIO_AF7,
#endif
        .type       = STM32_LPUART,
        .clk_src    = 0,
    },
    {   /* STMod+/PMOD connectors */
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 6),
        .tx_pin     = GPIO_PIN(PORT_G, 10),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_PIN(PORT_G, 11),
        .rts_pin    = GPIO_PIN(PORT_G, 12),
        .cts_af     = GPIO_AF7,
        .rts_af     = GPIO_AF7,
#endif
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_lpuart1)
#define UART_2_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
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
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C1EN,
        .irqn           = I2C1_ER_IRQn
    },
};

#define I2C_0_ISR           isr_i2c1_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_B, 4),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
