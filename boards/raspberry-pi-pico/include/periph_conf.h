/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_raspberry-pi-pico
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for Raspberry Pi Pico board
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* put here the board peripherals definitions:
- Available clocks
- Timers
- UARTs
- PWMs
- SPIs
- I2C
- ADC
- RTC
- RTT
etc
 */

#define CLK_XTAL_FREQ (12000000) // 12MHz.

/*
 * 0 = Do not use external crystal oscillator for clk_ref.
 * 1 = Use external crystal oscillator for clk_ref.
 */
#define CLK_REF_XTAL (1)

#define NUM_UART ((unsigned int) 2)

#define STDIO_UART_DEV (0)
#define STDIO_UART_BAUDRATE (115200U)

static const uart_conf_t uart_config[] = {
    {
        .stop_bits = 1,
        .data_bits = 8,
        .parity = false,
        .dev = uart0_hw,
        .rx_pin = GPIO_PIN(GPIO_BANK_USER, 2),
        .tx_pin = GPIO_PIN(GPIO_BANK_USER, 3),
    },
    {
        .stop_bits = 1,
        .data_bits = 8,
        .parity = false,
        .dev = uart1_hw,
        .rx_pin = GPIO_PIN(GPIO_BANK_USER, 4),
        .tx_pin = GPIO_PIN(GPIO_BANK_USER, 5),
    }
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
