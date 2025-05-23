/*
 * Copyright (C) 2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_xg23-pk6068a
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for xG23-PK6068A board
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#define HFXO_FREQ           (39000000UL)
#define CMU_HFXOINIT        CMU_HFXOINIT_DEFAULT

#define LFXO_FREQ           (32768UL)
#define CMU_LFXOINIT        CMU_LFXOINIT_DEFAULT

static const clk_mux_t clk_mux_config[] = {
    { .clk = cmuClock_SYSCLK,      .src = cmuSelect_HFXO },
    { .clk = cmuClock_EM01GRPACLK, .src = cmuSelect_HFXO },
    { .clk = cmuClock_EM01GRPCCLK, .src = cmuSelect_HFXO },
    { .clk = cmuClock_EM23GRPACLK, .src = cmuSelect_LFXO },
    { .clk = cmuClock_EUSART0CLK,  .src = cmuSelect_HFXO },
};
#define CLK_MUX_NUMOF         ARRAY_SIZE(clk_mux_config)

static const clk_div_t clk_div_config[] = {
    { .clk = cmuClock_HCLK,   .div = 1 },
    { .clk = cmuClock_PCLK,   .div = 1 },
    { .clk = cmuClock_LSPCLK, .div = 2 },
};
#define CLK_DIV_NUMOF         ARRAY_SIZE(clk_div_config)
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    {
        .dev = IADC0,
        .cmu = cmuClock_IADC0,
        .reference = iadcCfgReferenceInt1V2,
        .reference_mV = 1210,
        .gain = iadcCfgAnalogGain0P5x,
        .available_res = { ADC_RES_10BIT, ADC_RES_16BIT }
    }
};

static const adc_chan_conf_t adc_channel_config[] = {
    {
        .dev = 0,
        .input_pos = GPIO_PIN(PA, 10),
        .input_neg = GPIO_UNDEF
    },
    {
        .dev = 0,
        .input_pos = GPIO_PIN(PA, 0),
        .input_neg = GPIO_PIN(PA, 5)
    },
};

#define ADC_DEV_NUMOF       ARRAY_SIZE(adc_config)
#define ADC_NUMOF           ARRAY_SIZE(adc_channel_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = I2C0,
        .sda_pin = GPIO_PIN(PC, 7),
        .scl_pin = GPIO_PIN(PC, 5),
        .cmu = cmuClock_I2C0,
        .irq = I2C0_IRQn,
        .speed = I2C_SPEED_NORMAL
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define I2C_0_ISR           isr_i2c0
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_dev_t spi_config[] = {
    {
        .dev = USART0,
        .mosi_pin = GPIO_PIN(PC, 1),
        .miso_pin = GPIO_PIN(PC, 2),
        .clk_pin = GPIO_PIN(PC, 3),
        .cmu = cmuClock_USART0,
        .irq = USART0_RX_IRQn
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    Timer configuration
 *
 * The implementation can use one low-energy timer
 * or two regular timers in cascade mode.
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIMER0,
        .cmu = cmuClock_TIMER0,
        .irq = TIMER0_IRQn
    },
    {
        .dev = LETIMER0,
        .cmu = cmuClock_LETIMER0,
        .irq = LETIMER0_IRQn
    }
};

#define TIMER_0_ISR         isr_timer0
#define TIMER_1_ISR         isr_letimer0

#define TIMER_0_MAX_VALUE   TIMER_MAX_VALUE
#define TIMER_1_MAX_VALUE   LETIMER_MAX_VALUE

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = EUSART1,
        .rx_pin = GPIO_PIN(PA, 9),
        .tx_pin = GPIO_PIN(PA, 8),
        .cmu = cmuClock_EUSART1,
        .irq = EUSART1_RX_IRQn
    }
};
#define UART_0_ISR_RX       isr_eusart1_rx

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
