/*
 * redbee_uart1.c - UART1 driver for redbee
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *               2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "mc1322x.h"
#include "uart.h"

volatile uint8_t u1_tx_buf[UART1_TX_BUFFERSIZE];
volatile uint32_t u1_tx_head, u1_tx_tail;

#if UART1_RX_BUFFERSIZE > 32
volatile uint8_t u1_rx_buf[UART1_RX_BUFFERSIZE];
volatile uint32_t u1_rx_head, u1_rx_tail;
#endif

void uart1_isr ( void ) {
#if UART1_RX_BUFFERSIZE > 32
    /* receive interrupt */
    if ( UART1->USTATbits.RXRDY == 1) {
        /* copy fifo into SW buffer */
        while (UART1->RXCON != 0) {
            uint32_t u1_rx_tail_next;
            u1_rx_tail_next = u1_rx_tail + 1;
            if ( u1_rx_tail_next >= sizeof(u1_rx_buf) ) {
                u1_rx_tail_next = 0;
            }
            if ( u1_rx_head != u1_rx_tail_next ) {
                u1_rx_buf[u1_rx_tail] = UART1->DATA;
                u1_rx_tail = u1_rx_tail_next;
            }
            /* buffer full, flush fifo */
            else {
                while ( UART1->RXCON != 0 ) {
                    UART1->DATA;
                }
            }
        }
        return;
    }
#endif
    while ( UART1->TXCON != 0 ) {
        if ( u1_tx_head == u1_tx_tail ) {
#if UART1_RX_BUFFERSIZE > 32
            UART1->CONbits.MTXR = 1;
#else
            disable_irq(UART1);
#endif
            return;
        }
        
        UART1->DATA = u1_tx_buf[u1_tx_tail++];
        if ( u1_tx_tail >= sizeof(u1_tx_buf) ) {
            u1_tx_tail = 0;
        }
    }
}

void uart1_putc ( uint8_t c ) {
    /* disable UART1_IRQ since it modifies u1_tx_(head|tail) */
#if UART1_RX_BUFFERSIZE > 32
    UART1->CONbits.MTXR = 1;
#else
    disable_irq(UART1);
#endif
    if ( (u1_tx_head == u1_tx_tail) && (UART1->TXCON != 0) ) {
        UART1->DATA = c;
    }
    else {
        u1_tx_buf[u1_tx_head++] = c;
        if ( u1_tx_head >= sizeof(u1_tx_buf) ) {
            u1_tx_head = 0;
        }
        if ( u1_tx_head == u1_tx_tail ) {
            uint32_t u1_tx_tail_save = u1_tx_tail;
            /* back up head to show buffer not empty and enable tx interrupt */
            u1_tx_head--;
#if UART1_RX_BUFFERSIZE > 32
            UART1->CONbits.MTXR = 0;
#else
            enable_irq(UART1);
#endif
            /* tail will change after one char goes out */
            while ( u1_tx_tail_save == u1_tx_tail ) {
                /* wait */
            }
            /* restor head */
            u1_tx_head++;
            return;
        }
    }
#if UART1_RX_BUFFERSIZE > 32
    UART1->CONbits.MTXR = 0;
#else
    enable_irq(UART1);
#endif
}

uint8_t uart1_getc ( void ) {
#if UART1_RX_BUFFERSIZE > 32
    /* pull from ram buffer */
    uint8_t c = 0;
    if ( u1_rx_head != u1_rx_tail ) {
        c = u1_rx_buf[u1_rx_head++];
        if ( u1_rx_head >= sizeof(u1_rx_buf) ) {
            u1_rx_head = 0;
        }
        return c;
    }
#endif
    /* pull from hw fifo */
    while ( uart1_can_get() == 0 ) {
        /* wait */
    }
    return UART1->DATA;
}

// static volatile unsigned int running = 0;
// static volatile unsigned int fifo = 0;
// 
// static inline int uart0_puts(char *astring, int length)
// {
//     int i;
//     for (i=0;i<length;i++) {
//         /* TODO: Fix me */
//         while(UART1->TXCON != 0);
//         UART1->DATA = astring[i];
//     }
//     return length;
// }
// 
// int fw_puts(char *astring, int length)
// {
//     return uart0_puts(astring, length);
// }
// 
// void stdio_flush(void)
// {
//     /* disable TX interrupt */
//     UART1->CONbits.MTXR = 1;
//     while(running) {
//         while(UART1->TXCON != 0);
//         fifo=0;
//         push_queue();                           // dequeue to fifo
//     }
//     /* enable TX interrupt */
//     UART1->CONbits.MTXR = 0;
// }
