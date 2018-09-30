/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sensebox_samd21
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for SenseBox MCU with SAMD21
 *
 * @author      Jose Alamos <jose.alamos@haw-hamburg.de>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
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
 * @name    External oscillator and clock configuration
 *
 * For selection of the used CORECLOCK, we have implemented two choices:
 *
 * - usage of the PLL fed by the internal 8MHz oscillator divided by 8
 * - usage of the internal 8MHz oscillator directly, divided by N if needed
 *
 *
 * The PLL option allows for the usage of a wider frequency range and a more
 * stable clock with less jitter. This is why we use this option as default.
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
#define CLOCK_USE_PLL       (1)

#if CLOCK_USE_PLL
/* edit these values to adjust the PLL output frequency */
#define CLOCK_PLL_MUL       (47U)               /* must be >= 31 & <= 95 */
#define CLOCK_PLL_DIV       (1U)                /* adjust to your needs */
#define CLOCK_CORECLOCK     (((CLOCK_PLL_MUL + 1) * 1000000U) / CLOCK_PLL_DIV)
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
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TC3->COUNT16
#define TIMER_0_CHANNELS    2
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_0_ISR         isr_tc3

/* Timer 1 configuration */
#define TIMER_1_DEV         TC4->COUNT32
#define TIMER_1_CHANNELS    2
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_ISR         isr_tc4
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = &SERCOM3->USART,
        .rx_pin = GPIO_PIN(PA, 23),
        .tx_pin = GPIO_PIN(PA, 22),
        .mux    = GPIO_MUX_C,
        .rx_pad = UART_PAD_RX_1,
        .tx_pad = UART_PAD_TX_0,
        .flags  = UART_FLAG_NONE,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0,
    },
    {
        .dev    = &SERCOM4->USART,
        .rx_pin = GPIO_PIN(PB, 9),
        .tx_pin = GPIO_PIN(PB, 8),
        .mux    = GPIO_MUX_D,
        .rx_pad = UART_PAD_RX_1,
        .tx_pad = UART_PAD_TX_0,
        .flags  = UART_FLAG_NONE,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0,
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom3
#define UART_1_ISR          isr_sercom4

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
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
        .clk_pin  = GPIO_PIN(PA, 17),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM0->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 8),
        .sda_pin  = GPIO_PIN(PA, 9),
        .mux      = GPIO_MUX_C,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0,
        .flags    = I2C_FLAG_NONE
    },
    {
        .dev      = &(SERCOM2->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 12),
        .sda_pin  = GPIO_PIN(PA, 13),
        .mux      = GPIO_MUX_C,
        .gclk_src = GCLK_CLKCTRL_GEN_GCLK0,
        .flags    = I2C_FLAG_NONE
    }
};

#define I2C_NUMOF          (sizeof(i2c_config) / sizeof(i2c_config[0]))

/**
 * @name    RTC configuration
 * @{
 */
#define RTC_NUMOF           (1U)
#define RTC_DEV             RTC->MODE2

/** @} */

/**
 * @name    ADC configuration
 * @{
 */
#define ADC_0_EN                           1
/* ADC 0 device configuration */
#define ADC_0_DEV                          ADC
#define ADC_0_IRQ                          ADC_IRQn

/* ADC 0 Default values */
#define ADC_0_CLK_SOURCE                   0 /* GCLK_GENERATOR_0 */
#define ADC_0_PRESCALER                    ADC_CTRLB_PRESCALER_DIV512

#define ADC_0_NEG_INPUT                    ADC_INPUTCTRL_MUXNEG_GND
#define ADC_0_GAIN_FACTOR_DEFAULT          ADC_INPUTCTRL_GAIN_1X
#define ADC_0_REF_DEFAULT                  ADC_REFCTRL_REFSEL_INT1V

/* Digital pins (1 to 6) on the board can be configured as analog inputs */
static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    { GPIO_PIN(PA, 4), ADC_INPUTCTRL_MUXPOS_PIN4 },     /* Digital 1 */
    { GPIO_PIN(PA, 5), ADC_INPUTCTRL_MUXPOS_PIN5 },     /* Digital 2 */
    { GPIO_PIN(PA, 6), ADC_INPUTCTRL_MUXPOS_PIN6 },     /* Digital 3 */
    { GPIO_PIN(PA, 7), ADC_INPUTCTRL_MUXPOS_PIN7 },     /* Digital 4 */
    { GPIO_PIN(PA, 3), ADC_INPUTCTRL_MUXPOS_PIN1 },     /* Digital 5 */
    { GPIO_PIN(PA, 2), ADC_INPUTCTRL_MUXPOS_PIN0 },     /* Digital 6 */
};

#define ADC_0_CHANNELS                     (6U)
#define ADC_NUMOF                          ADC_0_CHANNELS
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
