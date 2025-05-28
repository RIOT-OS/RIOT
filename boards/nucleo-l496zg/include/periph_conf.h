/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nucleo-l496zg
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo144-l496 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"
#include "cfg_usb_otg_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    },
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim5

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_G, 8),
        .tx_pin     = GPIO_PIN(PORT_G, 7),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0,
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR1_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
#ifdef UART_USE_DMA
        .dma_stream = 5,
        .dma_chan   = 4
#endif
    }
};

#define UART_0_ISR          (isr_lpuart1)
#define UART_1_ISR          (isr_usart3)
#define UART_1_DMA_ISR      (isr_dma1_stream5)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_E,  9) /* D6 */, .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_E, 11) /* D5 */, .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_E, 13) /* D3 */, .cc_chan = 2},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0} },
        .af       = GPIO_AF1,
        .bus      = APB2
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR1_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_D, 15) /* D9 */, .cc_chan = 3},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0} },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
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
 * and not in the STM32L496ZG order.  Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5 and the internal VBAT channel.
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
 * For Nucleo-L496ZG this information is in board manual,
 * Table 11, page 38.
 *
 * VBAT is connected ADC1_IN18 or ADC3_IN18 and a voltage divider
 * is used, so that only 1/3 of the actual VBAT is measured. This
 * allows for a supply voltage higher than the reference voltage.
 *
 * For Nucleo-L496ZG more information is provided in MCU datasheet,
 * in section 3.17.3 - Vbat battery voltage monitoring, page 43.
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 3), .dev = 0, .chan =  8 }, /* ADC12_IN8   */
    { .pin = GPIO_PIN(PORT_C, 0), .dev = 0, .chan =  1 }, /* ADC123_IN1  */
    { .pin = GPIO_PIN(PORT_C, 3), .dev = 0, .chan =  4 }, /* ADC123_IN4  */
    { .pin = GPIO_PIN(PORT_C, 1), .dev = 0, .chan =  2 }, /* ADC123_IN2  */
    { .pin = GPIO_PIN(PORT_C, 4), .dev = 0, .chan = 13 }, /* ADC12_IN13  */
    { .pin = GPIO_PIN(PORT_C, 5), .dev = 0, .chan = 14 }, /* ADC12_IN14  */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 18 },
};

/**
 * @brief Number of ADC devices
 */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)

/**
 * @brief VBAT ADC line
 */
#define VBAT_ADC            ADC_LINE(6)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
