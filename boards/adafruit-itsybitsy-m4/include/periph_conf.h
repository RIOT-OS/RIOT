/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-itsybitsy-m4
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Adafruit ItsyBitsy M4
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    desired core clock frequency
 * @{
 */
#ifndef CLOCK_CORECLOCK
#define CLOCK_CORECLOCK     MHZ(120)
#endif
/** @} */

/**
 * @name    32kHz Oscillator configuration
 * @{
 */
#define EXTERNAL_OSC32_SOURCE                    0
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE      1
/** @} */

/**
 * @brief Enable the internal DC/DC converter
 *        The board is equipped with the necessary inductor.
 */
#define USE_VREG_BUCK       (1)

/**
 * @name Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC0,
        .irq            = TC0_IRQn,
        .mclk           = &MCLK->APBAMASK.reg,
        .mclk_mask      = MCLK_APBAMASK_TC0 | MCLK_APBAMASK_TC1,
        .gclk_id        = TC0_GCLK_ID,
        .gclk_src       = SAM0_GCLK_TIMER,
        .flags          = TC_CTRLA_MODE_COUNT32,
    },
    {   /* Timer 1 */
        .dev            = TC2,
        .irq            = TC2_IRQn,
        .mclk           = &MCLK->APBBMASK.reg,
        .mclk_mask      = MCLK_APBBMASK_TC2 | MCLK_APBBMASK_TC3,
        .gclk_id        = TC2_GCLK_ID,
        .gclk_src       = SAM0_GCLK_TIMER,
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    2
#define TIMER_0_ISR         isr_tc0

/* Timer 1 configuration */
#define TIMER_1_CHANNELS    2
#define TIMER_1_ISR         isr_tc2

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = &SERCOM3->USART,
        .rx_pin   = GPIO_PIN(PA, 16),
        .tx_pin   = GPIO_PIN(PA, 17),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom3_2
#define UART_0_ISR_TX       isr_sercom3_0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_0_EN            1

#if PWM_0_EN
/* PWM0 channels */
static const pwm_conf_chan_t pwm_chan0_config[] = {
    /* GPIO pin, MUX value, TCC channel */
    { GPIO_PIN(PA, 22), GPIO_MUX_G, 2 },
};
#endif

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
#if PWM_0_EN
    { .tim  = TCC_CONFIG(TCC0),
      .chan = pwm_chan0_config,
      .chan_numof = ARRAY_SIZE(pwm_chan0_config),
      .gclk_src = SAM0_GCLK_PERIPH,
    },
#endif
};

/* number of devices that are actually defined */
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &(SERCOM1->SPI),
        .miso_pin = GPIO_PIN(PB, 23),
        .mosi_pin = GPIO_PIN(PA, 0),
        .clk_pin  = GPIO_PIN(PA, 1),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_PERIPH,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM1_DMAC_ID_TX,
        .rx_trigger = SERCOM1_DMAC_ID_RX,
#endif
    },
#ifdef MODULE_PERIPH_SPI_ON_QSPI
    {    /* QSPI in SPI mode */
        .dev      = QSPI,
        .miso_pin = SAM0_QSPI_PIN_DATA_1,
        .mosi_pin = SAM0_QSPI_PIN_DATA_0,
        .clk_pin  = SAM0_QSPI_PIN_CLK,
        .miso_mux = SAM0_QSPI_MUX,
        .mosi_mux = SAM0_QSPI_MUX,
        .clk_mux  = SAM0_QSPI_MUX,
        .miso_pad = SPI_PAD_MISO_0,         /* unused */
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,   /* unused */
        .gclk_src = SAM0_GCLK_MAIN,         /* unused */
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = QSPI_DMAC_ID_TX,
        .rx_trigger = QSPI_DMAC_ID_RX,
#endif
    },
#endif
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM2->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 12),
        .sda_pin  = GPIO_PIN(PA, 13),
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_PERIPH,
        .flags    = I2C_FLAG_NONE
    },
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (32768U)
#endif
/** @} */

/**
 * @name USB peripheral configuration
 * @{
 */
static const sam0_common_usb_config_t sam_usbdev_config[] = {
    {
        .dm     = GPIO_PIN(PA, 24),
        .dp     = GPIO_PIN(PA, 25),
        .d_mux  = GPIO_MUX_H,
        .device = &USB->DEVICE,
        .gclk_src = SAM0_GCLK_PERIPH,
    }
};
/** @} */

/**
 * @name ADC Configuration
 * @{
 */

/* ADC Default values */
#define ADC_PRESCALER                       ADC_CTRLA_PRESCALER_DIV128

#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INTVCC1
#define ADC_DEV                             ADC0

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    {GPIO_PIN(PA, 2), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN0)},
    {GPIO_PIN(PA, 5), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN5)},
    {GPIO_PIN(PB, 8), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN2)},
    {GPIO_PIN(PB, 9), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN3)},
    {GPIO_PIN(PA, 4), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN4)},
    {GPIO_PIN(PA, 6), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN6)},
};

#define ADC_NUMOF                           ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name DAC configuration
 * @{
 */
                            /* Must not exceed 12 MHz */
#define DAC_CLOCK           SAM0_GCLK_TIMER
                            /* Use external reference voltage on PA03 */
                            /* (You have to manually connect PA03 with Vcc) */
                            /* Internal reference only gives 1V */
#define DAC_VREF            DAC_CTRLB_REFSEL_VREFPU
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
