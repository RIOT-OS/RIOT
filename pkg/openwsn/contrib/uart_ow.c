/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
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
 * @brief       RIOT adaption-specific definition of the "uart" bsp module
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "uart_ow.h"
#include "board_ow.h"
#include "periph/uart.h"
#include "periph/timer.h"

/**
 * @brief   We use a periph timer to generate an interrupt after one byte has
 *          been transmitted, as OpenWSN's openserial tool requires this. Be aware
 *          that some platforms might use Timer_DEV(1) for xtimer
 */
#ifndef RIOT_PERIPH_TIMER
#define RIOT_PERIPH_TIMER TIMER_DEV(1)
#endif

typedef struct {
    uart_tx_cbt txCb;
    uart_rx_cbt rxCb;
} uart_vars_t;

uart_vars_t uart_vars;


volatile uint8_t uart_rx_byte;

static void _riot_rx_cb(void *arg, uint8_t data)
{
    (void)arg;
    uart_rx_byte = data;
    uart_vars.rxCb();
}

static void _riot_tx_cb(void *arg, int count)
{
    (void)arg;
    (void)count;
    uart_vars.txCb();
}

void uart_init_ow(void)
{

    uart_init(UART_DEV(1), 115200, (uart_rx_cb_t) _riot_rx_cb, NULL);
    timer_init(RIOT_PERIPH_TIMER, 1000000, _riot_tx_cb, NULL); /* TODO make timer speed generic*/
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb)
{
    uart_vars.txCb = txCb;
    uart_vars.rxCb = rxCb;
}

void uart_enableInterrupts(void)
{

}

void uart_disableInterrupts(void)
{

}

void uart_clearRxInterrupts(void)
{

}

void uart_clearTxInterrupts(void)
{

}

void uart_writeByte(uint8_t byteToWrite)
{
    uart_write(UART_DEV(1), &byteToWrite, 1);
    timer_set(RIOT_PERIPH_TIMER, 0, 10); /* TODO check offset */
    timer_start(RIOT_PERIPH_TIMER);
}

uint8_t uart_readByte(void)
{
    return uart_rx_byte;
}
