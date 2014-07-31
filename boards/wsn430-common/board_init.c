/*
 * board_init.c - Implementation of functions to init board.
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "cpu.h"
#include "board.h"
#include "kernel_internal.h"
#include "msp430.h"
#include "debug.h"

volatile static uint32_t __msp430_cpu_speed = MSP430_INITIAL_CPU_SPEED;

void msp430_init_dco(void);

typedef enum {
    MCLK_2MHZ_SCLK_1MHZ = 1000002uL,
    MCLK_4MHZ_SCLK_1MHZ = 1000004uL,
    MCLK_8MHZ_SCLK_1MHZ = 1000008uL,
    MCLK_8MHZ_SCLK_8MHZ = 8000000uL
}speed_t;

/*---------------------------------------------------------------------------*/
static uint8_t calc_umctl(uint16_t br) {
    // from TI slaa049
    register uint8_t CMOD = 256 * br - 256 * (br + 1) / 2;
    register uint8_t c = 0;
    register int i = 0;
    register uint8_t a = CMOD;
    a <<= 1;
    do {
        if( a & 0x80 ) {           // Overflow to integer?
            a = a - 128 + CMOD;    // Yes, subtract 1.000000
            c |= 0x80;
        }
        else {
            a += CMOD;              // No, add fraction
        }
        if( i == 7 ) {
            return c;
        }
        i++;
        c >>= 1;
    } while(1);
}

static void msb_ports_init(void)
{
    // Port 1: GDO, Flash, BSL TX
    P1SEL = 0x02;    // Port1 Select: 00000010 = 0x02
    P1OUT = 0x00;    // Port1 Output: 00000000 = 0x00
    P1DIR = 0x87;    // Port1 Direction: 10000111 = 0x87

    // Port 2: GPIO, BSL RX, 1wire
    P2SEL = 0x04;    // Port2 Select: 00000100 = 0x04
    P2OUT = 0x00;    // Port2 Output: 00000000 = 0x00
    P2DIR = 0xFF;    // Port2 Direction: 11111111 = 0xFF


    // Port 3: UART
    P3SEL = 0xFE;    // Port3 Select: 11111110 = 0xFE
    P3OUT = 0x00;    // Port3 Output: 00000000 = 0x00
    P3DIR = 0xFF;    // Port3 Direction: 11111111 = 0xFF


    // Port 4: CS
    P4SEL = 0x00;    // Port4 Select: 00000000 = 0x00
    P4OUT = 0x14;    // Port4 Output: 00010100 = 0x14
    P4DIR = 0xFF;    // Port4 Direction: 11111111 = 0xFF

    // Port 5: SPI, LED
    P5SEL = 0x0E;    // Port5 Select: 00001110 = 0x0E
    P5OUT = 0x70;    // Port5 Output: 01110000 = 0x70
    P5DIR = 0x70;    // Port5 Direction: 01110000 = 0x70


    P6SEL = 0xFF;    // Port6 Select: 11111111 = 0xFF
    P6OUT = 0x00;    // Port6 Output: 00000000 = 0x00
    P6DIR = 0xFF;    // Port6 Direction: 11111000 = 0xF8

}

void msp430_set_cpu_speed(uint32_t speed)
{

    dint();
    __msp430_cpu_speed = speed;
    msp430_init_dco();
    uint16_t br;

    U0CTL = SWRST;
    U0CTL = SWRST | CHAR;          // 8-bit character
    U0TCTL = SSEL1 | TXEPT;        // UCLK = SCLK
    U0RCTL = 0;
    // activate
    ME1 |= UTXE0 + URXE0;          // Enable USART0 TXD/RXD
    br = (uint16_t)((__msp430_cpu_speed & 0xFFFFF0) / 115200uL);
    UBR00  = br;                   // set baudrate
    UBR10  = br>>8;
    UMCTL0 = calc_umctl(br);       // set modulation

    U0CTL &= ~SWRST;

    //URCTL0 |= URXEIE;            // allow chars to interrupt
    IE1  |= URXIE0;                // enable rx interrupt
    IFG1 &= ~UTXIFG0;
    eint();
}

/*---------------------------------------------------------------------------*/
void msp430_init_dco(void)
{
  /*----------------------- use external oszillator -------------------------*/
  uint16_t i;

  // Stop watchdog
  WDTCTL = WDTPW + WDTHOLD;

  BCSCTL1 = RSEL2;

  // Wait for xtal to stabilize
  do {
    IFG1 &= ~OFIFG;                // Clear oscillator fault flag
    for (i = 0xFF; i > 0; i--);    // Time for flag to set
  }
  while ((IFG1 & OFIFG) != 0);     // Oscillator fault flag still set?
  switch (__msp430_cpu_speed) {
  case MCLK_2MHZ_SCLK_1MHZ:
      BCSCTL2  = (SELM_2 | DIVM_2) | (SELS | DIVS_3);
      break;
  case MCLK_4MHZ_SCLK_1MHZ:
      BCSCTL2  = (SELM_2 | DIVM_1) | (SELS | DIVS_3);
      break;
  case MCLK_8MHZ_SCLK_1MHZ:
      BCSCTL2 = SELM_2 | (SELS | DIVS_3);
      break;
  default:
      BCSCTL2 = SELM_2 + SELS;     // MCLK and SMCLK = XT2 (safe)
      break;
  }
}

void board_init(void)
{
    msp430_cpu_init();
    msb_ports_init();

    LED_RED_ON;

    msp430_set_cpu_speed(MCLK_8MHZ_SCLK_8MHZ);
}
