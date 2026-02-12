/*
 * SPDX-FileCopyrightText: 2024 Prime Controls, Inc.(R)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-h753zi
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-h753zi board
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#ifndef CONFIG_BOARD_HAS_LSE
#  define CONFIG_BOARD_HAS_LSE    1
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#  define CONFIG_BOARD_HAS_HSE    1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_timer_tim5_and_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA configuration
 * @note    STM32H7 peripherals (D2 domain) use DMA1/DMA2.
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 4 },   /* DMA1 Stream 4 - USART3_TX */
    { .stream = 14 },  /* DMA2 Stream 6 - USART6_TX */
    { .stream = 6 },   /* DMA1 Stream 6 - USART2_TX */
};

#define DMA_0_ISR  isr_dma1_stream4
#define DMA_1_ISR  isr_dma2_stream6
#define DMA_2_ISR  isr_dma1_stream6

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    UART configuration
 * @note    STM32H753ZI uses APB1L/APB2 buses (D2 Domain)
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1LENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 0,
        .dma_chan   = 46 /* DMAMUX_REQ_USART3_TX */
#endif
    },
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_G, 9),
        .tx_pin     = GPIO_PIN(PORT_G, 14),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 1,
        .dma_chan   = 72 /* DMAMUX_REQ_USART6_TX */
#endif
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1LENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = 44 /* DMAMUX_REQ_USART2_TX */
#endif
    }
};

#define UART_0_ISR          (isr_usart3)
#define UART_1_ISR          (isr_usart6)
#define UART_2_ISR          (isr_usart2)
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_B, 5),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_A, 4), /* For HW NSS */
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    {
        .dev      = SPI4,
        .mosi_pin = GPIO_PIN(PORT_E, 6),
        .miso_pin = GPIO_PIN(PORT_E, 5),
        .sclk_pin = GPIO_PIN(PORT_E, 2),
        .cs_pin   = GPIO_PIN(PORT_E, 4), /* For HW NSS */
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI4EN,
        .apbbus   = APB2
    }
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name   ADC configuration
 * @note   STM32H7 ADC is 16-bit, and uses independent clocks
 * @{
 */
static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_A, 3), 0, 15},
    {GPIO_PIN(PORT_C, 0), 1, 10},
    {GPIO_PIN(PORT_C, 3), 0, 13},
    {GPIO_PIN(PORT_B, 1), 1, 5},
    {GPIO_PIN(PORT_C, 2), 0, 12},
    {GPIO_PIN(PORT_F, 10), 2, 6},
    {GPIO_UNDEF, 2, 17}, /* VBAT */
};

/**
 * @brief   ADC line for internal VBAT channel
 */
#define VBAT_ADC            ADC_LINE(6)
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_E, 9),  .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_E, 11), .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_E, 13), .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_E, 14), .cc_chan = 3} },
        .af       = GPIO_AF1,
        .bus      = APB2
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1LENR_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_D, 12), .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_D, 13), .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_D, 14), .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_D, 15), .cc_chan = 3} },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
