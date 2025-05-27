/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nucleo-f722ze
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f722ze board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

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
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"
#include "cfg_usb_otg_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 6,
        .dma_chan   = 4
#endif
    },
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_G, 9),
        .tx_pin     = GPIO_PIN(PORT_G, 14),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 5,
        .dma_chan   = 4
#endif
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 4,
        .dma_chan   = 4
#endif
    }
};

#define UART_0_ISR          (isr_usart3)
#define UART_0_DMA_ISR      (isr_dma1_stream6)
#define UART_1_ISR          (isr_usart6)
#define UART_1_DMA_ISR      (isr_dma1_stream5)
#define UART_2_ISR          (isr_usart2)
#define UART_2_DMA_ISR      (isr_dma1_stream4)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_A, 3),  .dev = 2, .chan = 3},  /* ADC123_IN3 */
    {GPIO_PIN(PORT_C, 0),  .dev = 2, .chan = 10}, /* ADC123_IN10 */
    {GPIO_PIN(PORT_C, 3),  .dev = 2, .chan = 13}, /* ADC123_IN13 */
    {GPIO_PIN(PORT_F, 3),  .dev = 2, .chan = 9},  /* ADC3_IN9    */
    {GPIO_PIN(PORT_F, 5),  .dev = 2, .chan = 15}, /* ADC3_IN15   */
    {GPIO_PIN(PORT_F, 10), .dev = 2, .chan = 8},  /* ADC3_IN8    */
    {GPIO_UNDEF,           .dev = 0, .chan = 18}, /* VBAT */
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */

#define ADC_CLK_MAX         MHZ(36)     /**< Use a faster than default ADC clock */

#define ADC_NUMOF           ARRAY_SIZE(adc_config)

#ifdef __cplusplus
}
#endif

/** @} */
