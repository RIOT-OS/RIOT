/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_same51-curiosity-nano
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Microchip SAM E51
 *              Curiosity Nano board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

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
#define EXTERNAL_OSC32_SOURCE                    1
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE      0
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
 * @name CAN configuration
 * @{
 */
/** Available CAN interfaces */
static const can_conf_t candev_conf[] = {
    {
        .can = CAN0,
        .rx_pin = GPIO_PIN(PA, 22),
        .tx_pin = GPIO_PIN(PB, 23),
        .gclk_src = SAM0_GCLK_PERIPH,
    },
    {
        .can = CAN1,
        .rx_pin = GPIO_PIN(PB, 15),
        .tx_pin = GPIO_PIN(PB, 14),
        .gclk_src = SAM0_GCLK_PERIPH,
    }
};

/** CAN 0 ISR configuration */
#define ISR_CAN0            isr_can0

/** CAN 1 ISR configuration */
#define ISR_CAN1            isr_can1

/** Number of CAN interfaces */
#define CAN_NUMOF           ARRAY_SIZE(candev_conf)
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {   /* Virtual COM Port */
        .dev      = &SERCOM5->USART,
        .rx_pin   = GPIO_PIN(PB, 16),
        .tx_pin   = GPIO_PIN(PB, 17),
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
    {   /* shared with CAN1 */
        .dev      = &SERCOM4->USART,
        .rx_pin   = GPIO_PIN(PB, 13),
        .tx_pin   = GPIO_PIN(PB, 12),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_PIN(PB, 14),
        .cts_pin  = GPIO_PIN(PB, 15),
#endif
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
#ifdef MODULE_PERIPH_UART_HW_FC
        .tx_pad   = UART_PAD_TX_0_RTS_2_CTS_3,
#else
        .tx_pad   = UART_PAD_TX_0,
#endif
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
    {
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PA, 9),
        .tx_pin   = GPIO_PIN(PA, 8),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_PIN(PA, 10),
        .cts_pin  = GPIO_PIN(PA, 10),
#endif
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
#ifdef MODULE_PERIPH_UART_HW_FC
        .tx_pad   = UART_PAD_TX_0_RTS_2_CTS_3,
#else
        .tx_pad   = UART_PAD_TX_0,
#endif
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    },
    {   /* shared with CAN0 */
        .dev      = &SERCOM3->USART,
        .rx_pin   = GPIO_PIN(PA, 23),
        .tx_pin   = GPIO_PIN(PA, 22),
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_PERIPH,
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom5_2
#define UART_0_ISR_TX       isr_sercom5_0

#define UART_1_ISR          isr_sercom4_2
#define UART_1_ISR_TX       isr_sercom4_0

#define UART_2_ISR          isr_sercom0_2
#define UART_2_ISR_TX       isr_sercom0_0

#define UART_3_ISR          isr_sercom3_2
#define UART_3_ISR_TX       isr_sercom3_0

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name PWM configuration
 * @{
 */

/* PWM0 channels */
static const pwm_conf_chan_t pwm_chan0_config[] = {
    /* GPIO pin, MUX value, TCC channel */
    {
        .pin  = GPIO_PIN(PA, 14),   /* LED0 */
        .mux  = GPIO_MUX_F,
        .chan = 0,
    },
};

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
    {
        .tim  = TCC_CONFIG(TCC2),
        .chan = pwm_chan0_config,
        .chan_numof = ARRAY_SIZE(pwm_chan0_config),
        .gclk_src = SAM0_GCLK_48MHZ,
    },
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
        .dev      = &SERCOM1->SPI,
        .miso_pin = GPIO_PIN(PA, 19),
        .mosi_pin = GPIO_PIN(PA, 16),
        .clk_pin  = GPIO_PIN(PA, 27),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1,
        .gclk_src = SAM0_GCLK_PERIPH,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM1_DMAC_ID_TX,
        .rx_trigger = SERCOM1_DMAC_ID_RX,
#endif

    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &SERCOM2->I2CM,
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 13),
        .sda_pin  = GPIO_PIN(PA, 12),
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
 *
 * Pins are only routed to solder pads on the board, no connector populated
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
#define ADC_GCLK_SRC                        SAM0_GCLK_PERIPH    /**< clock used for ADC */
#define ADC_PRESCALER                       ADC_CTRLA_PRESCALER_DIV8

#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INTVCC1

static const adc_conf_chan_t adc_channels[] = {
    /* inputctrl, dev */
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA07, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA04, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB09, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA11, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA10, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB08, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB03, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA02, .dev = ADC0 },
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB04, .dev = ADC1 },
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB05, .dev = ADC1 },
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB06, .dev = ADC1 },
    { .inputctrl = ADC1_INPUTCTRL_MUXPOS_PB07, .dev = ADC1 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB01, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PB00, .dev = ADC0 },
    { .inputctrl = ADC0_INPUTCTRL_MUXPOS_PA03, .dev = ADC0 },
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

/** @} */
