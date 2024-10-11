/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Inria
 *               2017 HAW-Hamburg
 *               2018 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-l452re
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-l452re board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR1_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_C, 11),
        .tx_pin     = GPIO_PIN(PORT_C, 10),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart3)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR1_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 4), .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7), .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_C, 8), .cc_chan = 2},
                      { .pin = GPIO_PIN(PORT_C, 9), .cc_chan = 3} },
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
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @brief ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32L452RE order. Instead, we
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
 * For Nucleo-L452RE this information is in board manual,
 * Table 22, page 51, or STM32L452RE MCU datasheet,
 * Table 16, page 63.
 *
 * VBAT is connected ADC1_IN18 or ADC3_IN18 and a voltage divider
 * is used, so that only 1/3 of the actual VBAT is measured. This
 * allows for a supply voltage higher than the reference voltage.
 *
 * For STM32L452RE more information is provided in MCU datasheet,
 * in section 3.15.3 - Vbat battery voltage monitoring, page 40.
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .dev = 0, .chan =  5 }, /* A0  ADC1_IN5  */
    { .pin = GPIO_PIN(PORT_A, 1), .dev = 0, .chan =  6 }, /* A1  ADC1_IN6  */
    { .pin = GPIO_PIN(PORT_A, 4), .dev = 0, .chan =  9 }, /* A2  ADC1_IN9  */
    { .pin = GPIO_PIN(PORT_B, 0), .dev = 0, .chan = 15 }, /* A3 ADC1_IN15  */
    { .pin = GPIO_PIN(PORT_C, 1), .dev = 0, .chan =  2 }, /* A4  ADC1_IN2  */
    { .pin = GPIO_PIN(PORT_C, 0), .dev = 0, .chan =  1 }, /* A5  ADC1_IN1  */
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
#endif /* PERIPH_CONF_H */
