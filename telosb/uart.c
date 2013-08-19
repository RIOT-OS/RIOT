  /*
   * uart.c - Implementation for the TelosB UART
   * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
   *
   * This file subject to the terms and conditions of the GLGPLv2 License. See the file LICENSE in the  
   * top level directory for more details.
   */

#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "board.h"
#include "kernel.h"

#define   UART1_TX                  U1TXBUF
#define   UART1_WAIT_TXDONE()       while ( (U1TCTL & TXEPT) == 0 ) { _NOP(); }

#define BAUDRATE    (115200ul)

void uart_init(void)
{
    UCTL1     =  SWRST;           /* hold UART1 module in reset */
    UCTL1    |=  CHAR;            /* 8-bit character */

    /* 115200 baud, clocked from 4.8MHz SMCLK */
    UTCTL1   |= SSEL1;        /* UCLK = SCLK */
    UBR01     = F_CPU / BAUDRATE;
    UBR11     = (F_CPU / BAUDRATE) >> 8;
    UMCTL1    =  0x4A;            /* modulation */

    ME2      |=  UTXE1 + URXE1;   /* enable UART1 TX/RX */
    UCTL1    &= ~SWRST;           /* clear UART1 reset bit */
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
