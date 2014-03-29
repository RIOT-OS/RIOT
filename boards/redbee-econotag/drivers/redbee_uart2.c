/*
 * uart1.c - UART1 driver for redbee
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *               2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "mc1322x.h"
#include "board_uart0.h"
#include "uart.h"
#include "thread.h"

void uart2_isr(void)
{
    uint32_t i = 0;

    if (UART2->USTATbits.RXRDY == 1) {
#ifdef MODULE_UART0
        while (UART2->RXCON != 0) {
            char c = UART2->DATA;
            uart0_handle_incoming(&c, 1);

            if (++i >= UART0_BUFSIZE) {
                thread_yield();
                i = 0;
            }
        }
        if (i > 0) {
            thread_yield();
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
