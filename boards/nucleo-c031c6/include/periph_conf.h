/*
 * Copyright (C) 2024 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-c031c6
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-c031c6 board
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
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
        .base     = periph_timer3,
        .dev      = TIM3,
        .max      = 0x0000ffff,
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
        .bus        = APB12, //?
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
 * and not in the STM32C031 order. Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5.
 *
 * To find appropriate device and channel find in the
 * board manual, table showing pin assignments and
 * information about ADC - a text similar to ARD_A[N]_IN[X],
 * where:
 * [N] - describes analog pin number,
 * [X] - describes used channel - indexed from 1,
 * for example ARD_A5_IN16 is channel 16
 *
 * For Nucleo-C031C6 this information is in board manual,
 * Table 11, page 20.
 *
 * STM32C031C6 do not have internal channel for VBAT, more details provided
 * in the MCU datasheet - section 3.14, page 20.
 *
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A,  0), .dev = 0, .chan =  0 }, /* ARD_A0_IN0  */
    { .pin = GPIO_PIN(PORT_A,  1), .dev = 0, .chan =  1 }, /* ARD_A1_IN1  */
    { .pin = GPIO_PIN(PORT_A,  4), .dev = 0, .chan =  4 }, /* ARD_A2_IN4  */
    { .pin = GPIO_PIN(PORT_B,  1), .dev = 0, .chan = 18 }, /* ARD_A3_IN18 */
    { .pin = GPIO_PIN(PORT_A, 11), .dev = 0, .chan = 11 }, /* ARD_A4_IN11 */
    { .pin = GPIO_PIN(PORT_A, 12), .dev = 0, .chan = 12 }, /* ARD_A5_IN12 */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 *
 * To find appriopate device and channel find in the MCU datasheet table
 * concerning "Alternate function AF0 to AF7" a text similar to TIM[X]_CH[Y],
 * where:
 * TIM[X] - is device,
 * [Y] - describes used channel (indexed from 0), for example TIM2_CH1 is
 * channel 0 in configuration structure (cc_chan - field),
 * Port column in the table describes connected port.
 *
 * For Nucleo-c031c6 this information is in the MCU datasheet, Table 13, page 35.
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

#endif /* PERIPH_CONF_H */
/** @} */
