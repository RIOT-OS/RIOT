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
