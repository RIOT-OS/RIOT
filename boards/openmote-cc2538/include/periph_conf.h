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
#define TIMER_NUMOF         (4U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1
#define TIMER_3_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         GPTIMER0
#define TIMER_0_CHANNELS    1
#define TIMER_0_MAX_VALUE   0xffffffff
#define TIMER_0_IRQn_1      GPTIMER_0A_IRQn
#define TIMER_0_IRQn_2      GPTIMER_0B_IRQn
#define TIMER_0_ISR_1       isr_timer0_chan0
#define TIMER_0_ISR_2       isr_timer0_chan1

/* Timer 1 configuration */
#define TIMER_1_DEV         GPTIMER1
#define TIMER_1_CHANNELS    1
#define TIMER_1_MAX_VALUE   0xffffffff
#define TIMER_1_IRQn_1      GPTIMER_1A_IRQn
#define TIMER_1_IRQn_2      GPTIMER_1B_IRQn
#define TIMER_1_ISR_1       isr_timer1_chan0
#define TIMER_1_ISR_2       isr_timer1_chan1

/* Timer 2 configuration */
#define TIMER_2_DEV         GPTIMER2
#define TIMER_2_CHANNELS    1
#define TIMER_2_MAX_VALUE   0xffffffff
#define TIMER_2_IRQn_1      GPTIMER_2A_IRQn
#define TIMER_2_IRQn_2      GPTIMER_2B_IRQn
#define TIMER_2_ISR_1       isr_timer2_chan0
#define TIMER_2_ISR_2       isr_timer2_chan1

/* Timer 3 configuration */
#define TIMER_3_DEV         GPTIMER3
#define TIMER_3_CHANNELS    1
#define TIMER_3_MAX_VALUE   0xffffffff
#define TIMER_3_IRQn_1      GPTIMER_3A_IRQn
#define TIMER_3_IRQn_2      GPTIMER_3B_IRQn
#define TIMER_3_ISR_1       isr_timer3_chan0
#define TIMER_3_ISR_2       isr_timer3_chan1
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

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
