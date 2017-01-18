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
 * @brief       Peripheral MCU configuration for the Re-Mote board prototype A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "cc2538_gpio.h"
#include "periph_cpu.h"
#include "periph_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

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
        .scl_pin = I2C_0_SCL_PIN,
        .sda_pin = I2C_0_SDA_PIN,
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
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
