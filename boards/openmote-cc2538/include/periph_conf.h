/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_openmote-cc2538
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the OpenMote-cc2538 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "cc2538_gpio.h"
#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (32000000U)         /* desired core clock frequency, 32MHz */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = GPTIMER0,
        .channels = 2,
        .cfg      = GPTMCFG_16_BIT_TIMER, /* required for XTIMER */
    },
    {
        .dev      = GPTIMER1,
        .channels = 1,
        .cfg      = GPTMCFG_32_BIT_TIMER,
    },
    {
        .dev      = GPTIMER2,
        .channels = 1,
        .cfg      = GPTMCFG_32_BIT_TIMER,
    },
    {
        .dev      = GPTIMER3,
        .channels = 1,
        .cfg      = GPTMCFG_32_BIT_TIMER,
    },
};

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
#define TIMER_IRQ_PRIO      1
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (7)

#define AD4_PIN             2   /* Single Channel Input to pin AIN2 */
#define CTS_DI07_PIN        3   /* Single Channel Input to pin AIN3 */
#define AD5_PIN             4   /* Single Channel Input to pin AIN4 */
#define AD6_PIN             5   /* Single Channel Input to pin AIN5 */
#define ON_SLEEP_PIN        6   /* Single Channel Input to pin AIN6 */
#define DIFF_PINS_2_3       9   /* Differential Input to pins AIN2-AIN3 */
#define DIFF_PINS_4_5       10  /* Differential Input to pins AIN4-AIN5 */

static const uint8_t periph_adc_map[ADC_NUMOF] = {
    AD4_PIN,
    CTS_DI07_PIN,
    AD5_PIN,
    AD6_PIN,
    ON_SLEEP_PIN,
    DIFF_PINS_2_3,
    DIFF_PINS_4_5,
};

#define ADC_LINE(x)         periph_adc_map[x]
#define SOC_ADC_ADCCON_REF  SOC_ADC_ADCCON_REF_AVDD5
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART0
#define UART_0_IRQ          UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_TX_PIN       GPIO_PA1
#define UART_0_RX_PIN       GPIO_PA0
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF               1
#define I2C_0_EN                1
#define I2C_IRQ_PRIO            1

/* I2C 0 device configuration */
#define I2C_0_DEV               0
#define I2C_0_IRQ               I2C_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c
#define I2C_0_SCL_PIN           GPIO_PB3 /* OpenBattery */
#define I2C_0_SDA_PIN           GPIO_PB4 /* OpenBattery */

static const i2c_conf_t i2c_config[I2C_NUMOF] = {
    {
        .scl_pin = GPIO_PB3, /* OpenBattery */
        .sda_pin = GPIO_PB4, /* OpenBattery */
    },
};
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           1
#define SPI_0_EN            1

static const periph_spi_conf_t spi_config[SPI_NUMOF] = {
    {
        .dev      = SSI0,
        .mosi_pin = GPIO_PA5,
        .miso_pin = GPIO_PA4,
        .sck_pin  = GPIO_PA2,
        .cs_pin   = GPIO_PA3,
    },
};

/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_IRQ_PRIO       1

#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
#define GPIO_8_EN           1
#define GPIO_9_EN           1
#define GPIO_10_EN          1
#define GPIO_11_EN          1
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1
#define GPIO_16_EN          1
#define GPIO_17_EN          1
#define GPIO_18_EN          1
#define GPIO_19_EN          1
#define GPIO_20_EN          1
#define GPIO_21_EN          1
#define GPIO_22_EN          1
#define GPIO_23_EN          1
#define GPIO_24_EN          1
#define GPIO_25_EN          1
#define GPIO_26_EN          1
#define GPIO_27_EN          1
#define GPIO_28_EN          1
#define GPIO_29_EN          1
#define GPIO_30_EN          1
#define GPIO_31_EN          1

/* GPIO channel configuration */
#define GPIO_0_PIN          GPIO_PA0
#define GPIO_1_PIN          GPIO_PA1
#define GPIO_2_PIN          GPIO_PA2
#define GPIO_3_PIN          GPIO_PA3
#define GPIO_4_PIN          GPIO_PA4
#define GPIO_5_PIN          GPIO_PA5
#define GPIO_6_PIN          GPIO_PA6
#define GPIO_7_PIN          GPIO_PA7
#define GPIO_8_PIN          GPIO_PB0
#define GPIO_9_PIN          GPIO_PB1
#define GPIO_10_PIN         GPIO_PB2
#define GPIO_11_PIN         GPIO_PB3
#define GPIO_12_PIN         GPIO_PB4
#define GPIO_13_PIN         GPIO_PB5
#define GPIO_14_PIN         GPIO_PB6
#define GPIO_15_PIN         GPIO_PB7
#define GPIO_16_PIN         GPIO_PC0
#define GPIO_17_PIN         GPIO_PC1
#define GPIO_18_PIN         GPIO_PC2
#define GPIO_19_PIN         GPIO_PC3
#define GPIO_20_PIN         GPIO_PC4
#define GPIO_21_PIN         GPIO_PC5
#define GPIO_22_PIN         GPIO_PC6
#define GPIO_23_PIN         GPIO_PC7
#define GPIO_24_PIN         GPIO_PD0
#define GPIO_25_PIN         GPIO_PD1
#define GPIO_26_PIN         GPIO_PD2
#define GPIO_27_PIN         GPIO_PD3
#define GPIO_28_PIN         GPIO_PD4
#define GPIO_29_PIN         GPIO_PD5
#define GPIO_30_PIN         GPIO_PD6
#define GPIO_31_PIN         GPIO_PD7
/** @} */

/**
 * @name Radio peripheral configuration
 * @{
 */
#define RADIO_IRQ_PRIO      1
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
