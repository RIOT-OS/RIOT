/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qn9080dk
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for QN9080DK base board board
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev          = I2C1,
        .pin_scl      = GPIO_PIN(PORT_A, 6),
        .pin_sda      = GPIO_PIN(PORT_A, 7),
        .speed        = I2C_SPEED_FAST,
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev          = USART0,
        .rx_pin       = GPIO_PIN(PORT_A, 17),
        .tx_pin       = GPIO_PIN(PORT_A, 16),
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
#define TIMER_NUMOF           4
/** @} */

/* put here the board peripherals definitions:
   - Available clocks
   - PWMs
   - SPIs
   - ADC
   - RTC
   - RTT
   etc
 */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
