/*
 * Copyright (C) 2015-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stk3700
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the STK3700 starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cpu.h"
#include "periph_cpu.h"
#include "em_cmu.h"
#include "usbdev_cfg_otg_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_HF
#define CLOCK_HF            cmuSelect_HFXO
#endif
#ifndef CLOCK_CORE_DIV
#define CLOCK_CORE_DIV      cmuClkDiv_1
#endif
#ifndef CLOCK_LFA
#define CLOCK_LFA           cmuSelect_LFXO
#endif
#ifndef CLOCK_LFB
#define CLOCK_LFB           cmuSelect_LFXO
#endif
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    {
        .dev = ADC0,
        .cmu = cmuClock_ADC0,
    }
};

static const adc_chan_conf_t adc_channel_config[] = {
    {
        .dev = 0,
        .input = adcSingleInputTemp,
        .reference = adcRef1V25,
        .acq_time = adcAcqTime8
    },
    {
        .dev = 0,
        .input = adcSingleInputVDDDiv3,
        .reference = adcRef1V25,
        .acq_time = adcAcqTime8
    }
};

#define ADC_DEV_NUMOF       ARRAY_SIZE(adc_config)
#define ADC_NUMOF           ARRAY_SIZE(adc_channel_config)
/** @} */

/**
 * @name    DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    {
        .dev = DAC0,
        .ref = dacRefVDD,
        .cmu = cmuClock_DAC0,
    }
};

static const dac_chan_conf_t dac_channel_config[] = {
    {
        .dev = 0,
        .index = 1,
    }
};

#define DAC_DEV_NUMOF       ARRAY_SIZE(dac_config)
#define DAC_NUMOF           ARRAY_SIZE(dac_channel_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = I2C0,
        .sda_pin = GPIO_PIN(PD, 6),
        .scl_pin = GPIO_PIN(PD, 7),
        .loc = I2C_ROUTE_LOCATION_LOC1,
        .cmu = cmuClock_I2C0,
        .irq = I2C0_IRQn,
        .speed = I2C_SPEED_NORMAL
    },
    {
        .dev = I2C1,
        .sda_pin = GPIO_PIN(PC, 4),
        .scl_pin = GPIO_PIN(PC, 5),
        .loc = I2C_ROUTE_LOCATION_LOC0,
        .cmu = cmuClock_I2C1,
        .irq = I2C1_IRQn,
        .speed = I2C_SPEED_NORMAL
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define I2C_0_ISR           isr_i2c0
#define I2C_1_ISR           isr_i2c1
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_chan_conf_t pwm_channel_config[] = {
    {
        .index = 2,
        .pin = GPIO_PIN(PE, 2),
        .loc = TIMER_ROUTE_LOCATION_LOC1
    }
};

static const pwm_conf_t pwm_config[] = {
    {
        .dev = TIMER3,
        .cmu = cmuClock_TIMER3,
        .irq = TIMER3_IRQn,
        .channels = 1,
        .channel = pwm_channel_config
    }
};

#define PWM_DEV_NUMOF       ARRAY_SIZE(pwm_config)
#define PWM_NUMOF           ARRAY_SIZE(pwm_channel_config)
/** @} */

/**
 * @name    RTT configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (1U)              /* in Hz */
#endif
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_dev_t spi_config[] = {
    {
        .dev = USART1,
        .mosi_pin = GPIO_PIN(PD, 0),
        .miso_pin = GPIO_PIN(PD, 1),
        .clk_pin = GPIO_PIN(PD, 2),
        .loc = USART_ROUTE_LOCATION_LOC1,
        .cmu = cmuClock_USART1,
        .irq = USART1_RX_IRQn
    },
    {
        .dev = USART2,
        .mosi_pin = GPIO_UNDEF,
        .miso_pin = GPIO_PIN(PC, 3),
        .clk_pin = GPIO_PIN(PC, 4),
        .loc = USART_ROUTE_LOCATION_LOC0,
        .cmu = cmuClock_USART2,
        .irq = USART2_RX_IRQn
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    Timer configuration
 *
 * The implementation uses two timers in cascade mode.
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .prescaler = {
            .dev = TIMER0,
            .cmu = cmuClock_TIMER0
        },
        .timer = {
            .dev = TIMER1,
            .cmu = cmuClock_TIMER1
        },
        .irq = TIMER1_IRQn,
        .channel_numof = 3
    },
    {
        .prescaler = {
            .dev = NULL,
            .cmu = cmuClock_LETIMER0
        },
        .timer = {
            .dev = LETIMER0,
            .cmu = cmuClock_LETIMER0
        },
        .irq = LETIMER0_IRQn,
        .channel_numof = 2
    }
};

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
#define TIMER_0_ISR         isr_timer1
#define TIMER_1_ISR         isr_letimer0
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = UART0,
        .rx_pin = GPIO_PIN(PE, 1),
        .tx_pin = GPIO_PIN(PE, 0),
        .loc = UART_ROUTE_LOCATION_LOC1,
        .cmu = cmuClock_UART0,
        .irq = UART0_RX_IRQn
    },
    {
        .dev = LEUART0,
        .rx_pin = GPIO_PIN(PD, 5),
        .tx_pin = GPIO_PIN(PD, 4),
        .loc = LEUART_ROUTE_LOCATION_LOC0,
        .cmu = cmuClock_LEUART0,
        .irq = LEUART0_IRQn
    }
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define UART_0_ISR_RX       isr_uart0_rx
#define UART_1_ISR_RX       isr_leuart0
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
