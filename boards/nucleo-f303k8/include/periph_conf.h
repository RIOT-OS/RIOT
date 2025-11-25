/*
 * Copyright (C) 2017   Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f303k8
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f303k8 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
 * and not in the STM32F303 order.  Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A3 and A6 and the internal VBAT channel.
 *
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .dev = 0, .chan = 1 }, /* ADC1_IN1, fast */
    { .pin = GPIO_PIN(PORT_A, 1), .dev = 0, .chan = 2 }, /* ADC1_IN2, fast */
    { .pin = GPIO_PIN(PORT_A, 3), .dev = 0, .chan = 4 }, /* ADC1_IN4, fast */
    { .pin = GPIO_PIN(PORT_A, 4), .dev = 1, .chan = 1 }, /* ADC2_IN1, fast */
    { .pin = GPIO_PIN(PORT_A, 7), .dev = 1, .chan = 4 }, /* ADC2_IN4, fast */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 17 }, /* VBAT */
};

#define VBAT_ADC            ADC_LINE(5) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 1 },    /* DMA1 Channel 2 - SPI1_RX */
    { .stream = 2 },    /* DMA1 Channel 3 - SPI1_TX */
    { .stream = 3 },    /* DMA1 Channel 4 - USART1_TX */
    { .stream = 6 },    /* DMA1 Channel 7 - USART2_TX */
};

#define DMA_0_ISR   isr_dma1_channel2
#define DMA_1_ISR   isr_dma1_channel3
#define DMA_2_ISR   isr_dma1_channel4
#define DMA_3_ISR   isr_dma1_channel7

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
        .rx_pin     = GPIO_PIN(PORT_A, 15),
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
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)

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
        .chan     = { { .pin = GPIO_PIN(PORT_B, 0) /* D3 */, .cc_chan = 2 },
                      { .pin = GPIO_PIN(PORT_B, 1) /* D6 */, .cc_chan = 3 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 } },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 8) /* D9 */, .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                   .cc_chan = 0 } },
        .af       = GPIO_AF6,
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
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 1,
        .tx_dma_chan = DMA_CHAN_CONFIG_UNSUPPORTED,
        .rx_dma   = 0,
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
