/*
 * Copyright (C) 2017 Ken Rabold
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_hifive1
 * @{
 *
 * @file
 * @brief       Peripheral specific definitions for the HiFive1 RISC-V board
 *
 * @author      Ken Rabold
 */

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
        .rx         = GPIO_PIN(0, 18),
        .tx         = GPIO_PIN(0, 23),
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

#ifdef __cplusplus
}
#endif

/** @} */
