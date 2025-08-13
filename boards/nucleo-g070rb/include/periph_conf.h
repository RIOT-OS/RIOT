/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-g070rb
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-g070rb board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "periph_cpu.h"

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
        .rx_pin     = GPIO_PIN(PORT_C, 5),
        .tx_pin     = GPIO_PIN(PORT_C, 4),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB12,
        .irqn       = USART1_IRQn,
    },
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @brief    ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32G070 order. Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5 and the internal VBAT channel.
 *
 * To find appropriate device and channel find in the
 * board manual, table showing pin assignments and
 * information about ADC - a text similar to ARD_A[N]_IN[X],
 * where:
 * [N] - describes analog pin number,
 * [X] - describes used channel - indexed from 1,
 * for example ARD_A5_IN16 is channel 16
 *
 * For Nucleo-G070RB this information is in board manual,
 * Table 12, page 30.
 *
 * VBAT is connected to an internal input and voltage divider
 * is used, so that only 1/3 of the actual VBAT is measured. This
 * allows for a supply voltage higher than the reference voltage.
 *
 * For Nucleo-G070RB more information is provided in MCU datasheet,
 * in section 3.14.3 - Vbat battery voltage monitoring, page 22.
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A,  0), .dev = 0, .chan =  0 }, /* ARD_A0_IN0  */
    { .pin = GPIO_PIN(PORT_A,  1), .dev = 0, .chan =  1 }, /* ARD_A1_IN1  */
    { .pin = GPIO_PIN(PORT_A,  4), .dev = 0, .chan =  4 }, /* ARD_A2_IN4  */
    { .pin = GPIO_PIN(PORT_B,  1), .dev = 0, .chan =  9 }, /* ARD_A3_IN9  */
    { .pin = GPIO_PIN(PORT_B, 11), .dev = 0, .chan = 15 }, /* ARD_A4_IN15 */
    { .pin = GPIO_PIN(PORT_B, 12), .dev = 0, .chan = 16 }, /* ARD_A5_IN16 */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 14}, /* VBAT */
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
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
