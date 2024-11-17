/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the Atmel SAM R21 Xplained
 *              Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   External oscillator and clock configuration
 *
 * There are three choices for selection of CORECLOCK:
 *
 * - usage of the 48 MHz DFLL fed by external oscillator running at 32 kHz
 * - usage of the PLL fed by the internal 8MHz oscillator divided by 8
 * - usage of the internal 8MHz oscillator directly, divided by N if needed
 *
 *
 * The PLL option allows for the usage of a wider frequency range and a more
 * stable clock with less jitter. This is why this option is default.
 *
 * The target frequency is computed from the PLL multiplier and the PLL divisor.
 * Use the following formula to compute your values:
 *
 * CORECLOCK = ((PLL_MUL + 1) * 1MHz) / PLL_DIV
 *
 * NOTE: The PLL circuit does not run with less than 32MHz while the maximum PLL
 *       frequency is 96MHz. So PLL_MULL must be between 31 and 95!
 *
 *
 * The internal Oscillator used directly can lead to a slightly better power
 * efficiency to the cost of a less stable clock. Use this option when you know
 * what you are doing! The actual core frequency is adjusted as follows:
 *
 * CORECLOCK = 8MHz / DIV
 *
 * NOTE: A core clock frequency below 1MHz is not recommended
 *
 * @{
 */
#define CLOCK_USE_PLL               (1)
#define CLOCK_USE_XOSC32_DFLL       (0)
/*
 * 0: use XOSC32K (always 32.768kHz) to clock GCLK2
 * 1: use OSCULP32K factory calibrated (~32.768kHz) to clock GCLK2
 *
 * OSCULP32K is factory calibrated to be around 32.768kHz but this values can
 * be of by a couple off % points, so prefer XOSC32K as default configuration.
 */
#define GEN2_ULP32K                 (0)

#if CLOCK_USE_PLL
/* edit these values to adjust the PLL output frequency */
#define CLOCK_PLL_MUL       (47U)               /* must be >= 31 & <= 95 */
#define CLOCK_PLL_DIV       (1U)                /* adjust to your needs */
/* generate the actual used core clock frequency */
#define CLOCK_CORECLOCK     (((CLOCK_PLL_MUL + 1) * 1000000U) / CLOCK_PLL_DIV)
#elif CLOCK_USE_XOSC32_DFLL
    /* Settings for 32 kHz external oscillator and 48 MHz DFLL */
#define CLOCK_CORECLOCK     (48000000U)
#define CLOCK_XOSC32K       (32768UL)
#define CLOCK_8MHZ          (1)
#else
/* edit this value to your needs */
#define CLOCK_DIV           (1U)
/* generate the actual core clock frequency */
#define CLOCK_CORECLOCK     (8000000 / CLOCK_DIV)
#endif
/** @} */

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC3,
        .irq            = TC3_IRQn,
        .pm_mask        = PM_APBCMASK_TC3,
        .gclk_ctrl      = GCLK_CLKCTRL_ID_TCC2_TC3,
#if CLOCK_USE_PLL || CLOCK_USE_XOSC32_DFLL
        .gclk_src       = SAM0_GCLK_1MHZ,
#else
        .gclk_src       = SAM0_GCLK_MAIN,
#endif
        .flags          = TC_CTRLA_MODE_COUNT16,
    },
    {   /* Timer 1 */
        .dev            = TC4,
        .irq            = TC4_IRQn,
        .pm_mask        = PM_APBCMASK_TC4 | PM_APBCMASK_TC5,
        .gclk_ctrl      = GCLK_CLKCTRL_ID_TC4_TC5,
#if CLOCK_USE_PLL || CLOCK_USE_XOSC32_DFLL
        .gclk_src       = SAM0_GCLK_1MHZ,
#else
        .gclk_src       = SAM0_GCLK_MAIN,
#endif
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

#define TIMER_0_MAX_VALUE   0xffff

/* interrupt function name mapping */
#define TIMER_0_ISR         isr_tc3
#define TIMER_1_ISR         isr_tc4

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = &SERCOM0->USART,
        .rx_pin   = GPIO_PIN(PA,5),
        .tx_pin   = GPIO_PIN(PA,4),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_PIN(PA,6),
        .cts_pin  = GPIO_PIN(PA,7),
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
#ifdef MODULE_PERIPH_UART_HW_FC
        .tx_pad   = UART_PAD_TX_0_RTS_2_CTS_3,
#else
        .tx_pad   = UART_PAD_TX_0,
#endif
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    },
    {
        .dev      = &SERCOM5->USART,
        .rx_pin   = GPIO_PIN(PA,23),
        .tx_pin   = GPIO_PIN(PA,22),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_PIN(PB,22),
        .cts_pin  = GPIO_PIN(PB,23),
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
#ifdef MODULE_PERIPH_UART_HW_FC
        .tx_pad   = UART_PAD_TX_0_RTS_2_CTS_3,
#else
        .tx_pad   = UART_PAD_TX_0,
#endif
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    }
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
#define PWM_0_EN            1
#define PWM_1_EN            1

