/*
 * driver_cc110x.c - Implementation of the board dependent cc1100 functions.
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

#include "cc110x_ng.h"

#define CC1100_GDO0         (P1IN & 0x08)   // read serial I/O (GDO0)
#define CC1100_GDO1         (P5IN & 0x04)   // read serial I/O (GDO1)
#define CC1100_GDO2         (P1IN & 0x10)   // read serial I/O (GDO2)

#define CC1100_CS_LOW       (P4OUT &= ~0x04)
#define CC1100_CS_HIGH      (P4OUT |= 0x04)

#define CC1100_GDO1_LOW_COUNT            (2700)          // loop count (timeout ~ 500 us) to wait
#define CC1100_GDO1_LOW_RETRY             (100)          // max. retries for GDO1 to go low

volatile int abort_count;
volatile int retry_count = 0;

void cc110x_gdo0_enable(void)
{
    P1IFG &= ~0x08;     /* Clear IFG for GDO0 */
    P1IE |= 0x08;       /* Enable interrupt for GDO0 */
}

void cc110x_gdo0_disable(void)
{
    P1IE &= ~0x08;      /* Disable interrupt for GDO0 */
    P1IFG &= ~0x08;     /* Clear IFG for GDO0 */
}

void cc110x_gdo2_enable(void)
{
    P1IFG &= ~0x10;     /* Clear IFG for GDO2 */
    P1IE |= 0x10;       /* Enable interrupt for GDO2 */
}

void cc110x_gdo2_disable(void)
{
    P1IE &= ~0x10;      /* Disable interrupt for GDO2 */
    P1IFG &= ~0x10;     /* Clear IFG for GDO2 */
}

void cc110x_before_send(void)
{
    // Disable GDO2 interrupt before sending packet
    cc110x_gdo2_disable();
}

void cc110x_after_send(void)
{
    // Enable GDO2 interrupt after sending packet
    cc110x_gdo2_enable();
}


int cc110x_get_gdo0(void) {
        return  CC1100_GDO0;
}

int cc110x_get_gdo1(void) {
        return  CC1100_GDO1;
}

int cc110x_get_gdo2(void) {
        return  CC1100_GDO2;
}

void cc110x_spi_cs(void)
{
    CC1100_CS_LOW;
}

uint8_t cc110x_txrx(uint8_t data)
{
    /* Ensure TX Buf is empty */
    long c = 0;
    IFG2 &= ~UTXIFG1;
    IFG2 &= ~URXIFG1;
    U1TXBUF = data;
    while(!(IFG2 & UTXIFG1)) {
        if (c++ == 1000000) {
            puts("cc110x_txrx alarm()");
        }
    }
    /* Wait for Byte received */
    c = 0;
    while(!(IFG2 & URXIFG1)) {
        if (c++ == 1000000) {
            puts("cc110x_txrx alarm()");
        }
    }
    return U1RXBUF;
}


void cc110x_spi_select(void)
{
    // Switch to GDO mode
    P5SEL &= ~0x04;
    P5DIR &= ~0x04;
    cs_low:
    // CS to low
    abort_count = 0;
    CC1100_CS_LOW;
    // Wait for SO to go low (voltage regulator
    // has stabilized and the crystal is running)
    loop:
//    asm volatile ("nop");
    if (CC1100_GDO1) {
        abort_count++;
        if (abort_count > CC1100_GDO1_LOW_COUNT) {
            retry_count++;
            if (retry_count > CC1100_GDO1_LOW_RETRY) {
                puts("[CC1100 SPI] fatal error\n");
                goto final;
            }
            CC1100_CS_HIGH;
            goto cs_low;        // try again
        }
        goto loop;
    }
    final:
    /* Switch to SPI mode */
    P5SEL |= 0x04;
}

void cc110x_spi_unselect(void) {
    CC1100_CS_HIGH;
}

void cc110x_init_interrupts(void)
{
    unsigned int state = disableIRQ(); /* Disable all interrupts */
    P1SEL = 0x00;       /* must be <> 1 to use interrupts */
    P1IES |= 0x10;      /* Enables external interrupt on low edge (for GDO2) */
    P1IE |= 0x10;       /* Enable interrupt */
    P1IFG &= ~0x10;     /* Clears the interrupt flag */
    P1IE &= ~0x08;      /* Disable interrupt for GDO0 */
    P1IFG &= ~0x08;     /* Clear IFG for GDO0 */
    restoreIRQ(state);  /* Enable all interrupts */
}

void cc110x_spi_init(void)
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
 * CC1100 receive interrupt
 */
interrupt (PORT1_VECTOR) __attribute__ ((naked)) cc110x_isr(void){
    __enter_isr();
     /* Check IFG */
    if ((P1IFG & 0x10) != 0) {
        P1IFG &= ~0x10;
        cc110x_gdo2_irq();
    }
    else if ((P2IFG & 0x08) != 0) {
        cc110x_gdo0_irq();
        P1IE &= ~0x08;                // Disable interrupt for GDO0
           P1IFG &= ~0x08;                // Clear IFG for GDO0
    }
    else {
        puts("cc110x_isr(): unexpected IFG!");
        /* Should not occur - only GDO1 and GDO2 interrupts are enabled */
    }
    __exit_isr();
}
