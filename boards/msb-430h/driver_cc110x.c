/*
 * Copyright (C) 2005, 2006, 2007, 2008 by Thomas Hillebrandt and Heiko Will
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

#define CC1100_GDO0         (P2IN & 0x02)   // read serial I/O (GDO0)
#define CC1100_GDO1         (P3IN & 0x04)   // read serial I/O (GDO1)
#define CC1100_GDO2         (P2IN & 0x01)   // read serial I/O (GDO2)

#define CC1100_CS_LOW       (P3OUT &= ~0x01)
#define CC1100_CS_HIGH      (P3OUT |= 0x01)

#define CC1100_GDO1_LOW_COUNT            (2700)          // loop count (timeout ~ 500 us) to wait
#define CC1100_GDO1_LOW_RETRY             (100)          // max. retries for GDO1 to go low

volatile int abort_count;
volatile int retry_count = 0;

void cc110x_gdo0_enable(void)
{
    P2IFG &= ~0x02;     /* Clear IFG for GDO0 */
    P2IE |= 0x02;       /* Enable interrupt for GDO0 */
}

void cc110x_gdo0_disable(void)
{
    P2IE &= ~0x02;      /* Disable interrupt for GDO0 */
    P2IFG &= ~0x02;     /* Clear IFG for GDO0 */
}

void cc110x_gdo2_enable(void)
{
    P2IFG &= ~0x01;     /* Clear IFG for GDO2 */
    P2IE |= 0x01;       /* Enable interrupt for GDO2 */
}

