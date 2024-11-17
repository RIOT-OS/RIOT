/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-metro-m4-express
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Adafruit Metro M4 Express
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Core clock frequency
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
#define EXTERNAL_OSC32_SOURCE                   1
#define INTERNAL_OSC32_SOURCE                   0
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE     0
/** @} */

/**
 * @brief Enable the internal DC/DC converter
 *        The board is equipped with L_EXT (L1) of 10uH at VSW pin.
 */
#define USE_VREG_BUCK       (1)

/**
 * @name ADC Configuration
 * @{
 */
#define ADC_GCLK_SRC                        SAM0_GCLK_PERIPH    /**< clock used for ADC */
#define ADC_PRESCALER                       ADC_CTRLA_PRESCALER_DIV8
#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INTVCC1

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos, dev */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA02, .dev = ADC0 }, /* A0 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA05, .dev = ADC0 }, /* A1 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA06, .dev = ADC0 }, /* A2 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA04, .dev = ADC0 }, /* A3 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB08, .dev = ADC1 }, /* A4 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB09, .dev = ADC1 }, /* A5 */
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
                            /* (A solder jumper connects PA03 to 3V3 on the
                             * back of the board. We assume the jumper has
                             * not been cut.) */
#define DAC_VREF            DAC_CTRLB_REFSEL_VREFPU
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM5->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PB, 3),   /* D: SERCOM5.1 */
        .sda_pin  = GPIO_PIN(PB, 2),   /* D: SERCOM5.0 */
        .mux      = GPIO_MUX_D,
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
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {   /* SPI on ISP */
        .dev      = &(SERCOM2->SPI),
        .miso_pin = GPIO_PIN(PA, 14), /* C: SERCOM2.2, D: SERCOM4.2 */
        .mosi_pin = GPIO_PIN(PA, 12), /* C: SERCOM2.0, D: SERCOM4.1 */
        .clk_pin  = GPIO_PIN(PA, 13), /* C: SERCOM2.1, D: SERCOM4.0 */
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_2,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_PERIPH,
#if MODULE_PERIPH_DMA
        .tx_trigger = SERCOM2_DMAC_ID_TX,
        .rx_trigger = SERCOM2_DMAC_ID_RX,
#endif
    },
#if !MODULE_PERIPH_UART
    {   /* D11=MOSI, D12=MISO, D13=SCK */
        .dev      = &(SERCOM3->SPI),
        .miso_pin = GPIO_PIN(PA, 17), /* C: SERCOM1.1, D: SERCOM3.0 */
        .mosi_pin = GPIO_PIN(PA, 19), /* C: SERCOM1.3, D: SERCOM3.3 */
        .clk_pin  = GPIO_PIN(PA, 16), /* C: SERCOM1.0, D: SERCOM3.1 */
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_3_SCK_1,
        .gclk_src = SAM0_GCLK_PERIPH,
#  if MODULE_PERIPH_DMA
        .tx_trigger = SERCOM3_DMAC_ID_TX,
        .rx_trigger = SERCOM3_DMAC_ID_RX,
#  endif
    },
#endif
#if MODULE_PERIPH_SPI_ON_QSPI
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
#  if MODULE_PERIPH_DMA
        .tx_trigger = QSPI_DMAC_ID_TX,
        .rx_trigger = QSPI_DMAC_ID_RX,
#  endif
    },
#endif
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
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
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
/* The UART pins can be routed to SERCOM3 (used by SPI) or
 * SERCOM5 (used by I2C). The pad configuration for SERCOM5
 * is impossible, as TXD cannot be routed to pad 1.
 * Hence, we let periph_spi and periph_uart conflict in
 * Makefile.features.
 */
    /* D0 = RXD, D1 = TXD */
    {
        .dev      = &SERCOM3->USART,
        .rx_pin   = GPIO_PIN(PA, 23),   /* C: SERCOM3.1, D: SERCOM5.0 */
        .tx_pin   = GPIO_PIN(PA, 22),   /* C: SERCOM3.0, D: SERCOM5.1 */
  #ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
  #endif
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
