/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_cc2538dk
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the CC2538DK board
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer peripheral configuration
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
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          1

#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0

#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART0
#define UART_0_IRQ          UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_TX_PIN       GPIO_PA1
#define UART_0_RX_PIN       GPIO_PA0
#define UART_0_RTS_PIN      GPIO_PD3
#define UART_0_CTS_PIN      GPIO_PB0

/* UART 1 device configuration */
#define UART_1_DEV          UART1
#define UART_1_IRQ          UART1_IRQn
#define UART_1_ISR          isr_uart1
/* UART 1 pin configuration */
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
#define I2C_0_SCL_PIN           GPIO_PA2 /* SPI_SCK on the SmartRF06 baseboard */
#define I2C_0_SDA_PIN           GPIO_PA4 /* SPI_MOSI on the SmartRF06 baseboard */

static const i2c_conf_t i2c_config[I2C_NUMOF] = {
    {
        .scl_pin = GPIO_PA2, /* SPI_SCK on the SmartRF06 baseboard */
        .sda_pin = GPIO_PA4, /* SPI_MOSI on the SmartRF06 baseboard */
    },
};
/** @} */

/**
 * @brief   Pre-calculated clock divider values based on a CLOCK_CORECLOCK (32MHz)
 *
 * Calculated with (CPSR * (SCR + 1)) = (CLOCK_CORECLOCK / bus_freq), where
 * 1 < CPSR < 255 and
 * 0 < SCR  < 256
 */
static const spi_clk_conf_t spi_clk_config[] = {
    { .cpsr = 10, .scr = 31 },  /* 100khz */
    { .cpsr =  2, .scr = 39 },  /* 400khz */
    { .cpsr =  2, .scr = 15 },  /* 1MHz */
    { .cpsr =  2, .scr =  2 },  /* ~4.5MHz */
    { .cpsr =  2, .scr =  1 }   /* ~10.7MHz */
};

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SSI0,
        .mosi_pin = GPIO_PA4,
        .miso_pin = GPIO_PA5,
        .sck_pin  = GPIO_PA2,
        .cs_pin   = GPIO_PD0
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
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

#endif /* PERIPH_CONF_H */
/** @} */