void cc110x_gdo2_disable(void)
{
    P2IE &= ~0x01;      /* Disable interrupt for GDO2 */
    P2IFG &= ~0x01;     /* Clear IFG for GDO2 */
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


int cc110x_get_gdo0(void)
{
    return  CC1100_GDO0;
}

int cc110x_get_gdo1(void)
{
    return  CC1100_GDO1;
}

int cc110x_get_gdo2(void)
{
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
    IFG1 &= ~UTXIFG0;
    IFG1 &= ~URXIFG0;
    TXBUF0 = data;

    while (!(IFG1 & UTXIFG0)) {
        if (c++ == 1000000) {
            puts("cc110x_txrx alarm()");
        }
    }

    /* Wait for Byte received */
    c = 0;

    while (!(IFG1 & URXIFG0)) {
        if (c++ == 1000000) {
            puts("cc110x_txrx alarm()");
        }
    }

    return RXBUF0;
}


void cc110x_spi_select(void)
{
    // Switch to GDO mode
    P3SEL &= ~0x04;
    P3DIR &= ~0x04;
cs_low:
    // CS to low
    abort_count = 0;
    CC1100_CS_LOW;
    // Wait for SO to go low (voltage regulator
    // has stabilized and the crystal is running)
loop:

    //  asm volatile ("nop");
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
    P3SEL |= 0x04;
}

void cc110x_spi_unselect(void)
{
    CC1100_CS_HIGH;
}

void cc110x_init_interrupts(void)
{
    unsigned int state = disableIRQ(); /* Disable all interrupts */
    P2SEL = 0x00;       /* must be <> 1 to use interrupts */
    P2IES |= 0x01;      /* Enables external interrupt on low edge (for GDO2) */
    P2IE |= 0x01;       /* Enable interrupt */
    P2IFG &= ~0x01;     /* Clears the interrupt flag */
    P2IE &= ~0x02;      /* Disable interrupt for GDO0 */
    P2IFG &= ~0x02;     /* Clear IFG for GDO0 */
    restoreIRQ(state);  /* Enable all interrupts */
}

void cc110x_spi_init(void)
{
    // Switch off async UART
    while (!(UTCTL0 & TXEPT));  // Wait for empty UxTXBUF register

    IE1 &= ~(URXIE0 + UTXIE0);  // Disable USART0 receive&transmit interrupt
    ME1 &= ~(UTXE0 + URXE0);
    P3SEL |= 0x0E;              // Set pin as SPI

    // Keep peripheral in reset state
    UCTL0  = SWRST;

    // 8-bit SPI Master 3-pin mode, with SMCLK as clock source
    // CKPL works also, but not CKPH+CKPL or none of them!!
    UCTL0 |= CHAR + SYNC + MM;
    UTCTL0 = CKPH + SSEL1 + SSEL0 + STC;

    // Ignore clockrate argument for now, just use clock source/2
    // SMCLK = 7,3728 MHz
    UBR00 = 0x02;  // Ensure baud rate >= 2
    UBR10 = 0x00;
    UMCTL0 = 0x00; // No modulation
    URCTL0 = 0x00; // Reset Receive Control Register

    // Enable SPI mode
    ME1 |= USPIE0;

    // Release for operation
    UCTL0  &= ~SWRST;
}


// #include <msp430x16x.h>
// #include <signal.h>
// #include "type.h"
// #include "cc110x_defines.h"
// #include "driver_cc110x.h"
// #include "driver_system.h"
// #include "spi0.h"
//
// static callback_t _paket_cb;
// static callback_t _cs_cb;
//
// //-------------------------------------------------------------------------------------------------------
// // Public CC1100 communication functions (SPI)
// //-------------------------------------------------------------------------------------------------------
//
// //-------------------------------------------------------------------------------------------------------
// //  void spiInitTrx(void)
// //
// //  DESCRIPTION:
// //       This function puts the cc110x into spi mode. You have to call this bevore every spi transaction.
// //
// //-------------------------------------------------------------------------------------------------------
//
//
// void drivercc110x_spiwriteburstreg(uint8_t addr, unsigned char *buffer, uint8_t count)
// {
//  uint8_t i;
//  long c;
//  drivercc110x_spiinittrx();
//  drivercc110x_trxspi(addr | CC1100_WRITE_BURST);
//  for (i = 0; i < count; i++)
//  {
//      c = 0;
//      IFG1 &= ~UTXIFG0;
//      IFG1 &= ~URXIFG0;
//      TXBUF0 = buffer[i];
//      /* Wait for TX to finish */
//      while(!(IFG1 & UTXIFG0))
//      {
//          if (c++ == 1000000)
//              alarm();
//      }
//  }
//  /* Wait for Byte received */
//  c = 0;
//  while(!(IFG1 & URXIFG0))
//  {
//      if (c++ == 1000000)
//          alarm();
//  }
//  CC1100_CS_HIGH;
// }
//
// void drivercc110x_spireadburstreg(uint8_t addr, char *buffer, uint8_t count)
// {
//  uint8_t i;
//  drivercc110x_spiinittrx();
//  drivercc110x_trxspi(addr | CC1100_READ_BURST);
//  for (i = 0; i < count; i++)
//  {
//      long c = 0;
//      IFG1 &= ~UTXIFG0;
//      IFG1 &= ~URXIFG0;
//      TXBUF0 = NOBYTE;
//      while(!(IFG1 & UTXIFG0))
//      {
//          if (c++ == 1000000)
//              alarm();
//      }
//      /* Wait for Byte received */
//      c = 0;
//      while(!(IFG1 & URXIFG0))
//      {
//          if (c++ == 1000000)
//              alarm();
//      }
//      buffer[i] = RXBUF0;
//  }
//  CC1100_CS_HIGH;
// }
//
// void drivercc110x_load(callback_t cs_cb,callback_t paket_cb)
// {
//  _paket_cb = paket_cb;
//  _cs_cb = cs_cb;
//  spi0_init(0);
// }
//
// void drivercc110x_aftersend(void)
// {
//     CLEAR(P2IFG, 0x01);
//  SET(P2IE, 0x01); /* Enable interrupts on port 2 pin 0 */
//  CLEAR(P4OUT, 0x08); /* Turn off Sending Led*/
// }
//
// void drivercc110x_initinterrupts(void)
// {
//  _DINT(); /* Disable all interrupts */
//  P2SEL = 0x00; /* must be <> 1 to use interrupts */
//  SET(P2IES, 0x01); /* Enables external interrupt on low edge (for GDO2) */
//  SET(P2IE, 0x01); /* Enable interrupt */
//     CLEAR(P2IFG, 0x01); /* Clears the interrupt flag */
//  CLEAR(P2IE, 0x02); /* Disable interrupt for GDO0 */
//  CLEAR(P2IFG, 0x02); /* Clear IFG for GDO0 */
//  _EINT(); /* Enable all interrupts */
// }
//
// void drivercc110x_beforesend(void)
// {
//  /* Turn on Led while sending paket for debug reasons */
//  SET(P4OUT, 0x08);
//  /* Disable interrupts on port 2 pin 0 */
//  CLEAR(P2IE, 0x01);
// }
//
//
// /*
//  * Private functions
//  */
//
//

/*
 * CC1100 receive interrupt
 */
interrupt(PORT2_VECTOR) __attribute__((naked)) cc110x_isr(void)
{
    __enter_isr();
    //  if (system_state.MT_TRANSCEIVER_POWERDOWN) SPI_INIT; /* Initialize SPI after wakeup */
    /* Check IFG */
    if ((P2IFG & 0x01) != 0) {
        P2IFG &= ~0x01;
        cc110x_gdo2_irq();
    }
    else if ((P2IFG & 0x02) != 0) {
        cc110x_gdo0_irq();
        P2IE &= ~0x02;              // Disable interrupt for GDO0
        P2IFG &= ~0x02;             // Clear IFG for GDO0
    }
    else {
        puts("cc110x_isr(): unexpected IFG!");
        /* Should not occur - only Port 2 Pin 0 interrupts are enabled */
        //      CLEAR(P2IFG, 0xFF); /* Clear all flags */
    }

    //  if (system_state.MT_TRANSCEIVER_POWERDOWN != 0) END_LPM3;
    __exit_isr();
}
