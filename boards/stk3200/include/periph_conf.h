/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stk3200
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the STK3200 starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cpu.h"
#include "mutex.h"

#include "periph_cpu.h"

#include "em_cmu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_HF            cmuSelect_HFXO
#define CLOCK_CORE_DIV      cmuClkDiv_1
#define CLOCK_LFA           cmuSelect_LFXO
#define CLOCK_LFB           cmuSelect_CORELEDIV2
/** @} */

/**
 * @brief ADC configuration
 * @{
 */
static const adc_chan_conf_t adc_channel_config[] = {
    {
        adcSingleInputTemp,                 /* channel to use */
        adcRef1V25,                         /* channel reference */
        adcAcqTime8                         /* acquisition time */
    }
};

static const adc_conf_t adc_config[] = {
    {
        ADC0,                               /* device */
        cmuClock_ADC0,                      /* CMU register */
        1,                                  /* number of channels */
        0,                                  /* channel offset */
    }
};

#define ADC_NUMOF           1
#define ADC_0_EN            1
#define ADC_MAX_CHANNELS    1
/** @} */

/**
 * @brief I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        I2C0,                               /* device */
        GPIO_PIN(PE, 12),                   /* SDA pin */
        GPIO_PIN(PE, 13),                   /* SCL pin */
        I2C_ROUTE_LOCATION_LOC6,            /* AF location */
        cmuClock_I2C0,                      /* CMU register */
        I2C0_IRQn,                          /* IRQ base channel */
        MUTEX_INIT                          /* mutex initializer */
    }
};

#define I2C_NUMOF           1
/** @} */

/**
 * @brief   PWM configuration
 * @{
 */
static const pwm_chan_conf_t pwm_channel_config[] = {
    /* no channels */
};

static const pwm_conf_t pwm_config[] = {
    /* no timers left */
};

#define PWM_NUMOF                    0
/** @} */

/**
 * @brief   RTT and RTC configuration
 * @{
 */
#ifndef RTT_AS_RTC
#define RTT_AS_RTC                   (0U)
#endif

#if RTT_AS_RTC
#define RTC_NUMOF                    (1U)
#define RTT_NUMOF                    (0U)
#else
#define RTC_NUMOF                    (0U)
#define RTT_NUMOF                    (1U)
#endif

#define RTT_MAX_VALUE                (0xFFFFFF)
#define RTT_FREQUENCY                (1U)
/** @} */

/**
 * @brief SPI configuration
 * @{
 */
static const spi_dev_t spi_config[] = {
    {
        USART1,                             /* device */
        GPIO_PIN(PD, 7),                    /* MOSI pin */
        GPIO_PIN(PD, 6),                    /* MISO pin */
        GPIO_PIN(PC, 15),                   /* CLK pin */
        USART_ROUTE_LOCATION_LOC1,          /* AF location */
        cmuClock_USART1,                    /* CMU register */
        USART1_RX_IRQn,                     /* IRQ base channel */
        MUTEX_INIT                          /* mutex initializer */
    }
};

#define SPI_0_EN            1
#define SPI_NUMOF           1
/** @} */

/**
 * @brief   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        {
            TIMER0,             /* lower numbered timer */
            cmuClock_TIMER0,    /* pre-scaler bit in the CMU register */
        },
        {
            TIMER1,             /* higher numbered timer, this is the one */
            cmuClock_TIMER1,    /* pre-scaler bit in the CMU register */
        },
        TIMER1_IRQn,            /* IRQn of the higher numbered timer */
    }
};

#define TIMER_0_ISR         isr_timer1
#define TIMER_0_MAX_VALUE   (0xffff)
#define TIMER_NUMOF         1

/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        LEUART0,                            /* device */
        GPIO_PIN(PD, 5),                    /* RX pin */
        GPIO_PIN(PD, 4),                    /* TX pin */
        LEUART_ROUTE_LOCATION_LOC0,         /* AF location */
        cmuClock_LEUART0,                   /* CMU register */
        LEUART0_IRQn                        /* IRQ base channel */
    },
    {
        USART1,                             /* device */
        GPIO_PIN(PD, 6),                    /* RX pin */
        GPIO_PIN(PD, 7),                    /* TX pin */
        USART_ROUTE_LOCATION_LOC2,          /* AF location */
        cmuClock_USART1,                    /* CMU register */
        USART1_RX_IRQn                      /* IRQ base channel */
    }
};

#define UART_0_ISR_RX       isr_leuart0
#define UART_1_ISR_RX       isr_usart1_rx
#define UART_NUMOF          2
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */