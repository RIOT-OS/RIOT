/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f334r8
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f334r8 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
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
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32F334 order.  Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5
 *
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .dev = 0, .chan =  1 }, /* ADC1_IN1,  fast */
    { .pin = GPIO_PIN(PORT_A, 1), .dev = 0, .chan =  2 }, /* ADC1_IN2,  fast */
    { .pin = GPIO_PIN(PORT_A, 4), .dev = 1, .chan =  1 }, /* ADC2_IN1,  fast */
    { .pin = GPIO_PIN(PORT_B, 0), .dev = 0, .chan = 11 }, /* ADC1_IN11, slow */
    { .pin = GPIO_PIN(PORT_C, 1), .dev = 1, .chan =  7 }, /* ADC12_IN7, slow */
    { .pin = GPIO_PIN(PORT_C, 0), .dev = 1, .chan =  6 }, /* ADC12_IN6, slow */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 1 },    /* DMA1 Channel 2 - SPI1_RX | USART3_TX */
    { .stream = 2 },    /* DMA1 Channel 3 - SPI1_TX */
    { .stream = 3 },    /* DMA1 Channel 4 - USART1_TX */
    { .stream = 6 },    /* DMA1 Channel 7 - USART2_TX */
};

#define DMA_0_ISR  isr_dma1_channel2
#define DMA_1_ISR  isr_dma1_channel3
#define DMA_2_ISR  isr_dma1_channel4
#define DMA_3_ISR  isr_dma1_channel7
#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

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
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 3,
        .dma_chan   = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A,  9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    },
/* SPI1 RX and USART3 are sharing the same DMA channel, so disable the
 * third UART when both SPI and DMA features are enabled. */
#if !defined(MODULE_PERIPH_SPI) || !defined(MODULE_PERIPH_DMA)
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_B, 11),
        .tx_pin     = GPIO_PIN(PORT_B, 10),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 0,
        .dma_chan   = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    }
#endif /* !defined(MODULE_PERIPH_SPI) || !defined(MODULE_PERIPH_DMA) */
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
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
        .dev         = SPI1,
        .mosi_pin    = GPIO_PIN(PORT_A, 7),
        .miso_pin    = GPIO_PIN(PORT_A, 6),
        .sclk_pin    = GPIO_PIN(PORT_A, 5),
        .cs_pin      = GPIO_UNDEF,
        .mosi_af     = GPIO_AF5,
        .miso_af     = GPIO_AF5,
        .sclk_af     = GPIO_AF5,
        .cs_af       = GPIO_AF5,
        .rccmask     = RCC_APB2ENR_SPI1EN,
        .apbbus      = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma      = 1,
        .tx_dma_chan = DMA_CHAN_CONFIG_UNSUPPORTED,
        .rx_dma      = 0,
        .rx_dma_chan = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
