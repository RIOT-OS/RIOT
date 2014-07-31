/*
 * uart.c - Implementation for the TelosB UART
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "board.h"
#include "kernel.h"
#include "board_uart0.h"

#define   UART1_TX                  U1TXBUF
#define   UART1_WAIT_TXDONE()       while ( (U1TCTL & TXEPT) == 0 ) { _NOP(); }

#define BAUDRATE    (115200ul)

static uint8_t calc_umctl(uint16_t br)
{
    /* from TI slaa049 */
    register uint8_t CMOD = 256 * br - 256 * (br + 1) / 2;
    register uint8_t c = 0;
    register int i = 0;
    register uint8_t a = CMOD;
    a <<= 1;

    do {
        if (a & 0x80) {            /* Overflow to integer? */
            a = a - 128 + CMOD;    /* Yes, subtract 1.000000 */
            c |= 0x80;
        }
        else {
            a += CMOD;              /* No, add fraction */
        }

        if (i == 7) {
            return c;
        }

        i++;
        c >>= 1;
    }
    while (1);
}

void uart_init(void)
{
    UCTL1     =  SWRST;           /* hold UART1 module in reset */
    UCTL1    |=  CHAR;            /* 8-bit character */

    /* 115200 baud, clocked from 4.8MHz SMCLK */
    UTCTL1   |= SSEL1;        /* UCLK = SCLK */
    UBR01     = F_CPU / BAUDRATE;
    UBR11     = (F_CPU / BAUDRATE) >> 8;
    UMCTL1    = calc_umctl(F_CPU / BAUDRATE);       /* set modulation */

    ME2      |=  UTXE1 + URXE1;   /* enable UART1 TX/RX */
    UCTL1    &= ~SWRST;           /* clear UART1 reset bit */

    IE2  |= URXIE1;               /* enable rx interrupt */
    IFG1 &= ~UTXIFG1;
}

int putchar(int c)
{
    UART1_TX = c;
    UART1_WAIT_TXDONE();
    return c;
}

uint8_t uart_readByte(void)
{
    return U1RXBUF;
}

void usart1irq(void);
/**
 * \brief the interrupt function
 */
interrupt(USART1RX_VECTOR) usart1irq(void)
{
    int c = 0;

    /* Check status register for receive errors. */
    if (U1RCTL & RXERR) {
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
