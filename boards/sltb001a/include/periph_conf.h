/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sltb001a
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for the SLTB001A starter kit
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cpu.h"

#include "periph_cpu.h"

#include "em_cmu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
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
#ifndef CLOCK_LFE
#define CLOCK_LFE           cmuSelect_LFXO
#endif
/** @} */

/**
 * @brief   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    {
        ADC0,                               /* device */
        cmuClock_ADC0,                      /* CMU register */
    }
};

static const adc_chan_conf_t adc_channel_config[] = {
    {
        0,                                  /* device index */
        adcPosSelTEMP,                      /* channel to use */
        adcRef1V25,                         /* channel reference */
        adcAcqTime8                         /* acquisition time */
    },
    {
        0,                                  /* device index */
        adcPosSelAVDD,                      /* channel to use */
        adcRef5V,                           /* channel reference */
        adcAcqTime8                         /* acquisition time */
    }
};

#define ADC_NUMOF           (2U)
/** @} */

/**
 * @brief   DAC configuration
 */
#define DAC_NUMOF           (0U)

/**
 * @brief   I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        I2C0,                               /* device */
        GPIO_PIN(PC, 10),                   /* SDA pin */
        GPIO_PIN(PC, 11),                   /* SCL pin */
        I2C_ROUTELOC0_SDALOC_LOC15 |
            I2C_ROUTELOC0_SCLLOC_LOC15,     /* AF location */
        cmuClock_I2C0,                      /* CMU register */
        I2C0_IRQn                           /* IRQ base channel */
    }
};

#define I2C_NUMOF           (1U)
#define I2C_0_ISR           isr_i2c0
/** @} */

/**
 * @brief   PWM configuration
 * @{
 */
static const pwm_chan_conf_t pwm_channel_config[] = {
    /* no available channels */
};

static const pwm_conf_t pwm_config[] = {
    /* no available timers */
};

#define PWM_NUMOF                    (0U)
/** @} */

/**
 * @brief   RTC configuration
 */
#define RTC_NUMOF           (1U)

/**
 * @brief   RTT configuration
 * @{
 */
#define RTT_NUMOF           (1U)

#define RTT_MAX_VALUE       (0xFFFFFFFF)
#define RTT_FREQUENCY       (1U)
/** @} */

/**
 * @brief   SPI configuration
 * @{
 */
static const spi_dev_t spi_config[] = {
    {
        USART1,                             /* device */
        GPIO_PIN(PC, 6),                    /* MOSI pin */
        GPIO_PIN(PC, 7),                    /* MISO pin */
        GPIO_PIN(PC, 8),                    /* CLK pin */
        USART_ROUTELOC0_RXLOC_LOC11 |
            USART_ROUTELOC0_TXLOC_LOC11 |
            USART_ROUTELOC0_CLKLOC_LOC11,   /* AF location */
        cmuClock_USART1,                    /* CMU register */
        USART1_RX_IRQn                      /* IRQ base channel */
    }
};

#define SPI_NUMOF           (1U)
#define SPI_0_EN            1
/** @} */

/**
 * @brief   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        {
            TIMER0,             /* lower numbered timer */
            cmuClock_TIMER0     /* pre-scaler bit in the CMU register */
        },
        {
            TIMER1,             /* higher numbered timer, this is the one */
            cmuClock_TIMER1     /* pre-scaler bit in the CMU register */
        },
        TIMER1_IRQn             /* IRQn of the higher numbered timer */
    }
};

#define TIMER_NUMOF         (1U)
#define TIMER_0_ISR         isr_timer1
#define TIMER_0_MAX_VALUE   (0xffff)
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        USART0,                             /* device */
        GPIO_PIN(PA, 1),                    /* RX pin */
        GPIO_PIN(PA, 0),                    /* TX pin */
        USART_ROUTELOC0_RXLOC_LOC0 |
            USART_ROUTELOC0_TXLOC_LOC0,     /* AF location */
        cmuClock_USART0,                    /* CMU register */
        USART0_RX_IRQn                      /* IRQ base channel */
    },
    {
        USART1,                             /* device */
        GPIO_PIN(PC, 6),                    /* RX pin */
        GPIO_PIN(PC, 7),                    /* TX pin */
        USART_ROUTELOC0_RXLOC_LOC11 |
            USART_ROUTELOC0_TXLOC_LOC11,    /* AF location */
        cmuClock_USART1,                    /* CMU register */
        USART1_RX_IRQn                      /* IRQ base channel */
    },
    {
        LEUART0,                            /* device */
        GPIO_PIN(PD, 11),                   /* RX pin */
        GPIO_PIN(PD, 10),                   /* TX pin */
        LEUART_ROUTELOC0_RXLOC_LOC18 |
            LEUART_ROUTELOC0_TXLOC_LOC18,   /* AF location */
        cmuClock_LEUART0,                   /* CMU register */
        LEUART0_IRQn                        /* IRQ base channel */
    }
};

#define UART_NUMOF          (3U)
#define UART_0_ISR_RX       isr_usart0_rx
#define UART_1_ISR_RX       isr_usart1_rx
#define UART_2_ISR_RX       isr_leuart0
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
