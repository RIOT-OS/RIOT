/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       common UART function fallback implementations
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */
#include <stddef.h>
#include <string.h>

#include "board.h"
#include "cpu.h"
#include "kernel_defines.h"
#include "periph/uart.h"

#if PERIPH_UART_NEEDS_WRITE_STRING
void uart_write_string(uart_t uart, const char *s)
{
    if (!PERIPH_UART_NEEDS_WRITE_BYTE) {
        while (*s) {
            uart_write_byte(uart, (uint8_t)*s++);
        }
    } else {
        uart_write(uart, (uint8_t*)s, strlen(s));
    }
}
#endif

#if PERIPH_UART_NEEDS_WRITE && !PERIPH_UART_NEEDS_WRITE_BYTE
void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (const uint8_t *end = data + len; data != end; ++data) {
        uart_write_byte(uart, *data);
    }
}
#endif
