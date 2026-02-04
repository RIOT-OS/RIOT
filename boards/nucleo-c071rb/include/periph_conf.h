/*
 * SPDX-FileCopyrightText: 2024 BISSELL Homecare, Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_nucleo-c071rb
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Nucleo-C071RB board
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
 */

#pragma once

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#  define CONFIG_BOARD_HAS_LSE          1
#endif

#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"

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
    {
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
    },
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32C071 order. Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5.
 *
 * The appropriate ADC device and channel for each pin
 * can be found in the board manual in the pin assignment
 * table. The format of the entries is ADC_IN[Y], where
 * [Y] - describes the used channel - indexed from 1.
 * For example: ADC_IN11 is Channel 11.
 *
 * For the Nucleo-C071RB this information is in the board manual,
 * Table 12 "ARDUINO connectors on NUCLEO-C071RB and NUCLEO-C092RC".
 *
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A,  0), .dev = 0, .chan =  0 }, /* ADC_IN0  */
    { .pin = GPIO_PIN(PORT_A,  1), .dev = 0, .chan =  1 }, /* ADC_IN1  */
    { .pin = GPIO_PIN(PORT_A,  4), .dev = 0, .chan =  4 }, /* ADC_IN4  */
    { .pin = GPIO_PIN(PORT_B,  0), .dev = 0, .chan = 17 }, /* ADC_IN17 */
    { .pin = GPIO_PIN(PORT_C,  4), .dev = 0, .chan = 11 }, /* ADC_IN11 */
    { .pin = GPIO_PIN(PORT_C,  5), .dev = 0, .chan = 12 }, /* ADC_IN12 */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 *
 * The appropriate PWM device and channel for each pin can be found
 * in the MCU datasheet table "Alternate function AF0 to AF7".
 * The format of the entries is TIM[X]_CH[Y], where TIM[X] is the timer device
 * and [Y] describes the used channel (indexed from 0). For example TIM3_CH1 is
 * Timer 3, Channel 1 which corresponds to Channel 0 in the PWM configuration
 * structure.
 * The port column in the table describes the connected port.
 *
 * For the Nucleo-C071RB this information can be found in the MCU datasheet,
 * Table 15 "Port B alternate function mapping (AF0 to AF7)".
 *
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APBENR1_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 5) /*CN9  D6 */, .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_B, 0) /*CN5 D10 */, .cc_chan = 2 },
                      { .pin = GPIO_PIN(PORT_B, 1) /*CN8  A3 */, .cc_chan = 3 },
                      { .pin = GPIO_UNDEF,                       .cc_chan = 0 } },
        .af       = GPIO_AF1,
        .bus      = APB1
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev            = SPI1,
        .mosi_pin       = GPIO_PIN(PORT_A, 7),  /* Arduino D11 */
        .miso_pin       = GPIO_PIN(PORT_A, 6),  /* Arduino D12 */
        .sclk_pin       = GPIO_PIN(PORT_A, 5),  /* Arduino D13 */
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

#ifdef __cplusplus
}
#endif

/** @} */
