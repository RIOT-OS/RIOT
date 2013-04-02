/*
 * redbee_uart1.c - UART1 driver for redbee
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "mc1322x.h"

static volatile unsigned int running = 0;
static volatile unsigned int fifo = 0;

static inline int uart0_puts(char *astring, int length)
{
    int i;
    for (i=0;i<length;i++) {
        /* TODO: Fix me */
        while(UART1->TXCON != 0);
        UART1->DATA = astring[i];
    }
    return length;
}

int fw_puts(char *astring, int length)
{
    return uart0_puts(astring, length);
}

void stdio_flush(void)
{
    /* disable TX interrupt */
    UART1->CONbits.MTXR = 1;
    while(running) {
        while(UART1->TXCON != 0);
        fifo=0;
        push_queue();                           // dequeue to fifo
    }
    /* enable TX interrupt */
    UART1->CONbits.MTXR = 0;
}
