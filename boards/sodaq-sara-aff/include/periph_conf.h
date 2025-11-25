/*
 * Copyright (C) 2017 Kees Bakker, SODAQ
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-sara-aff
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the SODAQ SARA AFF boards
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"
#include "cfg_clock_default.h"
#include "cfg_rtc_default.h"
#include "cfg_rtt_default.h"
#include "cfg_spi_default.h"
#include "cfg_timer_default.h"
#include "cfg_usbdev_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name UART configuration
 * @{
 * See Table 7-1 of the SAM D21 Datasheet (p. 29)
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = &SERCOM5->USART,
        .rx_pin   = GPIO_PIN(PB, 30),  /* D0, RX Pin */
        .tx_pin   = GPIO_PIN(PB, 31),  /* D1, TX Pin */
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    },
    {
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PA,5),
        .tx_pin   = GPIO_PIN(PA,6),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_2,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    },
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom5
#define UART_1_ISR          isr_sercom0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name ADC configuration
 * @{
 */

/* ADC Default values */
#define ADC_PRESCALER                       ADC_CTRLB_PRESCALER_DIV512

#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG_GND
#define ADC_GAIN_FACTOR_DEFAULT             ADC_INPUTCTRL_GAIN_DIV2
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INTVCC1

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PB00 },     /* A0 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PB01 },     /* A1 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PB02 },    /* A2 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PB03 },    /* A3 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA08 },    /* A4 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA09 },    /* A5 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA10 },   /* GROVE1/A6 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA11 },   /* GROVE2/A7 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PB05 },    /* BAT_VOLT/A8 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA02 },     /* D2/DAC */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA03 },     /* AREF */
};

#define ADC_NUMOF                           ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM1->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 17),
        .sda_pin  = GPIO_PIN(PA, 16),
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
    }
};
#define I2C_NUMOF          ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
