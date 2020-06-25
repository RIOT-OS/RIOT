/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the OpenWSN "uart" bsp module.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdatomic.h>

#include "board.h"
#include "periph/uart.h"

#ifdef MODULE_ZTIMER_USEC
#include "ztimer.h"
#endif

#include "openwsn_uart.h"

#define XOFF            0x13
#define XON             0x11
#define XONXOFF_ESCAPE  0x12
#define XONXOFF_MASK    0x10

typedef struct {
    uart_tx_cbt txCb;
    uart_rx_cbt rxCb;
    atomic_bool fXonXoffEscaping;
    atomic_char xonXoffEscapedByte;
} uart_vars_t;

static uart_vars_t _uart_vars;
static atomic_char _uart_rx_byte;

#ifdef MODULE_ZTIMER_USEC
static ztimer_t _ztimer_tx_uart;
#endif

static void _openwsn_uart_write(const uint8_t *data)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        uart_write(OPENWSN_UART_DEV, data, 1);
#ifdef MODULE_ZTIMER_USEC
        ztimer_set(ZTIMER_USEC, &_ztimer_tx_uart, 0);
#endif
    }
}

static void _riot_rx_cb(void *arg, uint8_t data)
{
    (void)arg;
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        _uart_rx_byte = data;
        if (_uart_vars.rxCb) {
            _uart_vars.rxCb();
        }
    }
    else {
        (void)data;
    }
}

static void _riot_tx_cb(void *arg)
{
    (void)arg;

    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        if (_uart_vars.fXonXoffEscaping == 0x01) {
            _uart_vars.fXonXoffEscaping = 0x00;
            _uart_vars.xonXoffEscapedByte ^= XONXOFF_MASK;
            _openwsn_uart_write((uint8_t *)&(_uart_vars.xonXoffEscapedByte));
        }
        else {
            if (_uart_vars.txCb) {
                _uart_vars.txCb();
            }
        }
    }
}

void uart_enableInterrupts(void)
{
    /* unused in RIOT */
}

void uart_disableInterrupts(void)
{
    /* unused in RIOT */
}

void uart_clearRxInterrupts(void)
{
    /* unused in RIOT */
}

void uart_clearTxInterrupts(void)
{
    /* unused in RIOT */
}

void uart_init_openwsn(void)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        uart_init(OPENWSN_UART_DEV, OPENWSN_UART_BAUDRATE, \
                  (uart_rx_cb_t)_riot_rx_cb, NULL);
#ifdef MODULE_ZTIMER_USEC
        _ztimer_tx_uart.callback = &_riot_tx_cb;
#else
        (void) _riot_tx_cb;
#endif
    }
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        _uart_vars.txCb = txCb;
        _uart_vars.rxCb = rxCb;
    }
    else {
        (void)rxCb;
        (void)txCb;
    }
}

void uart_setCTS(bool state)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        uint8_t byte;
        if (state == true) {
            byte = XON;
        }
        else {
            byte = XOFF;
        }
        _openwsn_uart_write(&byte);
    }
    else {
        (void)state;
    }
}

void uart_writeByte(uint8_t byteToWrite)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        if (byteToWrite == XON || byteToWrite == XOFF || \
            byteToWrite == XONXOFF_ESCAPE) {
            uint8_t byte = XONXOFF_ESCAPE;
            _uart_vars.fXonXoffEscaping = 0x01;
            _uart_vars.xonXoffEscapedByte = byteToWrite;
            _openwsn_uart_write(&byte);
        }
        else {
            _openwsn_uart_write(&byteToWrite);
        }
    }
    else {
        (void)byteToWrite;
    }
}

inline uint8_t uart_readByte(void)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        return _uart_rx_byte;
    }
    else {
        return 0x00;
    }
}
