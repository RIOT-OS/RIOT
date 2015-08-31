/*
 * board.c - Board initialization for the Wismote
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
 * @brief       Board specific implementations for the Wismote
 *
 * @author      Sumankumar Panchal <suman@ece.iisc.ernet.in>
 *
 * @}
 */


#include "cpu.h"
#include "board.h"

void uart_init(void);


static void wismote_ports_init(void)
{
    P1SEL = 0x00;
    P1OUT = 0x00;
    P1DIR = 0x00;

    P2SEL = 0x00;
    P2OUT = 0x00;
    P2DIR = 0x00;

    P3SEL = 0x00;
    P3OUT = 0x00;
    P3DIR = 0x00;

    P4SEL = 0x00;
    P4OUT = 0x00;
    P4DIR = 0x00;

    P5SEL = 0x00;
    P5OUT = 0x00;
    P5DIR = 0x00;

    P6SEL = 0x00;
    P6OUT = 0x00;
    P6DIR = 0x00;

}

/*---------------------------------------------------------------------------*/
void msp430_init_dco(void)
{
#ifdef __IAR_SYSTEMS_ICC__
  __bis_SR_register(SCG0);
#else
  //asmv("bis %0, r2" : : "i" (SCG0));
#endif

  UCSCTL0 = 0x0000;
  UCSCTL1 = DCORSEL_4;

  UCSCTL2 = MSP430_INITIAL_CPU_SPEED / 32768;
  UCSCTL4 = 0x33; /* instead of 0x44 that is DCO/2 */

#ifdef __IAR_SYSTEMS_ICC__
  __bic_SR_register(SCG0);
#else
  //asmv("bic %0, r2" : : "i" (SCG0));
#endif
}

/* "public" specific initialization function for the Wismote hardware */

void board_init(void)
{
    /* init CPU core */
    msp430_cpu_init();

    /* disable watchdog timer */
    WDTCTL     =  WDTPW + WDTHOLD;

    /* init MCU pins as adequate for Wismote hardware */
    wismote_ports_init();

    /* initializes DCO */
    msp430_init_dco();

    /* initialize UART/USB module */
    uart_init();

    /* enable interrupts */
    __bis_SR_register(GIE);
}
