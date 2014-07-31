/*
 * driver_cc2420.c - Implementation of the board dependent cc2420 functions.
 * Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
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
#include "crash.h"

#include "cc2420.h"
#include "cc2420_arch.h"
#include "cc2420_spi.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define CC2420_RESETn_PIN   0x80
#define CC2420_VREGEN_PIN   0x01

#define CC2420_GDO0_PIN     0x08
#define CC2420_GDO2_PIN     0x10
#define CC2420_SFD_PIN      0x20
#define CC2420_CCA_PIN      0x40

#define CC2420_GDO0         (P1IN & CC2420_GDO0_PIN)   // read serial I/O (GDO0)
#define CC2420_GDO2         (P1IN & CC2420_GDO2_PIN)   // read serial I/O (GDO2)
#define CC2420_SFD          (P1IN & CC2420_SFD_PIN)    // read serial I/0 (SFD)
#define CC2420_CCA          (P1IN & CC2420_CCA_PIN)    // read serial I/O (CCA)


#define CC2420_CS_LOW       (P4OUT &= ~0x04)
#define CC2420_CS_HIGH      (P4OUT |= 0x04)

#define CC2420_GDO1_LOW_COUNT            (2700)          // loop count (timeout ~ 500 us) to wait
#define CC2420_GDO1_LOW_RETRY             (100)          // max. retries for GDO1 to go low

volatile int abort_count;
volatile int retry_count = 0;

void cc2420_reset(void)
{
    P3OUT |= CC2420_VREGEN_PIN;
    P1OUT &= ~CC2420_RESETn_PIN;
    hwtimer_wait(500);
    P1OUT |= CC2420_RESETn_PIN;
}

void cc2420_gdo0_enable(void)
{
    P1IFG &= ~CC2420_GDO0_PIN;     /* Clear IFG for GDO0 */
    P1IE |= CC2420_GDO0_PIN;       /* Enable interrupt for GDO0 */
}

void cc2420_gdo0_disable(void)
{
    P1IE &= ~CC2420_GDO0_PIN;      /* Disable interrupt for GDO0 */
    P1IFG &= ~CC2420_GDO0_PIN;     /* Clear IFG for GDO0 */
}

void cc2420_gdo2_enable(void)
{
    P1IFG &= ~CC2420_GDO2_PIN;     /* Clear IFG for GDO2 */
    P1IE |= CC2420_GDO2_PIN;       /* Enable interrupt for GDO2 */
}

void cc2420_gdo2_disable(void)
{
    P1IE &= ~CC2420_GDO2_PIN;      /* Disable interrupt for GDO2 */
    P1IFG &= ~CC2420_GDO2_PIN;     /* Clear IFG for GDO2 */
}

void cc2420_before_send(void)
{
    // Disable SFD interrupt before sending packet
    // However this is not used atm
}

void cc2420_after_send(void)
{
    // Enable SFD interrupt after sending packet
    // However this is not used atm
}


int cc2420_get_gdo0(void)
{
    return CC2420_GDO0;
}

int cc2420_get_gdo2(void)
{
    return CC2420_GDO2;
}

uint8_t cc2420_get_sfd(void)
{
    return CC2420_SFD;
}

uint8_t cc2420_get_fifop(void)
{
    return CC2420_GDO2;
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
    return CC2420_CCA;
}

void cc2420_spi_cs(void)
{
    CC2420_CS_LOW;
}

uint8_t cc2420_txrx(uint8_t data)
{
    /* Ensure TX Buf is empty */
    long c = 0;
    IFG2 &= ~UTXIFG1;
    IFG2 &= ~URXIFG1;
    U1TXBUF = data;
    while(!(IFG2 & UTXIFG1)) {
        if (c++ == 1000000) {
            core_panic(0x2420, "cc2420_txrx() alarm");
        }
    }
    /* Wait for Byte received */
    c = 0;
    while(!(IFG2 & URXIFG1)) {
        if (c++ == 1000000) {
            core_panic(0x2420, "cc2420_txrx() alarm");
        }
    }
    return U1RXBUF;
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
    unsigned int state = disableIRQ(); /* Disable all interrupts */
    P1SEL = 0x00;       /* must be <> 1 to use interrupts */
    P1IES |= CC2420_GDO2_PIN;      /* Enables external interrupt on falling edge (for GDO2) */
    P1IE |= CC2420_GDO2_PIN;       /* Enable interrupt */
    P1IFG &= ~CC2420_GDO2_PIN;     /* Clears the interrupt flag */
    P1IES |= CC2420_SFD_PIN;      /* Enables external interrupt on falling edge (for GDO2) */
    P1IE |= CC2420_SFD_PIN;       /* Enable interrupt */
    P1IFG &= ~CC2420_SFD_PIN;     /* Clears the interrupt flag */
    P1IE &= ~CC2420_GDO0_PIN;      /* Disable interrupt for GDO0 */
    P1IFG &= ~CC2420_GDO0_PIN;     /* Clear IFG for GDO0 */
    restoreIRQ(state);  /* Enable all interrupts */
}

void cc2420_spi_init(void)
{
    // Switch off async UART
    while(!(U1TCTL & TXEPT));   // Wait for empty UxTXBUF register
    IE2 &= ~(URXIE1 + UTXIE1);  // Disable USART1 receive&transmit interrupt
    ME2 &= ~(UTXE1 + URXE1);
    P5DIR |= 0x0A;              // output for CLK and SIMO
    P5DIR &= ~(0x04);           // input for SOMI
    P5SEL |= 0x0E;              // Set pins as SPI

    // Keep peripheral in reset state
    U1CTL  = SWRST;

    // 8-bit SPI Master 3-pin mode, with SMCLK as clock source
    // CKPL works also, but not CKPH+CKPL or none of them!!
    U1CTL |= CHAR + SYNC + MM;
    U1TCTL = CKPH + SSEL1 + SSEL0 + STC;

    // Ignore clockrate argument for now, just use clock source/2
    // SMCLK = 8 MHz
    U1BR0 = 0x02;  // Ensure baud rate >= 2
    U1BR1 = 0x00;
    U1MCTL = 0x00; // No modulation
    U1RCTL = 0x00; // Reset Receive Control Register

    // Enable SPI mode
    ME2 |= USPIE1;

    // Release for operation
    U1CTL  &= ~SWRST;
}

/*
 * CC2420 receive interrupt
 */
interrupt (PORT1_VECTOR) __attribute__ ((naked)) cc2420_isr(void){
    __enter_isr();
     /* Check IFG */
    if ((P1IFG & CC2420_GDO2_PIN) != 0) {
        DEBUG("rx interrupt");
        P1IFG &= ~CC2420_GDO2_PIN;
        cc2420_rx_irq();
    }
    else if ((P1IFG & CC2420_GDO0_PIN) != 0) {
        if (cc2420_get_gdo2()) {
            cc2420_rxoverflow_irq();
            DEBUG("[CC2420] rxfifo overflow");
        }
        P1IFG &= ~CC2420_GDO0_PIN;                /* Clear IFG for GDO0 */
    }
    else if ((P1IFG & CC2420_SFD_PIN) != 0) {
        DEBUG("sfd interrupt");
        P1IFG &= ~CC2420_SFD_PIN;
        cc2420_switch_to_rx();
    }

    else {
        puts("cc2420_isr(): unexpected IFG!");
        /* Should not occur - only GDO1 and GDO2 interrupts are enabled */
    }
    __exit_isr();
}
