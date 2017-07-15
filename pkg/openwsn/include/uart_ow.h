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
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2018
 *
 * @}
 */

#ifndef __UART_OW_H
#define __UART_OW_H

#include "stdint.h"
#include "board_ow.h"

typedef enum {
    UART_EVENT_THRES,
    UART_EVENT_OVERFLOW,
} uart_event_t;

typedef void (*uart_tx_cbt)(void);
typedef void (*uart_rx_cbt)(void);

void    uart_init_ow(void);
void    uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb);
void    uart_enableInterrupts(void);
void    uart_disableInterrupts(void);
void    uart_clearRxInterrupts(void);
void    uart_clearTxInterrupts(void);
void    uart_writeByte(uint8_t byteToWrite);
uint8_t uart_readByte(void);

#endif
