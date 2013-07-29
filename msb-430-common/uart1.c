#include "board.h"

#define   UART1_TX                        TXBUF1
#define UART1_WAIT_TXDONE()       while( (UTCTL1 & TXEPT) == 0 ) { _NOP(); }

#include <stdio.h>

int putchar(int c)
{
    UART1_TX = c;
    UART1_WAIT_TXDONE();

    if (c == 10) {
        UART1_TX = 13;
        UART1_WAIT_TXDONE();
    }

    return c;
}


