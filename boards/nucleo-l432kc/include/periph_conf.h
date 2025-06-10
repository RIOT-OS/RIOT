/*
 * Copyright (C) 2017  Inria
 *               2017  OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nucleo-l432kc
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-l432kc board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb6_pb7.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2_tim15_tim16.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 15),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF3,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
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
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 8) /* D9 */, .cc_chan = 0 },
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
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_B, 4),
        .sclk_pin = GPIO_PIN(PORT_B, 3),
        .cs_pin   = SPI_CS_UNDEF,
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

/**
 * @brief ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32L432KC order. Instead, we
 * just define 5 ADC channels, for the Nucleo
 * Arduino Nano header pins A0-A3 and A6 - all which are
 * enabled by default and without collision with other
 * features, for example, I2C or VCP_TX.
 *
 * To find appropriate device and channel find in the
 * board manual, table showing pin assignments and
 * information about ADC - a text similar to ADC[X]_IN[Y],
 * where:
 * [X] - describes used device - indexed from 0,
 * for example ADC1_IN10 is device 0,
 * [Y] - describes used channel - indexed from 1,
 * for example ADC1_IN10 is channel 10
 *
 * For Nucleo-L432KC this information is in board manual,
 * Table 16 or STM32L432KC MCU datasheet - Table 14.
 * @{
 */
static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_A, 0),  .dev = 0, .chan = 5},  /* ADC12_IN5  */
    {GPIO_PIN(PORT_A, 1),  .dev = 0, .chan = 6},  /* ADC12_IN6  */
    {GPIO_PIN(PORT_A, 3),  .dev = 0, .chan = 8},  /* ADC12_IN8  */
    {GPIO_PIN(PORT_A, 4),  .dev = 0, .chan = 9},  /* ADC12_IN9  */
    {GPIO_PIN(PORT_A, 7),  .dev = 0, .chan = 12}, /* ADC12_IN12 */
};

/**
 * @brief Number of ADC devices
 */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
