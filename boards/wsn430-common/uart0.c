  /*
   * uart0.c - Implementation of the uart.
   * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
   *
   * This source code is licensed under the GNU General Public License,
   * Version 3.  See the file LICENSE for more details.
   */

#include "board.h"

#define   UART0_TX                        U0TXBUF
#define   UART0_WAIT_TXDONE()       while( (U0TCTL & TXEPT) == 0 ) { _NOP(); }

#include <stdio.h>
#include <kernel.h>

#include <board_uart0.h>

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
    int dummy = 0;
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
        dummy = U0RXBUF;
    }
#ifdef MODULE_UART0
    else if (uart0_handler_pid) {
                dummy = U0RXBUF;
                uart0_handle_incoming(dummy);
                uart0_notify_thread();
            }
#endif
}

