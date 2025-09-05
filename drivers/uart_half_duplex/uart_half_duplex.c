/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_uart_half_duplex
 * @{
 *
 * @file
 * @brief       Driver implementation for half-duplex UART devices
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "uart_half_duplex.h"

#include "periph/uart.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define IS_SET(dir)   ((dir & 1) == 0)
#define IS_CLEAR(dir) ((dir & 1) == 1)

#define IS_PIN(dir)    (dir >= UART_HALF_DUPLEX_DIR_PIN_SET(0))
#define GET_PIN(dir)   ((dir >> 1) - 1)

static inline void _enable_tx(const uart_half_duplex_t *dev)
{
    if (dev->params.dir.enable_tx) {
        dev->params.dir.enable_tx(dev->params.uart);
    }
}

static inline void _disable_tx(const uart_half_duplex_t *dev)
{
    if (dev->params.dir.disable_tx) {
        dev->params.dir.disable_tx(dev->params.uart);
    }
}

static void _rx_cb(void* data, uint8_t c)
{
    uart_half_duplex_t *dev = data;
    if (dev->size < dev->buffer_max_size) {
        dev->buffer[dev->size++] = c;
    }
}

int uart_half_duplex_init(uart_half_duplex_t *dev, uint8_t *buffer, size_t buffer_max_size, const uart_half_duplex_params_t *params)
{
    if (buffer == NULL || buffer_max_size <= 7) {
        return UART_HALF_DUPLEX_NOBUFF;
    }

    dev->buffer = buffer;
    dev->buffer_max_size = buffer_max_size;
    dev->params = *params;
    dev->timeout_us = UART_HALF_DUPLEX_DEFAULT_TIMEOUT_US;

    if (dev->params.dir.init) {
        dev->params.dir.init(dev->params.uart);
    }

    int ret = uart_init(dev->params.uart, dev->params.baudrate, _rx_cb, dev);

    _disable_tx(dev);
    uart_half_duplex_set_rx(dev);

    return ret;
}

size_t uart_half_duplex_send(const uart_half_duplex_t *dev, size_t size)
{
    _enable_tx(dev);
    uart_write(dev->params.uart, dev->buffer, size);
    _disable_tx(dev);
    return size;
}

size_t uart_half_duplex_recv(const uart_half_duplex_t *dev, size_t size)
{
    const uint32_t begin = xtimer_now_usec();
    while (xtimer_now_usec() - begin < dev->timeout_us) {
        if (dev->size >= size) {
            break;
        }
    }
    return dev->size;
}
