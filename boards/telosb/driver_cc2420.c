/*
 * driver_cc2420.c - Implementation of the board dependent cc2420 functions.
 * Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "hwtimer.h"
#include "panic.h"

#include "cc2420.h"
#include "cc2420_arch.h"
#include "cc2420_spi.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define CC2420_RESETn_PIN   0x40
#define CC2420_VREGEN_PIN   0x20

#define CC2420_FIFOP_PIN    0x01
#define CC2420_GIO0_PIN     0x08
#define CC2420_GIO1_PIN     0x10
#define CC2420_CCA_PIN      0x40
#define CC2420_SFD_PIN      0x02

#define CC2420_FIFOP        (P1IN & CC2420_FIFOP_PIN)   /* FIFOP <-> packet interrupt (P1.0) */
#define CC2420_GIO0         (P1IN & CC2420_GIO0_PIN)    /* FIFO  <-> GIO0 - RX data available (P1.3) */
#define CC2420_GIO1         (P1IN & CC2420_GIO1_PIN)    /* CCA   <-> GIO1 - clear channel (P1.4) */
#define CC2420_SFD          (P4IN & CC2420_SFD_PIN)     /* SFD   <-> TBL - start frame delimiter (P4.1) */

#define CC2420_CS_LOW       (P4OUT &= ~0x04)            /* P4.2 */
#define CC2420_CS_HIGH      (P4OUT |= 0x04)

volatile int abort_count;
volatile int retry_count = 0;

void cc2420_reset(void)
{
    P4OUT |= CC2420_VREGEN_PIN;
    P4OUT &= ~CC2420_RESETn_PIN;
    hwtimer_wait(500);
    P4OUT |= CC2420_RESETn_PIN;
}

void cc2420_gio0_enable(void)
{
    P1IFG &= ~CC2420_GIO0_PIN;     /* Clear IFG for GIO0 */
    P1IE |= CC2420_GIO0_PIN;       /* Enable interrupt for GIO0 */
}

void cc2420_gio0_disable(void)
{
    P1IE &= ~CC2420_GIO0_PIN;      /* Disable interrupt for GIO0 */
    P1IFG &= ~CC2420_GIO0_PIN;     /* Clear IFG for GIO0 */
}

void cc2420_gio1_enable(void)
{
    P1IFG &= ~CC2420_GIO1_PIN;     /* Clear IFG for GIO1 */
    P1IE |= CC2420_GIO1_PIN;       /* Enable interrupt for GIO1 */
}

void cc2420_gio1_disable(void)
{
    P1IE &= ~CC2420_GIO1_PIN;      /* Disable interrupt for GIO1 */
    P1IFG &= ~CC2420_GIO1_PIN;     /* Clear IFG for GIO1 */
}

void cc2420_before_send(void)
{
    /* Disable SFD interrupt before sending packet */
    /* However this is not used atm */
}

void cc2420_after_send(void)
{
    /* Enable SFD interrupt after sending packet */
    /* However this is not used atm */
}


int cc2420_get_gio0(void)
{
    return CC2420_GIO0;
}

int cc2420_get_gio1(void)
{
    return CC2420_GIO1;
}

uint8_t cc2420_get_fifop(void)
{
    return CC2420_FIFOP;
}

uint8_t cc2420_get_sfd(void)
{
    return CC2420_SFD;
}

#define MAX_RSSI_WAIT 1000

uint8_t cc2420_get_cca(void)
{
    uint8_t status;
    long count = 0;
    do {
        unsigned int sr = disableIRQ();
        cc2420_spi_select();
        status = cc2420_txrx(NOBYTE);
        cc2420_spi_unselect();
        restoreIRQ(sr);
        count++;
        if (count >= MAX_RSSI_WAIT) {
            core_panic(0x2420, "cc2420_get_cca(): RSSI never valid!");
        }
    } while (!(status & CC2420_STATUS_RSSI_VALID));
    return CC2420_GIO1;
}

void cc2420_spi_cs(void)
{
    CC2420_CS_LOW;
}

