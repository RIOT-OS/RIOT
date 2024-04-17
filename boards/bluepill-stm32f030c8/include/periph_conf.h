/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_bluepill-stm32f030c8
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the bluepill-stm32f030c8 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* This board provides a LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE        1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE        1
#endif

#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .base     = periph_timer1,
        .dev      = TIM1,
        .max      = 0x0000ffff,
        .bus      = APB2,
        .irqn     = TIM1_CC_IRQn
    },
    {
        .base     = periph_timer3,
        .dev      = TIM3,
        .max      = 0x0000ffff,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    },
};

#define TIMER_0_ISR         (isr_tim1_cc)
#define TIMER_1_ISR         (isr_tim3)

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB2,
        .irqn       = USART1_IRQn
    }
};

#define UART_0_ISR          (isr_usart1)

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
        .chan     = { { .pin = GPIO_PIN(PORT_A,  6), .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_A,  7), .cc_chan = 1},
                      { .pin = GPIO_UNDEF,           .cc_chan = 0},
                      { .pin = GPIO_UNDEF,           .cc_chan = 0} },
        .af       = GPIO_AF1,
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
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_B, 1),
        .mosi_af  = GPIO_AF0,
        .miso_af  = GPIO_AF0,
        .sclk_af  = GPIO_AF0,
        .cs_af    = GPIO_AF0,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { GPIO_PIN(PORT_A, 0), 0 },
    { GPIO_PIN(PORT_A, 1), 1 },
    { GPIO_PIN(PORT_A, 2), 2 },
    { GPIO_PIN(PORT_A, 3), 3 },
    { GPIO_PIN(PORT_A, 4), 4 },
    { GPIO_PIN(PORT_A, 5), 5 }
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
