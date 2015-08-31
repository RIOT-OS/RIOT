/*
 * uart.c - Implementation for the Wismote UART
 * Copyright (C) 2015 Sumankumar Panchal <suman@ece.iisc.ernet.in>
 *
 * Author : Sumankumar Panchal <suman@ece.iisc.ernet.in>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_wismote
 * @{
 *
 * @file
 * @brief       Board specific UART/USB driver HAL for the Wismote
 *
 * @author      Sumankumar Panchal<suman@ece.iisc.ernet.in>
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


/*---------------------------------------------------------------------------*/
/**
 *  * Initalize the RS232 port.
 *   *
 *    */
void
uart_init(void)
{
  /* RS232 */
  UCA1CTL1 |= UCSWRST;            /* Hold peripheral in reset state */
  UCA1CTL1 |= UCSSEL_2;           /* CLK = SMCLK */

/*  ubr = (MSP430_CPU_SPEED / ubr);*/
  unsigned long ubr;

  ubr = (16000000uL / 115200uL);
  UCA1BR0 = ubr & 0xff;
  UCA1BR1 = (ubr >> 8) & 0xff;
  /* UCA1MCTL |= UCBRS_2 + UCBRF_0;            // Modulation UCBRFx=0 */
  UCA1MCTL = UCBRS_3;             /* Modulation UCBRSx = 3 */

  P4DIR |= BIT5;
  P4OUT |= BIT5 ;
  P5SEL |= BIT6|BIT7;  // P5.6,7 = USCI_A1 TXD/RXD

  P4SEL |= BIT7;
  P4DIR |= BIT7;
/*UCA1CTL1 &= ~UCSWRST;*/       /* Initialize USCI state machine */

//  transmitting = 0;

  /* XXX Clear pending interrupts before enable */
  UCA1IE &= ~UCRXIFG;
  UCA1IE &= ~UCTXIFG;

  UCA1CTL1 &= ~UCSWRST;                   /* Initialize USCI state machine **before** enabling interrupts */
  UCA1IE |= UCRXIE;                        /* Enable UCA1 RX interrupt */
}
/*------------------------------------------------------------------------------*/

int putchar(int c)
{
    unsigned sr = disableIRQ();

    /* the LF endline character needs to be "doubled" into CR+LF */
    if (c == '\n') {
        putchar('\r');
    }
    /* wait for a previous transmission to end */
    /*while ((IFG2 & UCA0TXIFG) == 0) {
        __asm__("nop");
    }*/
    while((UCA1STAT & UCBUSY));

    /* load TX byte buffer */
    UCA1TXBUF = (uint8_t) c;

    restoreIRQ(sr);
    return c;
}

uint8_t uart_readByte(void)
{
    return UCA1RXBUF;
}

/**
 * \brief the interrupt handler for UART reception
 */
#if 0
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
    } else if (uart0_handler_pid != KERNEL_PID_UNDEF) {
        /* All went well -> let's signal the reception to adequate callbacks */
        c = UCA0RXBUF;
        uart0_handle_incoming(c);
        uart0_notify_thread();
#endif
    }

    __exit_isr();
}
#endif