uint8_t cc2420_txrx(uint8_t data)
{
    /* Ensure TX Buf is empty */
    long c = 0;
    IFG1 &= ~UTXIFG0;
    IFG1 &= ~URXIFG0;
    U0TXBUF = data;
    while(!(IFG1 & UTXIFG0)) {
        if (c++ == 1000000) {
            core_panic(0x2420, "cc2420_txrx() alarm");
        }
    }
    /* Wait for Byte received */
    c = 0;
    while(!(IFG1 & URXIFG0)) {
        if (c++ == 1000000) {
            core_panic(0x2420, "cc2420_txrx() alarm");
        }
    }
    return U0RXBUF;
}

void cc2420_spi_select(void)
{
    CC2420_CS_LOW;
}

void cc2420_spi_unselect(void) {
    CC2420_CS_HIGH;
}

void cc2420_init_interrupts(void)
{
    unsigned int state = disableIRQ();  /* Disable all interrupts */
    P1SEL &= ~CC2420_FIFOP_PIN;         /* must be <> 1 to use interrupts */
    P1SEL &= ~CC2420_GIO0_PIN;         /* must be <> 1 to use interrupts */

    /* FIFO <-> GIO0 interrupt */
    P1IES |= CC2420_GIO0_PIN;      /* Enables external interrupt on falling edge (for GIO0/FIFO) */
    P1IE |= CC2420_GIO0_PIN;       /* Enable interrupt */
    P1IFG &= ~CC2420_GIO0_PIN;     /* Clears the interrupt flag */

    /* FIFOP <-> Packet interrupt */
    P1IE |= CC2420_FIFOP_PIN;      /* Enable interrupt for FIFOP */
    P1IFG &= ~CC2420_FIFOP_PIN;     /* Clear IFG for FIFOP */
    restoreIRQ(state);              /* Enable all interrupts */
}

void cc2420_spi_init(void)
{
    /* Switch off async UART */
    while(!(U0TCTL & TXEPT));   /* Wait for empty UxTXBUF register */
    IE1 &= ~(URXIE0 + UTXIE0);  /* Disable USART0 receive&transmit interrupt */
    ME1 &= ~(UTXE0 + URXE0);

    /* configure SPI-related pins */
    P3SEL     |=  0x0E;                           /* P3.1 -  SIMO mode, P3.2 - SOMI mode, P3.3 - SCL mode */
    P3DIR     |=  0x0A;                           /* P3.1 and P3.3 as output */
    P3DIR     &= ~(0x04);                         /* P3.2 as input for SOMI */
    P4OUT     |=  0x04;                           /* P4.2 radio CS, hold high */
    P4DIR     |=  0x04;                           /* P4.2 radio CS, output */

    /* Keep peripheral in reset state */
    U0CTL  = SWRST;

    /* 8-bit SPI Master 3-pin mode, with SMCLK as clock source */
    /* CKPL works also, but not CKPH+CKPL or none of them!! */
    U0CTL |= CHAR + SYNC + MM;
    U0TCTL = CKPH + SSEL1 + SSEL0 + STC + TXEPT;;

    /* Ignore clockrate argument for now, just use clock source/2 */
    /* SMCLK = 8 MHz */
    U0BR0 = 0x02;  /* Ensure baud rate >= 2 */
    U0BR1 = 0x00;
    U0MCTL = 0x00; /* No modulation */
    U0RCTL = 0x00; /* Reset Receive Control Register */

    /* Enable SPI mode */
    ME1 |= USPIE0;

    /* Release for operation */
    U0CTL  &= ~SWRST;
}

/*
 * CC2420 receive interrupt
 */
interrupt (PORT1_VECTOR) __attribute__ ((naked)) cc2420_isr(void)
{
    __enter_isr();
     /* Check IFG */
    if ((P1IFG & CC2420_FIFOP_PIN) != 0) {
        P1IFG &= ~CC2420_FIFOP_PIN;
        cc2420_rx_irq();
        DEBUG("rx interrupt");
    }
    /* GIO0 is falling => check if FIFOP is high, indicating an RXFIFO overflow */
    else if ((P1IFG & CC2420_GIO0_PIN) != 0) {
        P1IFG &= ~CC2420_GIO0_PIN;
        if (cc2420_get_fifop()) {
            cc2420_rxoverflow_irq();
            DEBUG("[CC2420] rxfifo overflow");
        }
    }
    else {
        puts("cc2420_isr(): unexpected IFG!");
        /* Should not occur - only GDO1 and GIO1 interrupts are enabled */
    }
    __exit_isr();
}
