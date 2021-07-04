/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup boards_raspberry-pi-pico
 * @{
 *
 * @file
 * @brief Configuration of CPU peripherals of Raspberry Pi Pico
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLK_XTAL_HZ (12000000UL)

#define UART_NUMOF (2U)
#define TIMER_NUMOF (1U)
#define TIMER_CHANNEL_NUMOF (4U)

#define STDIO_UART_DEV (UART_DEV(0))
#define STDIO_UART_BAUDRATE (115200U)

static const uart_conf_t uart_config[] = {
    {
        .dev = uart0_hw,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1,
        .data_bits = UART_DATA_BITS_8,
        .tx_pin = GPIO_PIN(GPIO_BANK_USER, 0),
        .rx_pin = GPIO_PIN(GPIO_BANK_USER, 1),
    },
    {
        .dev = uart1_hw,
        .parity = UART_PARITY_NONE,
        .stop_bits = UART_STOP_BITS_1,
        .data_bits = UART_DATA_BITS_8,
        .tx_pin = GPIO_PIN(GPIO_BANK_USER, 4),
        .rx_pin = GPIO_PIN(GPIO_BANK_USER, 5),
    }
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
