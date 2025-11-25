/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_saml1x
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Microchip
 *              SAML10 & SAML11 Xplained Pro board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
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
 * @brief Enable the internal DC/DC converter
 *        The board is equipped with the necessary inductor.
 */
#define USE_VREG_BUCK       (1)

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC0,
        .irq            = TC0_IRQn,
        .mclk           = &MCLK->APBCMASK.reg,
        .mclk_mask      = MCLK_APBCMASK_TC0 | MCLK_APBCMASK_TC1,
        .gclk_id        = TC0_GCLK_ID,
        .gclk_src       = SAM0_GCLK_MAIN,
        .flags          = TC_CTRLA_MODE_COUNT32,
    },
    {
        .dev            = TC2,
        .irq            = TC2_IRQn,
        .mclk           = &MCLK->APBCMASK.reg,
        .mclk_mask      = MCLK_APBCMASK_TC2,
        .gclk_id        = TC2_GCLK_ID,
        .gclk_src       = SAM0_GCLK_MAIN,
        .flags          = TC_CTRLA_MODE_COUNT16,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    2
#define TIMER_0_ISR         isr_tc0

/* Timer 1 configuration */
#define TIMER_1_CHANNELS    2
#define TIMER_1_ISR         isr_tc2
#define TIMER_1_MAX_VALUE   0xffff

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Virtual COM Port */
        .dev      = &SERCOM2->USART,
        .rx_pin   = GPIO_PIN(PA, 25),
        .tx_pin   = GPIO_PIN(PA, 24),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_3,
        .tx_pad   = UART_PAD_TX_2,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    },
    {    /* EXT1 */
        /* For SAML11, see boards/saml11/doc.txt
         * to properly enable this SERCOM */
        .dev      = &SERCOM1->USART,
        .rx_pin   = GPIO_PIN(PA, 9),
        .tx_pin   = GPIO_PIN(PA, 8),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom2_2
#define UART_0_ISR_TX       isr_sercom2_0
#define UART_1_ISR          isr_sercom1_2
#define UART_1_ISR_TX       isr_sercom1_0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_0_EN            1
#define PWM_1_EN            0

#if PWM_0_EN
/* PWM0 channels */
static const pwm_conf_chan_t pwm_chan0_config[] = {
    /* GPIO pin, MUX value, TC channel */
    { GPIO_PIN(PA, 18), GPIO_MUX_E, 0 },
    { GPIO_PIN(PA, 19), GPIO_MUX_E, 1 },
};
#endif
#if PWM_1_EN
/* PWM1 channels */
static const pwm_conf_chan_t pwm_chan1_config[] = {
    /* GPIO pin, MUX value, TC channel */
    { GPIO_PIN(PA, 7), GPIO_MUX_E, 1 }, /* LED */
};
#endif

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
#if PWM_0_EN
    { .tim  = TC_CONFIG(TC2),
      .chan = pwm_chan0_config,
      .chan_numof = ARRAY_SIZE(pwm_chan0_config),
      .gclk_src = SAM0_GCLK_MAIN,
    },
#endif
#if PWM_1_EN
    /* conflicts with xtimer config (TC0_TC1) */
    { .tim  = TC_CONFIG(TC1),
      .chan = pwm_chan1_config,
      .chan_numof = ARRAY_SIZE(pwm_chan0_config),
      .gclk_src = SAM0_GCLK_MAIN,
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
        .dev      = &(SERCOM0->SPI),
        .miso_pin = GPIO_PIN(PA,  4),
        .mosi_pin = GPIO_PIN(PA, 14),
        .clk_pin  = GPIO_PIN(PA, 15),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM0_DMAC_ID_TX,
        .rx_trigger = SERCOM0_DMAC_ID_RX,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
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
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    RTC configuration
 * @{
 */
#define EXTERNAL_OSC32_SOURCE                    1
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE      0
/** @} */

/**
 * @name    RTT configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (32768U)
#endif
/** @} */

/**
 * @name ADC Configuration
 * @{
 */

/* ADC Default values */
#define ADC_PRESCALER                       ADC_CTRLB_PRESCALER_DIV256

#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INTVCC2

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA10 },
};

#define ADC_NUMOF                           ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name DAC configuration
 * @{
 */
#define DAC_CLOCK           SAM0_GCLK_32KHZ
                            /* use Vcc as reference voltage */
#define DAC_VREF            DAC_CTRLB_REFSEL_AVCC
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