#if PWM_0_EN
/* PWM0 channels */
static const pwm_conf_chan_t pwm_chan0_config[] = {
    /* GPIO pin, MUX value, TCC channel */
    { .pin = GPIO_PIN(PA, 6), .mux = GPIO_MUX_E, .chan = 0 },
    { .pin = GPIO_PIN(PA, 7), .mux = GPIO_MUX_E, .chan = 1 },
};
#endif
#if PWM_1_EN
/* PWM1 channels */
static const pwm_conf_chan_t pwm_chan1_config[] = {
    /* GPIO pin, MUX value, TCC channel */
    { .pin = GPIO_PIN(PA, 16), .mux = GPIO_MUX_F, .chan = 0 },
    { .pin = GPIO_PIN(PA, 18), .mux = GPIO_MUX_F, .chan = 2 },
    { .pin = GPIO_PIN(PA, 19), .mux = GPIO_MUX_F, .chan = 3 },
};
#endif

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
#if PWM_0_EN
    {
        .tim = TCC_CONFIG(TCC1),
        .chan = pwm_chan0_config,
        .chan_numof = ARRAY_SIZE(pwm_chan0_config),
        .gclk_src = SAM0_GCLK_MAIN,
    },
#endif
#if PWM_1_EN
    {
        .tim = TCC_CONFIG(TCC0),
        .chan = pwm_chan1_config,
        .chan_numof = ARRAY_SIZE(pwm_chan1_config),
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
        .dev      = &SERCOM4->SPI,
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
    },
    {
        .dev      = &SERCOM5->SPI,
        .miso_pin = GPIO_PIN(PB, 2),
        .mosi_pin = GPIO_PIN(PB, 22),
        .clk_pin  = GPIO_PIN(PB, 23),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM5_DMAC_ID_TX,
        .rx_trigger = SERCOM5_DMAC_ID_RX,
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
        .dev      = &(SERCOM3->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 17),
        .sda_pin  = GPIO_PIN(PA, 16),
        .mux      = GPIO_MUX_D,
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
     }
};
#define I2C_NUMOF          ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (32768U)    /* in Hz. For changes see `rtt.c` */
#endif
#define RTT_MIN_OFFSET      (10U)
/** @} */

/**
 * @name ADC Configuration
 * @{
 */

/* ADC Default values */
#define ADC_PRESCALER                       ADC_CTRLB_PRESCALER_DIV512

#define ADC_NEG_INPUT                       ADC_INPUTCTRL_MUXNEG_GND
#define ADC_GAIN_FACTOR_DEFAULT             ADC_INPUTCTRL_GAIN_1X
#define ADC_REF_DEFAULT                     ADC_REFCTRL_REFSEL_INT1V

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA06 },      /* EXT1, pin 3 */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA07 },      /* EXT1, pin 4 */
};

#define ADC_NUMOF                           ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name USB peripheral configuration
 * @{
 */
static const sam0_common_usb_config_t sam_usbdev_config[] = {
    {
        .dm     = GPIO_PIN(PA, 24),
        .dp     = GPIO_PIN(PA, 25),
        .d_mux  = GPIO_MUX_G,
        .device = &USB->DEVICE,
        .gclk_src = SAM0_GCLK_MAIN,
    }
};
/** @} */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
