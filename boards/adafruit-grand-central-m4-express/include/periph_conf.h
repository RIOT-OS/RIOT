/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-grand-central-m4-express
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Adafruit Grand Central M4 Express
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
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

/* ADC Default values */
#define ADC_PRESCALER                       ADC_CTRLA_PRESCALER_DIV128

#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INTVCC1

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos, dev */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA02, .dev = ADC0 }, /* A0 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA05, .dev = ADC0 }, /* A1 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB03, .dev = ADC0 }, /* A2 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PC00, .dev = ADC1 }, /* A3 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PC01, .dev = ADC1 }, /* A4 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PC02, .dev = ADC1 }, /* A5 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PC03, .dev = ADC1 }, /* A6 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB04, .dev = ADC1 }, /* A7 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB05, .dev = ADC1 }, /* A8 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB06, .dev = ADC1 }, /* A9 */
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB07, .dev = ADC1 }, /* A10 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB08, .dev = ADC0 }, /* A11 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB09, .dev = ADC0 }, /* A12 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA04, .dev = ADC0 }, /* A13 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA06, .dev = ADC0 }, /* A14 */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA07, .dev = ADC0 }, /* A15 */
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

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM3->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PB, 21),   /* D21 (SCL) */
        .sda_pin  = GPIO_PIN(PB, 20),   /* D20 (SDA) */
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_PERIPH,
        .flags    = I2C_FLAG_NONE
    },
    {
        .dev      = &(SERCOM6->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PC, 17),   /* D24 */
        .sda_pin  = GPIO_PIN(PC, 16),   /* D25 */
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_PERIPH,
        .flags    = I2C_FLAG_NONE
    },
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
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
    {   /* SPI on XIO connector *AND* SPI on ISP */
        .dev      = &(SERCOM7->SPI),
        .miso_pin = GPIO_PIN(PD, 11),   /* D50 MISO */
        .mosi_pin = GPIO_PIN(PD, 8),    /* D51 MOSI */
        .clk_pin  = GPIO_PIN(PD, 9),    /* D52 SCK */
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_PERIPH,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM7_DMAC_ID_TX,
        .rx_trigger = SERCOM7_DMAC_ID_RX,
#endif
    },
    {   /* SD Card */
        .dev      = &(SERCOM2->SPI),
        .miso_pin = GPIO_PIN(PB, 29),
        .mosi_pin = GPIO_PIN(PB, 26),
        .clk_pin  = GPIO_PIN(PB, 27),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_PERIPH,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM2_DMAC_ID_TX,
        .rx_trigger = SERCOM2_DMAC_ID_RX,
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
    {
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PB, 25),   /* D0 (UART0_RX) */
        .tx_pin   = GPIO_PIN(PB, 24),   /* D1 (UART0_TX) */
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
    {
        .dev      = &SERCOM4->USART,
        .rx_pin   = GPIO_PIN(PB, 13),   /* D19 (UART2_RX) */
        .tx_pin   = GPIO_PIN(PB, 12),   /* D18 (UART2_TX) */
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
    {
        .dev      = &SERCOM1->USART,
        .rx_pin   = GPIO_PIN(PC, 23),   /* D17 (UART2_RX) */
        .tx_pin   = GPIO_PIN(PC, 22),   /* D16 (UART2_TX) */
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

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom0_2
#define UART_0_ISR_TX       isr_sercom0_0
#define UART_1_ISR          isr_sercom4_2
#define UART_1_ISR_TX       isr_sercom4_0
#define UART_2_ISR          isr_sercom1_2
#define UART_2_ISR_TX       isr_sercom1_0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
