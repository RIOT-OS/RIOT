/*
 * uart1.c - UART1 driver for redbee
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *               2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "mc1322x.h"
#include "board_uart0.h"
#include "uart.h"

void uart2_isr(void)
{
    int i = 0;

    if (UART2->USTATbits.RXRDY == 1) {
#ifdef MODULE_UART0

        if (uart0_handler_pid) {
            while (UART2->RXCON != 0) {
                uart0_handle_incoming(UART2->DATA);

                if (++i >= UART0_BUFSIZE) {
                    uart0_notify_thread();
                    i = 0;
                }
            }

            uart0_notify_thread();
        }

#endif
    }
}

void uart2_putc(uint8_t c)
{
    /* while uart fifo is full */
    while (UART2->TXCON == 0) {
        /* wait */
    }

    UART2->DATA = c;
}

uint8_t uart2_getc(void)
{
    /* while uart fifo is empty */
    while (UART2->RXCON == 0) {
        /* wait */
    }

    return UART2->DATA;
}