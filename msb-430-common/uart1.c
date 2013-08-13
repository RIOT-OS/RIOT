#include "board.h"

#define UART1_TX                        TXBUF1
#define UART1_WAIT_TXDONE()       while( (UTCTL1 & TXEPT) == 0 ) { _NOP(); }

#include <stdio.h>
#include <kernel.h>

#include <board_uart0.h>

int putchar(int c)
{
    UART1_TX = c;
    UART1_WAIT_TXDONE();

    if (c == 10) {
        UART1_WAIT_TXDONE();
    }

    return c;
}

void usart0irq(void);
/**
 * \brief the interrupt function
 */
interrupt(USART1RX_VECTOR) usart0irq(void) 
{
    U1TCTL &= ~URXSE; /* Clear the URXS signal */
    U1TCTL |= URXSE;  /* Re-enable URXS - needed here?*/
    int c = 0;
    /* Check status register for receive errors. */
    if(U1RCTL & RXERR) {
        if (U1RCTL & FE) {
           puts("rx framing error");
        }
        if (U1RCTL & OE) {
            puts("rx overrun error");
        }
        if (U1RCTL & PE) {
            puts("rx parity error");
        }
        if (U1RCTL & BRK) {
            puts("rx break error");
        }
        /* Clear error flags by forcing a dummy read. */
        c = U1RXBUF;
    }
#ifdef MODULE_UART0
    else if (uart0_handler_pid) {
        c = U1RXBUF;
        uart0_handle_incoming(c);
        uart0_notify_thread();
    }
#endif
}

