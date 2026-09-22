/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "periph_gba.h"

#include "periph/uart.h"

static uart_rx_cb_t uart_cb = NULL;
static void *uart_cb_arg = NULL;

static inline uint16_t _baud_to_flag(uint32_t baud)
{
    switch (baud) {
    case 9600:
        return 0;
    case 38400:
        return 1;
    case 57600:
        return 2;
    case 115200:
    default:
        return 3;
    }
}

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg)
{
    (void) uart;
    GBA_SIO_RCNT = 0;

    GBA_UART->baud = _baud_to_flag(baud);
    GBA_UART->cts = 0; /* disable / always send */
    GBA_UART->parity = 0; /* even */
    GBA_UART->data_bits = 1; /* 8 bits */
    GBA_UART->fifo_enable = 0;
    GBA_UART->parity_enable = 0;
    GBA_UART->send_enable = 1;
    GBA_UART->receive_enable = 1;
    GBA_UART->must_be_1_for_uart = 3;
    GBA_UART->irq_enable = 1;

    uart_cb = rx_cb;
    uart_cb_arg = arg;

    GBA_IE = 0; /* todo flaf for uart */
    return 0;
}

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    (void) uart;
    (void) data_bits;
    (void) parity;
    (void) stop_bits;
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    (void) uart;
    int i = 0;
    while (len > 0) {
        /* wait for hardware to get ready */
        while (GBA_UART->send_data) {}
        GBA_UART->data = data[i++];
        len--;
    }
}