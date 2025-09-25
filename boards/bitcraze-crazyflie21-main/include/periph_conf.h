/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_bitcraze_crazyflie21_main
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for Crazyflie 2.1 main application MCU
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

/* This board provides an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#  define CONFIG_BOARD_HAS_LSE 1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#  define CONFIG_BOARD_HAS_HSE 1
#endif

/* The HSE provides a 8MHz clock */
#ifndef CONFIG_CLOCK_HSE
#  define CONFIG_CLOCK_HSE                 MHZ(8)
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_usb_otg_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 11 },   /* DMA2 Stream 3 - SPI1_TX */
    { .stream = 10 },   /* DMA2 Stream 2 - SPI1_RX */
    { .stream = 3 },    /* DMA1 Stream 3 - USART3_TX */
    { .stream = 6 },    /* DMA1 Stream 6 - USART2_TX */
};

#define DMA_0_ISR           isr_dma2_stream3
#define DMA_1_ISR           isr_dma2_stream2
#define DMA_2_ISR           isr_dma1_stream3
#define DMA_3_ISR           isr_dma1_stream6

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name   Timer configuration
 *
 * Avoiding TIM2 and TIM4 because they are used for pwm
 *
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIM5,
        .max = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .bus = APB1,
        .irqn = TIM5_IRQn,
    }
};

#define TIMER_0_ISR         isr_tim5

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin = GPIO_PIN(PORT_C, 11),
        .tx_pin = GPIO_PIN(PORT_C, 10),
        .rx_af = GPIO_AF7,
        .tx_af = GPIO_AF7,
        .bus = APB1,
        .irqn = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = 2,
        .dma_chan = 4
#endif
    },
    {
        .dev = USART2,
        .rcc_mask = RCC_APB1ENR_USART2EN,
        .rx_pin = GPIO_PIN(PORT_A, 3),
        .tx_pin = GPIO_PIN(PORT_A, 2),
        .rx_af = GPIO_AF7,
        .tx_af = GPIO_AF7,
        .bus = APB1,
        .irqn = USART2_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = 3,
        .dma_chan = 4
#endif
    }
};

#define UART_0_ISR          (isr_usart3)
#define UART_1_ISR          (isr_usart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 2), .dev = 0, .chan = 2 },
    { .pin = GPIO_PIN(PORT_A, 3), .dev = 0, .chan = 3 },
    { .pin = GPIO_PIN(PORT_A, 5), .dev = 1, .chan = 5 },
    { .pin = GPIO_PIN(PORT_A, 6), .dev = 1, .chan = 6 },
    { .pin = GPIO_PIN(PORT_A, 7), .dev = 1, .chan = 7 },
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 0}, /* VBAT */
};

/* The radio MCU takes care of power management -> adc line without proper pin. */
#define VBAT_ADC            ADC_LINE(5) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = TIM9,
        .rcc_mask = RCC_APB2ENR_TIM9EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_A, 2), .cc_chan = 0 },
            { .pin = GPIO_PIN(PORT_A, 3), .cc_chan = 1 },
            { .pin = GPIO_UNDEF,          .cc_chan = 0 },
            { .pin = GPIO_UNDEF,          .cc_chan = 0 } },
        .af = GPIO_AF3,
        .bus = APB2,
    },
    {
        .dev = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_A, 15),  .cc_chan = 0 }, /* Motor 3 */
            { .pin = GPIO_PIN(PORT_A, 1),   .cc_chan = 1 }, /* Motor 1 */
            { .pin = GPIO_PIN(PORT_A, 2),   .cc_chan = 2 }, /* broken out */
            { .pin = GPIO_PIN(PORT_B, 11),  .cc_chan = 3 }  /* Motor 2 */ },
        .af = GPIO_AF1,
        .bus = APB1
    },
    {
        .dev = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .chan = {
            { .pin = GPIO_PIN(PORT_B, 9), .cc_chan = 3 }, /* Motor 4 */
            { .pin = GPIO_UNDEF,          .cc_chan = 0 },
            { .pin = GPIO_UNDEF,          .cc_chan = 0 },
            { .pin = GPIO_UNDEF,          .cc_chan = 0 } },
        .af = GPIO_AF2,
        .bus = APB1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin = GPIO_UNDEF,
        .mosi_af = GPIO_AF5,
        .miso_af = GPIO_AF5,
        .sclk_af = GPIO_AF5,
        .cs_af = GPIO_AF5,
        .rccmask = RCC_APB2ENR_SPI1EN,
        .apbbus = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma = 0,
        .tx_dma_chan = 3,
        .rx_dma = 1,
        .rx_dma_chan = 3,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = I2C1,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_B, 6),
        .sda_pin = GPIO_PIN(PORT_B, 7),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF4,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C1EN,
        .clk = CLOCK_APB1,
        .irqn = I2C1_EV_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_ev

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
