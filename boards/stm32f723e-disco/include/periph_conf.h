/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f723e-disco
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F723E-DISCO board
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

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_usb_otg_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_C, 6),
        .tx_pin     = GPIO_PIN(PORT_C, 7),
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
    },
    { /* Arduino connector (D0/D1) */
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
    },
    { /* PMOD connector */
        .dev        = UART7,
        .rcc_mask   = RCC_APB1ENR_UART7EN,
        .rx_pin     = GPIO_PIN(PORT_F, 6),
        .tx_pin     = GPIO_PIN(PORT_F, 7),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB1,
        .irqn       = UART7_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_PIN(PORT_F, 9),
        .rts_pin    = GPIO_PIN(PORT_F, 8),
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
    },
    { /* ESP-01 connector */
        .dev        = UART5,
        .rcc_mask   = RCC_APB1ENR_UART5EN,
        .rx_pin     = GPIO_PIN(PORT_D, 2),
        .tx_pin     = GPIO_PIN(PORT_C, 12),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB1,
        .irqn       = UART5_IRQn,
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin    = GPIO_UNDEF,
        .rts_pin    = GPIO_UNDEF,
        .cts_af     = GPIO_AF8,
        .rts_af     = GPIO_AF8,
#endif
    },
};

#define UART_0_ISR          (isr_usart6)
#define UART_1_ISR          (isr_usart2)
#define UART_2_ISR          (isr_uart4)
#define UART_3_ISR          (isr_uart7)
#define UART_4_ISR          (isr_uart5)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    { /* Shared between Arduino D14/D15 and STMOD+ connector */
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_H, 4),
        .sda_pin        = GPIO_PIN(PORT_H, 5),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C2EN,
        .irqn           = I2C2_ER_IRQn,
    },
    {   /* Connected to touchscreen controller */
        .dev            = I2C3,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 8),
        .sda_pin        = GPIO_PIN(PORT_H, 8),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C3EN,
        .irqn           = I2C3_ER_IRQn,
    },
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
};

#define I2C_0_ISR           isr_i2c2_er
#define I2C_1_ISR           isr_i2c3_er
#define I2C_2_ISR           isr_i2c1_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    { /* Arduino connector */
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_B, 4),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    { /* PMOD connector */
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_I, 3),
        .miso_pin = GPIO_PIN(PORT_I, 2),
        .sclk_pin = GPIO_PIN(PORT_I, 1),
        .cs_pin   = GPIO_PIN(PORT_I, 0),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
