/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F3discovery board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 *
 * @note    This is auto-generated from
 *          `cpu/stm32_common/dist/clk_conf/clk_conf.c`
 * @{
 */
/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 72MHz */
#define CLOCK_CORECLOCK     (72000000U)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#define CLOCK_HSE           (8000000U)
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#define CLOCK_LSE           (1)
/* peripheral clock setup */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2     /* max 36MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* max 72MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)

/* PLL factors */
#define CLOCK_PLL_PREDIV     (1)
#define CLOCK_PLL_MUL        (9)
/** @} */

/**
 * @name   DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    { .pin = GPIO_PIN(PORT_A,  4), .chan = 0 }
};

#define DAC_NUMOF           (sizeof(dac_config) / sizeof(dac_config[0]))
/** @} */

/**
 * @name   Timer configuration
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

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
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
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart2)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_C, 6), .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7), .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_C, 8), .cc_chan = 2 },
                      { .pin = GPIO_PIN(PORT_C, 9), .cc_chan = 3 } },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_D, 12), .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_D, 13), .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_D, 14), .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_D, 15), .cc_chan = 3} },
        .af       = GPIO_AF2,
        .bus      = APB1
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 36000000Hz */
        7,  /* -> 140625Hz */
        6,  /* -> 281250Hz */
        4,  /* -> 1125000Hz */
        2,  /* -> 4500000Hz */
        1   /* -> 9000000Hz */
    },
    {       /* for APB2 @ 72000000Hz */
        7,  /* -> 281250Hz */
        7,  /* -> 281250Hz */
        5,  /* -> 1125000Hz */
        3,  /* -> 4500000Hz */
        2   /* -> 9000000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_UNDEF,
        .af       = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    {
        .dev      = SPI3,
        .mosi_pin = GPIO_PIN(PORT_C, 12),
        .miso_pin = GPIO_PIN(PORT_C, 11),
        .sclk_pin = GPIO_PIN(PORT_C, 10),
        .cs_pin   = GPIO_PIN(PORT_A, 15),
        .af       = GPIO_AF6,
        .rccmask  = RCC_APB1ENR_SPI3EN,
        .apbbus   = APB1
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 6),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .rcc_sw_mask    = RCC_CFGR3_I2C1SW,
        .irqn           = I2C1_ER_IRQn
    },
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_F, 1),
        .sda_pin        = GPIO_PIN(PORT_F, 0),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C2EN,
        .rcc_sw_mask    = RCC_CFGR3_I2C2SW,
        .irqn           = I2C2_ER_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_er
#define I2C_1_ISR           isr_i2c2_er

#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
