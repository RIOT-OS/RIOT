/*
 * Copyright (C)  2016 Freie Universität Berlin
 *                2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mkr1000
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for Arduino MKR1000 board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Bumsik kim <kbumsik@gmail.com>
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
 * @brief   External oscillator and clock configuration
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
/* generate the actual used core clock frequency */
#define CLOCK_CORECLOCK     (((CLOCK_PLL_MUL + 1) * 1000000U) / CLOCK_PLL_DIV)
#else
/* edit this value to your needs */
#define CLOCK_DIV           (1U)
/* generate the actual core clock frequency */
#define CLOCK_CORECLOCK     (8000000 / CLOCK_DIV)
#endif
/** @} */

/**
 * @name Timer peripheral configuration
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
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = &SERCOM5->USART,
        .rx_pin = GPIO_PIN(PB,23),  /* ARDUINO_PIN_13, RX Pin */
        .tx_pin = GPIO_PIN(PB,22),  /* ARDUINO_PIN_14, TX Pin */
        .mux    = GPIO_MUX_D,
        .rx_pad = UART_PAD_RX_3,
        .tx_pad = UART_PAD_TX_2
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom5

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    2
/* for compatibility with test application */
#define PWM_0_CHANNELS      PWM_MAX_CHANNELS
#define PWM_1_CHANNELS      PWM_MAX_CHANNELS

/* PWM device configuration */
static const pwm_conf_t pwm_config[] = {
#if PWM_0_EN
    {TCC0, {
        /* GPIO pin, MUX value, TCC channel */
        { GPIO_PIN(PA, 8), GPIO_MUX_E,  0 },
        { GPIO_PIN(PA, 9), GPIO_MUX_E,  1 },
    }},
#endif
#if PWM_1_EN
    {TCC1, {
        /* GPIO pin, MUX value, TCC channel */
        { GPIO_PIN(PA, 6), GPIO_MUX_E, 0 },
        { GPIO_PIN(PA, 7), GPIO_MUX_E, 1 },
    }},
#endif
};

/* number of devices that are actually defined */
#define PWM_NUMOF           (2U)
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_0_EN                           1
#define ADC_MAX_CHANNELS                   14
/* ADC 0 device configuration */
#define ADC_0_DEV                          ADC
#define ADC_0_IRQ                          ADC_IRQn

/* ADC 0 Default values */
#define ADC_0_CLK_SOURCE                   0 /* GCLK_GENERATOR_0 */
#define ADC_0_PRESCALER                    ADC_CTRLB_PRESCALER_DIV512

#define ADC_0_NEG_INPUT                    ADC_INPUTCTRL_MUXNEG_GND
#define ADC_0_GAIN_FACTOR_DEFAULT          ADC_INPUTCTRL_GAIN_1X
#define ADC_0_REF_DEFAULT                  ADC_REFCTRL_REFSEL_INT1V

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    {GPIO_PIN(PA, 2), ADC_INPUTCTRL_MUXPOS_PIN0},     /* A0 */
    {GPIO_PIN(PB, 2), ADC_INPUTCTRL_MUXPOS_PIN10},    /* A1 */
    {GPIO_PIN(PB, 3), ADC_INPUTCTRL_MUXPOS_PIN11},    /* A2 */
    {GPIO_PIN(PA, 4), ADC_INPUTCTRL_MUXPOS_PIN4},     /* A3 */
    {GPIO_PIN(PA, 5), ADC_INPUTCTRL_MUXPOS_PIN5},     /* A4 */
    {GPIO_PIN(PA, 6), ADC_INPUTCTRL_MUXPOS_PIN6},     /* A5 */
    {GPIO_PIN(PA, 7), ADC_INPUTCTRL_MUXPOS_PIN7},     /* A6 */
};

#define ADC_0_CHANNELS                     (7U)
#define ADC_NUMOF                          ADC_0_CHANNELS
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &SERCOM1->SPI,
        .miso_pin = GPIO_PIN(PA, 19),   /* ARDUINO_PIN_8, SERCOM1-MISO */
        .mosi_pin = GPIO_PIN(PA, 16),   /* ARDUINO_PIN_10, SERCOM1-MOSI */
        .clk_pin  = GPIO_PIN(PA, 17),   /* ARDUINO_PIN_9, SERCOM1-SCK */
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1
    },
    {   /* SPI Pins connected to WINC1500 wifi module */
        .dev      = &SERCOM2->SPI,
        .miso_pin = GPIO_PIN(PA, 15),
        .mosi_pin = GPIO_PIN(PA, 12),
        .clk_pin  = GPIO_PIN(PA, 13),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_3,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_1
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF          (1U)
#define I2C_0_EN            1
#define I2C_1_EN            0
#define I2C_2_EN            0
#define I2C_3_EN            0
#define I2C_IRQ_PRIO        1

#define I2C_0_DEV           SERCOM0->I2CM
#define I2C_0_IRQ           SERCOM0_IRQn
#define I2C_0_ISR           isr_sercom0
/* I2C 0 GCLK */
#define I2C_0_GCLK_ID       SERCOM0_GCLK_ID_CORE
#define I2C_0_GCLK_ID_SLOW  SERCOM0_GCLK_ID_SLOW
/* I2C 0 pin configuration */
#define I2C_0_SDA           GPIO_PIN(PA, 8) /* SERCOM0-SDA, on-board pull-up */
#define I2C_0_SCL           GPIO_PIN(PA, 9) /* SERCOM0-SCL, on-board pull-up */
#define I2C_0_MUX           GPIO_MUX_C
/** @} */

/**
 * @name RTC configuration
 * @{
 */
#define RTC_NUMOF           (1U)
#define RTC_DEV             RTC->MODE2
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_DEV             RTC->MODE0
#define RTT_IRQ             RTC_IRQn
#define RTT_IRQ_PRIO        10
#define RTT_ISR             isr_rtc
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (32768U)    /* in Hz. For changes see `rtt.c` */
#define RTT_RUNSTDBY        (1)         /* Keep RTT running in sleep states */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
