/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_remote
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Re-Mote board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
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
#define CLOCK_CORECLOCK     (32000000U) /* 32MHz */
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
#define I2C_0_SCL_PIN           GPIO_PB1
#define I2C_0_SDA_PIN           GPIO_PB0

static const i2c_conf_t i2c_config[I2C_NUMOF] = {
    {
        .scl_pin = GPIO_PC3,
        .sda_pin = GPIO_PC2,
    },
};
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           2
#define SPI_0_EN            1
#define SPI_1_EN            1

#ifdef HAVE_PERIPH_SPI_CONF_T
static const periph_spi_conf_t spi_config[SPI_NUMOF] = {
    {
        .dev      = SSI0,
        .mosi_pin = GPIO_PD0,
        .miso_pin = GPIO_PC4,
        .sck_pin  = GPIO_PD1,
        .cs_pin   = GPIO_PD3,
    },
    {
        .dev      = SSI1,
        .mosi_pin = GPIO_PC7,
        .miso_pin = GPIO_PA4,
        .sck_pin  = GPIO_PB5,
    },
};
#endif

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

/**
 * @brief Port config
 *
 * These defines configures the port settings
 */
/* GPIO 0 configuration - LED1 Green */
#define GPIO_0_PIN          GPIO_PD5
/* GPIO 1 configuration - LED2 Blue */
#define GPIO_1_PIN          GPIO_PC3
/* GPIO 2 configuration - LED3 Red */
#define GPIO_2_PIN          GPIO_PD2
/* GPIO 3 configuration - UART0 RX */
#define GPIO_3_PIN          GPIO_PA0
/* GPIO 4 configuration - UART0 TX */
#define GPIO_4_PIN          GPIO_PA1
/* GPIO 5 configuration - UART1 TX */
#define GPIO_5_PIN          GPIO_PC5
/* GPIO 6 configuration - UART1 RX */
#define GPIO_6_PIN          GPIO_PC6
/* GPIO 7 configuration - UART1 CTS */
#define GPIO_7_PIN          GPIO_PC1
/* GPIO 8 configuration - UART RTS */
#define GPIO_8_PIN          GPIO_PC2
/* GPIO 9 configuration - User button/Bootloader */
#define GPIO_9_PIN          GPIO_PA3
/* GPIO 10 configuration - ADC2 */
#define GPIO_10_PIN         GPIO_PA6
/* GPIO 11 configuration - ADC3 */
#define GPIO_11_PIN         GPIO_PA7
/* GPIO 12 configuration - SSI0 CC1120 CLK */
#define GPIO_12_PIN         GPIO_PD1
/* GPIO 13 configuration - SSI0 CC1120 MOSI */
#define GPIO_13_PIN         GPIO_PD0
/* GPIO 14 configuration - SSI0 CC1120 MISO */
#define GPIO_14_PIN         GPIO_PC4
/* GPIO 15 configuration - I2C SCL */
#define GPIO_15_PIN         GPIO_PB1
/* GPIO 16 configuration - I2C SDA */
#define GPIO_16_PIN         GPIO_PB0
/* GPIO 17 configuration - RF SWITCH */
#define GPIO_17_PIN         GPIO_PD4
/* GPIO 18 configuration - SSI1 MicroSD CLK */
#define GPIO_18_PIN         GPIO_PB5
/* GPIO 19 configuration - SSI1 MicroSD MOSI */
#define GPIO_19_PIN         GPIO_PC7
/* GPIO 20 configuration - SSI1 MicroSD MISO */
#define GPIO_20_PIN         GPIO_PA4
/* GPIO 21 configuration - CC1120 CSn */
#define GPIO_21_PIN         GPIO_PD3
/* GPIO 22 configuration - CC1120 GDO0 */
#define GPIO_22_PIN         GPIO_PB4
/* GPIO 23 configuration - CC1120 GDO2 */
#define GPIO_23_PIN         GPIO_PB3
/* GPIO 24 configuration - CC1120 Reset */
#define GPIO_24_PIN         GPIO_PB2
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
