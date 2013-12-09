/******************************************************************************
Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
    http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
    scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/*
 * debug_uart.c: provides initial serial debug output
 *
 * Copyright (C) 2008, 2009  Kaspar Schleiser <kaspar@schleiser.de>
 *                           Heiko Will <hwill@inf.fu-berlin.de>
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lpc23xx.h"
#include "VIC.h"
#include <kernel.h>

#include <board_uart0.h>

/**
 * @file
 * @ingroup     lpc2387
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @version     $Revision$
 *
 * @note    $Id$
 */

typedef struct toprint_t {
    unsigned int len;
    char content[];
} toprint_t;

#define QUEUESIZE 255
static volatile toprint_t *queue[QUEUESIZE];
static volatile unsigned char queue_head = 0;
static volatile unsigned char queue_tail = 0;
static volatile unsigned char queue_items = 0;

static volatile unsigned int actual_pos = 0;
static volatile unsigned int running = 0;
static volatile unsigned int fifo = 0;

static volatile toprint_t *actual = NULL;

static inline void enqueue(void)
{
    queue_items++;
    queue_tail++;
}

static inline void dequeue(void)
{
    actual = (queue[queue_head]);
    queue_items--;
    queue_head++;
}

static void  push_queue(void)
{
    running = 1;
    lpm_prevent_sleep |= LPM_PREVENT_SLEEP_UART;
start:

    if (!actual) {
        if (queue_items) {
            dequeue();
        }
        else {
            running = 0;
            lpm_prevent_sleep &= ~LPM_PREVENT_SLEEP_UART;

            if (!fifo)
                while (!(U0LSR & BIT6)) {};

            return;
        }
    }

    while ((actual_pos < actual->len)  && (fifo++ < 16)) {
        U0THR = actual->content[actual_pos++];
    }

    if (actual_pos == actual->len) {
        free((void *)actual);
        actual = NULL;
        actual_pos = 0;
        goto start;
    }
}

int uart_active(void)
{
    return (running || fifo);
}

void stdio_flush(void)
{
    U0IER &= ~BIT1;                             // disable THRE interrupt

    while (running) {
        while (!(U0LSR & (BIT5 | BIT6))) {};    // transmit fifo

        fifo = 0;

        push_queue();                           // dequeue to fifo
    }

    U0IER |= BIT1;                              // enable THRE interrupt
}

void UART0_IRQHandler(void) __attribute__((interrupt("IRQ")));
void UART0_IRQHandler(void)
{
    int iir;
    iir = U0IIR;

    switch (iir & UIIR_ID_MASK) {
        case UIIR_THRE_INT:               // Transmit Holding Register Empty
            fifo = 0;
            push_queue();
            break;

        case UIIR_CTI_INT:                // Character Timeout Indicator
        case UIIR_RDA_INT:                // Receive Data Available
#ifdef MODULE_UART0
            if (uart0_handler_pid) {
                do {
                    int c = U0RBR;
                    uart0_handle_incoming(c);
                }
                while (U0LSR & ULSR_RDR);

                uart0_notify_thread();
            }

#endif
            break;

        default:
            U0LSR;
            U0RBR;
            break;
    } // switch

    VICVectAddr = 0;                    // Acknowledge Interrupt
}

static inline int uart0_puts(char *astring, int length)
{
    /*    while (queue_items == (QUEUESIZE-1)) {} ;
        U0IER = 0;
        queue[queue_tail] = malloc(length+sizeof(unsigned int));
        queue[queue_tail]->len = length;
        memcpy(&queue[queue_tail]->content,astring,length);
        enqueue();
        if (!running)
            push_queue();
        U0IER |= BIT0 | BIT1;       // enable RX irq
    */
    /* alternative without queue:*/
    int i;

    for (i = 0; i < length; i++) {
        while (!(U0LSR & BIT5));

        U0THR = astring[i];
    }

    /*    */

    return length;
}

int fw_puts(char *astring, int length)
{
    return uart0_puts(astring, length);
}

int
bl_uart_init(void)
{
    PCONP |= PCUART0;                               // power on

    // UART0 clock divider is CCLK/8
    PCLKSEL0 |= BIT6 + BIT7;

    U0LCR = 0x83;                                   // 8 bits, no Parity, 1 Stop bit

    // TODO: UART Baudrate calculation using uart->config->speed
    /*
     * Baudrate calculation
     * BR = PCLK (9 MHz) / (16 x 256 x DLM + DLL) x (1/(DIVADDVAL/MULVAL))
     */
    U0FDR = 0x92;       // DIVADDVAL = 0010 = 2, MULVAL = 1001 = 9
    U0DLM = 0x00;
    U0DLL = 0x04;

    U0LCR = 0x03;       // DLAB = 0
    U0FCR = 0x07;       // Enable and reset TX and RX FIFO

    /* irq */
    install_irq(UART0_INT, UART0_IRQHandler, 6);
    U0IER |= BIT0;       // enable only RX irq
    return 1;
}

