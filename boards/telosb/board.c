/*
 * board.c - Board initiazilation for the TelosB
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "cpu.h"
#include "board.h"

void uart_init(void);

static void telosb_ports_init(void)
{
    /* Port 1: GDO, Flash, BSL TX */
    P1SEL = 0x02;    /* Port1 Select: 00000010 = 0x02 */
    P1OUT = 0x00;    /* Port1 Output: 00000000 = 0x00 */
    P1DIR = 0x87;    /* Port1 Direction: 10000111 = 0x87 */

    /* Port 2: GPIO, BSL RX, 1wire */
    P2SEL = 0x04;    /* Port2 Select: 00000100 = 0x04 */
    P2OUT = 0x00;    /* Port2 Output: 00000000 = 0x00 */
    P2DIR = 0xFF;    /* Port2 Direction: 11111111 = 0xFF */

    /* Port 3: UART and SPI */
    P3SEL = 0x0E;    /* Port3 Select: 11001110 = 0xCE */
    P3OUT = 0x00;    /* Port3 Output: 00000000 = 0x00 */
    P3DIR = 0xFE;    /* Port3 Direction: 01001110 = 0x4E */

    /* Port 4: CS */
    P4SEL = 0x02;    /* Port4 Select: 00000010 = 0x02 */
    P4OUT = 0x04;    /* Port4 Output: 00000100 = 0x04 */
    P4DIR = 0x64;    /* Port4 Direction: 01100100 = 0x64 */

    /* Port 5: SPI, LED */
    P5SEL = 0x00;    /* Port5 Select: 00000000 = 0x00 */
    P5OUT = 0x70;    /* Port5 Output: 01110000 = 0x70 */
    P5DIR = 0x70;    /* Port5 Direction: 01110000 = 0x70 */

    P6SEL = 0xFF;    /* Port6 Select: 11111111 = 0xFF */
    P6OUT = 0x00;    /* Port6 Output: 00000000 = 0x00 */
    P6DIR = 0xFF;    /* Port6 Direction: 11111000 = 0xFF */

}

/*---------------------------------------------------------------------------*/
/* taken from Contiki code */
void msp430_init_dco(void)
{
    /* This code taken from the FU Berlin sources and reformatted. */
#define DELTA    (F_CPU / (F_RC_OSCILLATOR / 8))

    unsigned int oldcapture = 0;
    unsigned int i;

    /* 10100100 = XT2 is off, ACLK divided by 4, RSELx=4 */
    BCSCTL1 = XT2OFF | DIVA_2 | RSEL2;

    /* Init undivided DCO with internal resistor for MCLK and SMCLK
     * DCO = 32762Hz -> FLL = 2,4576 MHz */
    BCSCTL2 = 0x00;

    BCSCTL1 |= DIVA1 + DIVA0;             /* ACLK = LFXT1CLK/8 */

    for (i = 0xFFFF; i > 0; i--) {        /* Delay for XTAL to settle */
        __asm__("nop");
    }

    CCTL2 = CCIS0 + CM0 + CAP;            /* Define CCR2, CAP, ACLK */
    TACTL = TASSEL1 + TACLR + MC1;        /* SMCLK, continuous mode */

    while (1) {
        unsigned int compare;

        while ((CCTL2 & CCIFG) != CCIFG);   /* Wait until capture occurred!*/

        CCTL2 &= ~CCIFG;                    /* Capture occurred, clear flag */
        compare = CCR2;                     /* Get current captured SMCLK */
        compare = compare - oldcapture;     /* SMCLK difference */
        oldcapture = CCR2;                  /* Save current captured SMCLK */

        if (DELTA == compare) {
            break;                            /* if equal, leave "while (1)" */
        }
        else if (DELTA < compare) {        /* DCO is too fast, slow it down */
            DCOCTL--;

            if (DCOCTL == 0xFF) {             /* Did DCO role under? */
                BCSCTL1--;
            }
        }
        else {                            /* -> Select next lower RSEL */
            DCOCTL++;

            if (DCOCTL == 0x00) {             /* Did DCO role over? */
                BCSCTL1++;
            }

            /* -> Select next higher RSEL  */
        }
    }

    CCTL2 = 0;                            /* Stop CCR2 function */
    TACTL = 0;                            /* Stop Timer_A */

    BCSCTL1 &= ~(DIVA1 + DIVA0);          /* remove /8 divisor from ACLK again */
}

//=========================== public ==========================================

void board_init(void)
{
    /* disable watchdog timer */
    WDTCTL     =  WDTPW + WDTHOLD;

    telosb_ports_init();
    msp430_init_dco();

    /* enable interrupts */
    __bis_SR_register(GIE);
}
