/*
 * Copyright (C) 2017 Inria
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f302r8
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f302r8 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* This board provides an LSE */
#define CLOCK_LSE            (1)

#include "periph_cpu.h"
#include "f1f3/cfg_clock_default.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A,  9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_C, 11),
        .tx_pin     = GPIO_PIN(PORT_C, 10),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM16,
        .rcc_mask = RCC_APB2ENR_TIM16EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 4) /* D5 */, .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 } },
        .af       = GPIO_AF1,
        .bus      = APB2
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
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
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = GPIO_PIN(PORT_B, 12),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1
    }
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
        .rcc_sw_mask    = RCC_CFGR3_I2C1SW,
        .irqn           = I2C1_ER_IRQn
    },
    {
        .dev            = I2C3,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 8),
        .sda_pin        = GPIO_PIN(PORT_A, 5),
        .scl_af         = GPIO_AF5,
        .sda_af         = GPIO_AF8,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C3EN,
        .rcc_sw_mask    = RCC_CFGR3_I2C3SW,
        .irqn           = I2C3_ER_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_er
#define I2C_1_ISR           isr_i2c3_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
