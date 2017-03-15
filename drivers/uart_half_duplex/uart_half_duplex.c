/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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

static inline void _enable_tx(uart_half_duplex_dir_t dir)
{
    if (IS_PIN(dir)) {
        if (IS_SET(dir)) {
            gpio_set(GET_PIN(dir));
            return;
        }
        if (IS_CLEAR(dir)) {
            gpio_clear(GET_PIN(dir));
            return;
        }
    }
}

static inline void _disable_tx(uart_half_duplex_dir_t dir)
{
    if (IS_PIN(dir)) {
        if (IS_SET(dir)) {
            gpio_clear(GET_PIN(dir));
            return;
        }
        if (IS_CLEAR(dir)) {
            gpio_set(GET_PIN(dir));
            return;
        }
    }
}

static void _rx_cb(void* data, uint8_t c)
{
    uart_half_duplex_t *dev = data;
    if (dev->size < dev->limit) {
        dev->buffer[dev->size++] = c;
    }
}

int uart_half_duplex_init(uart_half_duplex_t *dev, const uart_half_duplex_params_t *params)
{
    if (params->buffer == NULL || params->limit <= 7) {
        return UART_HALF_DUPLEX_NOBUFF;
    }

    dev->uart = params->uart;
    dev->dir = params->dir;
    dev->buffer = params->buffer;
    dev->limit = params->limit;
    dev->timeout_us = UART_HALF_DUPLEX_DEFAULT_TIMEOUT_US;

    if (IS_PIN(dev->dir)) {
        gpio_init(GET_PIN(dev->dir), GPIO_OUT);
    }

    int ret = uart_init(dev->uart, params->baudrate, _rx_cb, dev);

    _disable_tx(dev->dir);
    uart_half_duplex_set_rx(dev);

    return ret;
}

size_t uart_half_duplex_send(uart_half_duplex_t *dev, size_t size)
{
    _enable_tx(dev->dir);
    uart_write(dev->uart, dev->buffer, size);
    _disable_tx(dev->dir);
    return size;
}

size_t uart_half_duplex_recv(uart_half_duplex_t *dev, size_t size)
{
    const uint32_t begin = xtimer_now_usec();
    while(xtimer_now_usec()-begin < dev->timeout_us) {
        if (dev->size >= size) {
            break;
        }
    }
    return dev->size;
}
