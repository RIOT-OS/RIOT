/*
 * uart0.c - Implementation of the uart.
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include "board.h"

#define   UART0_TX                        U0TXBUF
#define   UART0_WAIT_TXDONE()       while( (U0TCTL & TXEPT) == 0 ) { _NOP(); }

#include "kernel.h"

#include "board_uart0.h"

int putchar(int c)
{
    UART0_TX = c;
    UART0_WAIT_TXDONE();

    if (c == 10) {
        UART0_TX = 13;
        UART0_WAIT_TXDONE();
    }

    return c;
}

void usart0irq(void);
/**
 * \brief the interrupt function
 */
interrupt(USART0RX_VECTOR) usart0irq(void) {
    /* Check status register for receive errors. */
    if(U0RCTL & RXERR) {
        if (U0RCTL & FE) {
           puts("rx framing error");
        }
        if (U0RCTL & OE) {
            puts("rx overrun error");
        }
        if (U0RCTL & PE) {
            puts("rx parity error");
        }
        if (U0RCTL & BRK) {
            puts("rx break error");
        }
        /* Clear error flags by forcing a dummy read. */
        volatile int dummy = U0RXBUF;
        (void) dummy;
    }
#ifdef MODULE_UART0
    else if (uart0_handler_pid != KERNEL_PID_UNDEF) {
        int c = U0RXBUF;
        uart0_handle_incoming(&c, 1);
        thread_yield();
    }
#endif
}
