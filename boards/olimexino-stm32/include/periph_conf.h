/*
 * Copyright (C) 2020 Scallog
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_olimexino-stm32
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the OLIMEXINO STM32 board
 *
 * @author      Corentin Vigourt <cvigourt@scallog.com>
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
        { GPIO_PIN(PORT_C, 0), 0, 10 },
        { GPIO_PIN(PORT_C, 1), 0, 11 },
        { GPIO_PIN(PORT_C, 2), 0, 12 },
        { GPIO_PIN(PORT_C, 3), 0, 13 },
        { GPIO_PIN(PORT_C, 4), 0, 14 },
        { GPIO_PIN(PORT_C, 5), 0, 15 }
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan = { { .pin = GPIO_PIN(PORT_A,   8), .cc_chan = 0 },
                  { .pin = GPIO_PIN(PORT_A,   9), .cc_chan = 1 },
                  { .pin = GPIO_PIN(PORT_A,  10), .cc_chan = 2 },
                  { .pin = GPIO_PIN(PORT_A,  11), .cc_chan = 3 } },
        .af = GPIO_AF_OUT_PP,
        .bus = APB2
    },
    {
        .dev = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .chan = { { .pin = GPIO_PIN(PORT_A,  0), .cc_chan = 0 },
                  { .pin = GPIO_PIN(PORT_A,  1), .cc_chan = 1 },
                  { .pin = GPIO_PIN(PORT_A,  2), .cc_chan = 2 },
                  { .pin = GPIO_PIN(PORT_A,  3), .cc_chan = 3 } },
        .af = GPIO_AF_OUT_PP,
        .bus = APB1
    },
    {
        .dev = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan = { { .pin = GPIO_PIN(PORT_A,  6), .cc_chan = 0 },
                  { .pin = GPIO_PIN(PORT_A,  7), .cc_chan = 1 },
                  { .pin = GPIO_PIN(PORT_B,  0), .cc_chan = 2 },
                  { .pin = GPIO_PIN(PORT_B,  1), .cc_chan = 3 } },
        .af = GPIO_AF_OUT_PP,
        .bus = APB1
    },
    {
        .dev = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .chan = { { .pin = GPIO_PIN(PORT_B, 6), .cc_chan = 0 },
                  { .pin = GPIO_PIN(PORT_B, 7), .cc_chan = 1 },
                  { .pin = GPIO_PIN(PORT_B, 8), .cc_chan = 2 },
                  { .pin = GPIO_PIN(PORT_B, 9), .cc_chan = 3 } },
        .af = GPIO_AF_OUT_PP,
        .bus = APB1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .base     = periph_timer2,
        .dev = TIM2,
        .max = 0x0000ffff,
        .bus = APB1,
        .irqn = TIM2_IRQn
    },
    {
        .base     = periph_timer3,
        .dev = TIM3,
        .max = 0x0000ffff,
        .bus = APB1,
        .irqn = TIM3_IRQn
    },
    {
        .base     = periph_timer4,
        .dev = TIM4,
        .max = 0x0000ffff,
        .bus = APB1,
        .irqn = TIM4_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim3
#define TIMER_2_ISR         isr_tim4

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = USART2,
        .rcc_mask = RCC_APB1ENR_USART2EN,
        .rx_pin = GPIO_PIN(PORT_A, 3),
        .tx_pin = GPIO_PIN(PORT_A, 2),
        .bus = APB1,
        .irqn = USART2_IRQn
    },
    {
        .dev = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin = GPIO_PIN(PORT_A, 10),
        .tx_pin = GPIO_PIN(PORT_A, 9),
        .bus = APB2,
        .irqn = USART1_IRQn
    },
    {
        .dev = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin = GPIO_PIN(PORT_B, 11),
        .tx_pin = GPIO_PIN(PORT_B, 10),
        .bus = APB1,
        .irqn = USART3_IRQn
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    Real time counter configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (16384)      /* in Hz */
#endif
/** @} */

/**
 * @name    I2C configuration
 * @note    This board may require external pullup resistors for i2c operation.
 * @{
 */

static const i2c_conf_t i2c_config[] = {
    {
        .dev = I2C1,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_B, 8),
        .sda_pin = GPIO_PIN(PORT_B, 9),
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C1EN,
        .clk = CLOCK_APB1,
        .irqn = I2C1_EV_IRQn
    },
    {
        .dev = I2C2,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_B, 10),
        .sda_pin = GPIO_PIN(PORT_B, 11),
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C2EN,
        .clk = CLOCK_APB1,
        .irqn = I2C2_EV_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_ev
#define I2C_1_ISR           isr_i2c2_ev

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin = GPIO_UNDEF,
        .rccmask = RCC_APB2ENR_SPI1EN,
        .apbbus = APB2
    },
    {
        .dev = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin = GPIO_UNDEF,
        .rccmask = RCC_APB1ENR_SPI2EN,
        .apbbus = APB1
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
