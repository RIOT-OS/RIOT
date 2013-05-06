/*
 * redbee_uart2.c - UART2 driver for redbee
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include "mc1322x.h"
#include "uart.h"

volatile uint8_t u2_tx_buf[UART2_TX_BUFFERSIZE];
volatile uint32_t u2_tx_head, u2_tx_tail;

#if UART2_RX_BUFFERSIZE > 32
volatile uint8_t u2_rx_buf[UART2_RX_BUFFERSIZE];
volatile uint32_t u2_rx_head, u2_rx_tail;
#endif

void uart2_isr ( void ) {
#if UART2_RX_BUFFERSIZE > 32
    /* receive interrupt */
    if ( UART2->USTATbits.RXRDY == 1) {
        /* copy fifo into SW buffer */
        while (UART2->RXCON != 0) {
            uint32_t u2_rx_tail_next;
            u2_rx_tail_next = u2_rx_tail + 1;
            if ( u2_rx_tail_next >= sizeof(u2_rx_buf) ) {
                u2_rx_tail_next = 0;
            }
            if ( u2_rx_head != u2_rx_tail_next ) {
                u2_rx_buf[u2_rx_tail] = UART2->DATA;
                u2_rx_tail = u2_rx_tail_next;
            }
            /* buffer full, flush fifo */
            else {
                while ( UART2->RXCON != 0 ) {
                    UART2->DATA;
                }
            }
        }
        return;
    }
#endif
    while ( UART2->TXCON != 0 ) {
        if ( u2_tx_head == u2_tx_tail ) {
#if UART2_RX_BUFFERSIZE > 32
            UART2->CONbits.MTXR = 1;
#else
            disable_irq(UART2);
#endif
            return;
        }
        
        UART2->DATA = u2_tx_buf[u2_tx_tail++];
        if ( u2_tx_tail >= sizeof(u2_tx_buf) ) {
            u2_tx_tail = 0;
        }
    }
}

void uart2_putc ( uint8_t c ) {
    /* disable UART2_IRQ since it modifies u2_tx_(head|tail) */
#if UART2_RX_BUFFERSIZE > 32
    UART2->CONbits.MTXR = 1;
#else
    disable_irq(UART2);
#endif
    if ( (u2_tx_head == u2_tx_tail) && (UART2->TXCON != 0) ) {
        UART2->DATA = c;
    }
    else {
        u2_tx_buf[u2_tx_head++] = c;
        if ( u2_tx_head >= sizeof(u2_tx_buf) ) {
            u2_tx_head = 0;
        }
        if ( u2_tx_head == u2_tx_tail ) {
            uint32_t u2_tx_tail_save = u2_tx_tail;
            /* back up head to show buffer not empty and enable tx interrupt */
            u2_tx_head--;
#if UART2_RX_BUFFERSIZE > 32
            UART2->CONbits.MTXR = 0;
#else
            enable_irq(UART2);
#endif
            /* tail will change after one char goes out */
            while ( u2_tx_tail_save == u2_tx_tail ) {
                /* wait */
            }
            /* restor head */
            u2_tx_head++;
            return;
        }
    }
#if UART2_RX_BUFFERSIZE > 32
    UART2->CONbits.MTXR = 0;
#else
    enable_irq(UART2);
#endif
}

uint8_t uart2_getc ( void ) {
#if UART2_RX_BUFFERSIZE > 32
    /* pull from ram buffer */
    uint8_t c = 0;
    if ( u2_rx_head != u2_rx_tail ) {
        c = u2_rx_buf[u2_rx_head++];
        if ( u2_rx_head >= sizeof(u2_rx_buf) ) {
            u2_rx_head = 0;
        }
        return c;
    }
#endif
    /* pull from hw fifo */
    while ( uart2_can_get() == 0 ) {
        /* wait */
    }
    return UART2->DATA;
}