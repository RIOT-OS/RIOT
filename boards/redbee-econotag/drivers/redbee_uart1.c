/*
 * uart1.c - UART1 driver for redbee
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *               2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "mc1322x.h"
#include "board_uart0.h"
#include "uart.h"

void uart1_isr(void)
{
    if (UART1->USTATbits.RXRDY == 1) {
#ifdef MODULE_UART0
        uint32_t i = 0;
        while (UART1->RXCON != 0) {
            char c = UART1->DATA;
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

void uart1_putc(uint8_t c)
{
    /* while uart fifo is full */
    while (UART1->TXCON == 0) {
        /* wait */
    }

    UART1->DATA = c;
}

uint8_t uart1_getc(void)
{
    /* while uart fifo is empty */
    while (UART1->RXCON == 0) {
        /* wait */
    }

    return UART1->DATA;
}
