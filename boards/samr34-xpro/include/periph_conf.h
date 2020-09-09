/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr34-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Microchip SAM R34
 *              Xplained Pro board.
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
#define CLOCK_CORECLOCK     (48000000U)

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
        .gclk_src       = SAM0_GCLK_8MHZ,
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    2
#define TIMER_0_ISR         isr_tc0
#define TIMER_NUMOF         (sizeof(timer_config)/sizeof(timer_config[0]))
/** @} */


/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Virtual COM Port */
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PA, 5),
        .tx_pin   = GPIO_PIN(PA, 4),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
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
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM4_DMAC_ID_TX,
        .rx_trigger = SERCOM4_DMAC_ID_RX,
#endif
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
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
     }
};
#define I2C_NUMOF          (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name    RTC configuration
 * @{
 */
#define EXTERNAL_OSC32_SOURCE                   1
#define INTERNAL_OSC32_SOURCE                   0
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE     0
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

/* ADC 0 Default values */
#define ADC_PRESCALER                           ADC_CTRLB_PRESCALER_DIV256

#define ADC_NEG_INPUT                           ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                         ADC_REFCTRL_REFSEL_INTVCC2

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    {GPIO_PIN(PA, 6), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN6)},
    {GPIO_PIN(PA, 7), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN7)}
};


#define ADC_NUMOF                               ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name USB peripheral configuration
 * @{
 */
static const sam0_common_usb_config_t sam_usbdev_config[] = {
    {
        .dm       = GPIO_PIN(PA, 24),
        .dp       = GPIO_PIN(PA, 25),
        .d_mux    = GPIO_MUX_G,
        .device   = &USB->DEVICE,
        .gclk_src = SAM0_GCLK_48MHZ,
    }
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
