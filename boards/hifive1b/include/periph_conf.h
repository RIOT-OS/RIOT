/*
 * Copyright (C) 2019 Ken Rabold
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_hifive1b
 * @{
 *
 * @file
 * @brief       Peripheral specific definitions for the HiFive1b RISC-V board
 *
 * @author      Ken Rabold
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "kernel_defines.h"
#include "macros/units.h"
#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 *
 * @{
 */
#define TIMER_NUMOF                 (1)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .addr       = UART0_CTRL_ADDR,
        .rx         = GPIO_PIN(0, 16),
        .tx         = GPIO_PIN(0, 17),
        .isr_num    = INT_UART0_BASE,
    },
    {
        .addr       = UART1_CTRL_ADDR,
        .rx         = GPIO_PIN(0, 23),
        .tx         = GPIO_PIN(0, 18),
        .isr_num    = INT_UART1_BASE,
    },
};

#define UART_NUMOF                  ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI device configuration
 *
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .addr       = SPI1_CTRL_ADDR,
        .mosi       = GPIO_PIN(0, 3), /* D11 */
        .miso       = GPIO_PIN(0, 4), /* D12 */
        .sclk       = GPIO_PIN(0, 5), /* D13 */
    },
};

#define SPI_NUMOF                  ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    PWM configuration
 *
 * @{
 */
#define PWM_NUMOF                   (3)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .addr   = I2C0_CTRL_ADDR,
        .scl    = GPIO_PIN(0, 13),
        .sda    = GPIO_PIN(0, 12),
        .speed  = I2C_SPEED_NORMAL,
     },
};

#define I2C_NUMOF                   ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
