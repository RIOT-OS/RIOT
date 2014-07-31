/*
 * uart.c - Implementation for the Zolertia Z1 UART
 * Copyright (C) 2014 INRIA
 *
 * Author : Kevin Roussel <kevin.roussel@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_z1
 * @{
 *
 * @file        uart.c
 * @brief       Board specific UART/USB driver HAL for the Zolertia Z1
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "board.h"
#include "kernel.h"
#include "irq.h"
#include "board_uart0.h"


#define BAUDRATE    (115200ul)

#define BAUD_RATE_MAJOR   (int)(MSP430_INITIAL_CPU_SPEED / BAUDRATE)
#define BAUD_RATE_MINOR   (int)(((MSP430_INITIAL_CPU_SPEED / BAUDRATE) - BAUD_RATE_MAJOR) * 8)


void uart_init(void)
{
    /*
     * NOTE : MCU pin (GPIO port) initialisation is done
     * in board.c, function z1_ports_init().
     */
    UCA0CTL1  = UCSWRST;         /* hold UART module in reset state
                                    while we configure it */

    UCA0CTL1 |= UCSSEL_2;          /* source UART's BRCLK from 8 MHz SMCLK  */
    UCA0MCTL  = UCBRS1 + UCBRS0;   /* low-frequency baud rate generation,
                                      modulation type 4 */

    /* 115200 baud, divided from 8 MHz == 69 */
    UCA0BR0   = BAUD_RATE_MAJOR;
    UCA0BR1   = BAUD_RATE_MINOR;

    /* remaining registers : set to default */
    UCA0CTL0  = 0x00;   /* put in asynchronous (== UART) mode, LSB first */
    UCA0STAT  = 0x00;   /* reset status flags */

    /* clear UART-related interrupt flags */
    IFG2 &= ~(UCA0RXIFG | UCA0TXIFG);

    /* configuration done, release reset bit => start UART */
    UCA0CTL1 &= ~UCSWRST;

    /* enable UART0 RX interrupt, disable UART0 TX interrupt */
    IE2 |= UCA0RXIE;
    IE2 &= ~UCA0TXIE;
}

int putchar(int c)
{
    unsigned sr = disableIRQ();

    /* the LF endline character needs to be "doubled" into CR+LF */
    if (c == '\n') {
        putchar('\r');
    }
    /* wait for a previous transmission to end */
    while ((IFG2 & UCA0TXIFG) == 0) {
        __asm__("nop");
    }
    /* load TX byte buffer */
    UCA0TXBUF = (uint8_t) c;

    restoreIRQ(sr);
    return c;
}

uint8_t uart_readByte(void)
{
    return UCA0RXBUF;
}

/**
 * \brief the interrupt handler for UART reception
 */
interrupt(USCIAB0RX_VECTOR) __attribute__ ((naked)) usart1irq(void)
{
    __enter_isr();

#ifndef MODULE_UART0
    int __attribute__ ((unused)) c;
#else
    int c;
#endif

    /* Check status register for receive errors. */
    if (UCA0STAT & UCRXERR) {
        if (UCA0STAT & UCFE) {
            puts("UART RX framing error");
        }
        if (UCA0STAT & UCOE) {
            puts("UART RX overrun error");
        }
        if (UCA0STAT & UCPE) {
            puts("UART RX parity error");
        }
        if (UCA0STAT & UCBRK) {
            puts("UART RX break condition -> error");
        }
        /* Clear error flags by forcing a dummy read. */
        c = UCA0RXBUF;
#ifdef MODULE_UART0
    } else if (uart0_handler_pid) {
    	/* All went well -> let's signal the reception to adequate callbacks */
    	c = UCA0RXBUF;
    	uart0_handle_incoming(c);
    	uart0_notify_thread();
#endif
    }

    __exit_isr();
}
