/*
 * Copyright (C) 2017 Baptiste Clenet <bapclenet@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr30-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Atmel SAM R30 Xplained Pro board
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GCLK reference speed
 */
#define CLOCK_CORECLOCK     (16000000U)

/**
 * @name    Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF        (1U)
#define TIMER_0_EN         1

/* Timer 0 configuration */
#define TIMER_0_DEV        TC0->COUNT32
#define TIMER_0_CHANNELS   1
#define TIMER_0_MAX_VALUE  (0xffffffff)
#define TIMER_0_ISR        isr_tc0
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Virtual COM Port */
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PA,5),
        .tx_pin   = GPIO_PIN(PA,4),
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = GCLK_PCHCTRL_GEN_GCLK0
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom0

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &(SERCOM4->SPI),
        .miso_pin = GPIO_PIN(PC, 19),
        .mosi_pin = GPIO_PIN(PB, 30),
        .clk_pin  = GPIO_PIN(PC, 18),
        .miso_mux = GPIO_MUX_F,
        .mosi_mux = GPIO_MUX_F,
        .clk_mux  = GPIO_MUX_F,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM1->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 17),
        .sda_pin  = GPIO_PIN(PA, 16),
        .mux      = GPIO_MUX_C,
        .gclk_src = GCLK_PCHCTRL_GEN_GCLK0,
        .flags    = I2C_FLAG_NONE
     }
};
#define I2C_NUMOF          (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name    RTC configuration
 * @{
 */
#define RTC_NUMOF                               (1)
#define EXTERNAL_OSC32_SOURCE                   1
#define INTERNAL_OSC32_SOURCE                   0
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE     0
/** @} */

/**
 * @name    RTT configuration
 * @{
 */
#define RTT_FREQUENCY                           (32768U)
#define RTT_MAX_VALUE                           (0xffffffffU)
#define RTT_NUMOF                               (1)
/** @} */

/**
 * @name ADC Configuration
 * @{
 */
#define ADC_NUMOF                               (5U)

/* ADC 0 Default values */
#define ADC_0_CLK_SOURCE                        0 /* GCLK_GENERATOR_0 */
#define ADC_0_PRESCALER                         ADC_CTRLB_PRESCALER_DIV256

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    {GPIO_PIN(PA, 6), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN6)}, /* EXT1, pin 3 */
    {GPIO_PIN(PA, 7), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN7)}, /* EXT1, pin 4 */
    {GPIO_PIN(PA, 10), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN18)},
    {GPIO_PIN(PA, 11), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN19)},
    {GPIO_PIN(PA, 2), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN0)}
};

#define ADC_0_NEG_INPUT                         ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_0_REF_DEFAULT                       ADC_REFCTRL_REFSEL_INTVCC2
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
