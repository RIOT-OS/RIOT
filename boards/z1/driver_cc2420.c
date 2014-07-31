/*
 * driver_cc2420.c - Implementation of the board dependent cc2420 functions
 *  for Zolertia Z1.
 * Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 * Copyright (C) 2014 Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_z1
 * @{
 *
 * @file        driver_cc2420.c
 * @brief       Board specific CC2420 driver HAL for the Zolertia Z1
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
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

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define CC2420_RESETn_PIN   0x40                        /* RADIO_RESET   <-> P4.6 */
#define CC2420_VREGEN_PIN   0x20                        /* RADIO_VREG_EN <-> P4.5 */

#define CC2420_FIFOP_PIN    0x04
#define CC2420_GIO0_PIN     0x08
#define CC2420_GIO1_PIN     0x10

#define CC2420_SFD_PIN      0x02

#define CC2420_FIFOP        (P1IN & CC2420_FIFOP_PIN)   /* FIFOP <-> packet interrupt (P1.2) */
#define CC2420_GIO0         (P1IN & CC2420_GIO0_PIN)    /* FIFO  <-> GIO0 - RX data available (P1.3) */
#define CC2420_GIO1         (P1IN & CC2420_GIO1_PIN)    /* CCA   <-> GIO1 - clear channel (P1.4) */
#define CC2420_SFD          (P4IN & CC2420_SFD_PIN)     /* SFD   <-> TBL - start frame delimiter (P4.1) */

#define CC2420_CS_LOW       (P3OUT &= ~0x01)            /* RADIO_CS <-> P3.0 */
#define CC2420_CS_HIGH      (P3OUT |=  0x01)

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
    /* However there is no interrupt on MSP430F2617 port 4 */
}

void cc2420_after_send(void)
{
    /* Enable SFD interrupt after sending packet */
    /* However there is no interrupt on MSP430F2617 port 4 */
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
    CC2420_CS_LOW;   /* Chip Select line is active-low */
}

#define MAX_SPI_WAIT 1000

uint8_t cc2420_txrx(uint8_t data)
{
    /* Wait for SPI to be ready for transmission */
    long count = 0;
    do {
        count++;
        if (count >= MAX_SPI_WAIT) {
            core_panic(0x2420, "cc2420_txrx(): SPI never ready for TX!");
        }
    } while (!(IFG2 & UCB0TXIFG));
    /* Transmit data byte to CC2420, and wait for end of transmission */
    IFG2 &= ~UCB0RXIFG;
    UCB0TXBUF = data;
    count = 0;
    do {
    	count++;
        if (count >= MAX_SPI_WAIT) {
            core_panic(0x2420, "cc2420_txrx(): couldn't send byte!");
        }
    } while (UCB0STAT & UCBUSY);
    /* Read the byte that CC2420 has (normally, during TX) returned */
    count = 0;
    do {
    	count++;
        if (count >= MAX_SPI_WAIT) {
            core_panic(0x2420, "cc2420_txrx(): couldn't receive byte!");
        }
    } while (!(IFG2 & UCB0RXIFG));
    /* Return received byte */
    return UCB0RXBUF;
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

    /* done in board.c : function z1_ports_init()
    P1SEL &= ~CC2420_FIFOP_PIN;         // must be <> 1 to use interrupts
    P1SEL &= ~CC2420_GIO0_PIN;          // must be <> 1 to use interrupts
    */

    /* FIFO <-> GIO0 interrupt */
    P1IES |=  CC2420_GIO0_PIN;          /* Enable external interrupt on falling edge for GIO0/FIFO */
    P1IE  |=  CC2420_GIO0_PIN;
    P1IFG &= ~CC2420_GIO0_PIN;          /* Clear the interrupt flag */

    /* FIFOP <-> Packet interrupt */
    P1IES &= ~CC2420_FIFOP_PIN;         /* Enable external interrupt on rising edge for FIFOP */
    P1IE  |=  CC2420_FIFOP_PIN;
    P1IFG &= ~CC2420_FIFOP_PIN;         /* Clear IFG for FIFOP */

    restoreIRQ(state);                  /* Enable all interrupts */
}

void cc2420_spi_init(void)
{
    /*
     * NOTES :
     * - we will use SPI mode using block B of the USCI0,
     *   so as to avoid disturbing UART0 which is managed by USCI0 block A
     * - MCU pin (GPIO port) initialisation is done in board.c,
     *   function z1_ports_init().
     */

    /* Keep peripheral in reset state during configuration */
    UCB0CTL1 = UCSWRST;

    /* 8-bit SPI Master 3-pin mode, MSB first,
       with SMCLK as clock source */
    UCB0CTL0  = UCSYNC + UCMST + UCMODE_0 + UCMSB;
    UCB0CTL1 |= UCSSEL_3;

    UCB0CTL0 |= UCCKPH;  /* Data captured on rising edge, changed on falling */
    UCB0CTL0 &= ~UCCKPL; /* SPI data lines are active-high/inactive-low */

    /* Ignore clockrate argument for now, just use clock source/2 */
    UCB0BR0   = 0x02;  /* Ensure baud rate <= SMCLK/2 */
    UCB0BR1   = 0x00;

    /* Release for operation */
    UCB0CTL1 &= ~UCSWRST;
}

/*
 * CC2420 receive interrupt
 */
interrupt (PORT1_VECTOR) __attribute__ ((naked)) cc2420_isr(void)
{
    __enter_isr();

     /* Check if FIFOP signal is raising => RX interrupt */
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
        /* Should not occur - only FIFOP and GIO0 interrupts are enabled */
    }

    __exit_isr();
}
