/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup boards
 * @{
 */

/**
 * @file
 * @brief       msb-430 common board initialization
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#include "cpu.h"
#include "irq.h"
#include "board.h"
#include "uart_stdio.h"
#include "periph_conf.h"
#include "msp430.h"
#include "debug.h"

static volatile uint32_t __msp430_cpu_speed = MSP430_INITIAL_CPU_SPEED;

void msp430_init_dco(void);

static void msb_ports_init(void)
{
    /* Port 1: Free port, for energy saving all outputs are set to zero. */
    P1SEL = 0x00;   /* Port1 I/O Function */
    P1OUT = 0x00;   /* Port1 Output register: 00000000 = 0x00 */
    P1DIR = 0xFF;   /* Port1 Direction: 11111111 = 0xFF */

    P2SEL = 0x20;   /* Port2 I/O Function */
    P2OUT = 0x00;   /* Port2 Output register: 00000000 = 0x00 */
    P2DIR = 0x1C;   /* Port2 Direction: 00011010 = 0x1C */
    /*   0 - P2.0 [IN ] - */
    /*   0 - P2.1 [OUT] - */
    /*   1 - P2.2 [IN ] - */
    /*   1 - P2.3 [OUT] - */
    /*   1 - P2.4 [OUT] - */
    /*   0 - P2.5 [IN ] - */
    /*   0 - P2.6 [IN ] - SDC Protect */
    /*   0 - P2.7 [IN ] - SDC Detect */

    P3SEL = 0x00;   /* Port3 Pins 6 & 7 for USART */
    P3OUT = 0x00;   /* Port3 Output register: 01001001: 0x49 */
    P3DIR = 0xFF;   /* Port3 Direction: 10101011: 0xAB */
    /*   1 - P3.0 */
    /*   1 - P3.1 */
    /*   0 - P3.2 */
    /*   1 - P3.3 */
    /*   0 - P3.4 [IN ] - SHT 11 DATA (OUT/IN) */
    /*   1 - P3.5 [OUT] - SHT 11 CLK */
    /*   0 - P3.6 [2-Funktion] - RS232_RxD */
    /*   1 - P3.7 [2-Funktion] - RS232_TxD */

    /* Port 4: Free port, for energy saving all outputs are set to zero. */
    P4SEL = 0x00;   /* Port4 I/O Function */
    P4OUT = 0x00;   /* Port4 Output register: 00000000 = 0x00 */
    P4DIR = 0xFF;   /* Port4 Direction: 11111111 = 0xFF */
    /*   1 - P4.0 [OUT] - unused */
    /*   1 - P4.1 [OUT] - unused */
    /*   1 - P4.2 [OUT] - unused */
    /*   1 - P4.3 [OUT] - unused */
    /*   1 - P4.4 [OUT] - unused */
    /*   1 - P4.5 [OUT] - unused */
    /*   1 - P4.6 [OUT] - unused */
    /*   1 - P4.7 [OUT] - unused */

    P5SEL = 0x00;   /* Port5 I/O Function: 00000000 = 0x00 */
    P5OUT = 0x80;   /* Port5 Output register: 00001001 = 0x09 */
    P5DIR = 0xFF;   /* Port5 Direction: 11111011 = 0xFB */
    /*   1 - P5.0 [OUT] - SDC /CS */
    /*   1 - P5.1 [OUT] - SDC DI */
    /*   0 - P5.2 [IN ] - SDC DO */
    /*   1 - P5.3 [OUT] - SDC DCLK */
    /*   1 - P5.4 [OUT] - MMA GS1 */
    /*   1 - P5.5 [OUT] - MMA GS2 */
    /*   1 - P5.6 [OUT] - MMA /SLEEP */
    /*   1 - P5.7 [OUT] - LED_RED 0-on, 1-off */

    P6SEL = 0x00;   /* Port6 I/O Function = 0x07 */
    P6OUT = 0x00;   /* Port6 Output register: 00000000 = 0x00 */
    P6DIR = 0xFF;   /* Port6 Direction: 11111000 = 0xF8 */
    /*   0 - P6.0 [AD-IN] - MMA X-Axis */
    /*   0 - P6.1 [AD-IN] - MMA Y-Axis */
    /*   0 - P6.2 [AD-IN] - MMA Z-Axis */
    /*   1 - P6.3 [OUT] - unused */
    /*   1 - P6.4 [OUT] - unused */
    /*   1 - P6.5 [OUT] - unused */
    /*   1 - P6.6 [OUT] - unused */
    /*   1 - P6.7 [OUT] - unused */
}

void msp430_set_cpu_speed(uint32_t speed)
{
    irq_disable();
    __msp430_cpu_speed = speed;
    msp430_init_dco();
    irq_enable();
}

/*---------------------------------------------------------------------------*/
void msp430_init_dco(void)
{
#if MSP430_HAS_EXTERNAL_CRYSTAL
    /*------------------ use external oszillator -----------------------*/
    uint16_t i;

    /* Stop watchdog */
    WDTCTL = WDTPW + WDTHOLD;

    //Init crystal for mclk
    //XT2 = HF XTAL
    BCSCTL1 = RSEL2;

    /* Wait for xtal to stabilize */
    do {
        IFG1 &= ~OFIFG;             /* Clear oscillator fault flag */

        for (i = 0xFF; i > 0; i--); /* Time for flag to set */
    }
    while ((IFG1 & OFIFG) != 0);    /* Oscillator fault flag still set? */

    BCSCTL2 = SELM_2 + SELS;        /* MCLK und SMCLK = XT2 (safe) */
#else
    int delta = __msp430_cpu_speed >> 12;
    unsigned int oldcapture = 0;
    unsigned int i;

    BCSCTL1 = 0xa4; /* ACLK is devided by 4. RSEL=6 no division for MCLK
             and SSMCLK. XT2 is off. */

    /* Init FLL to desired frequency using the 32762Hz crystal */
#if MSP430_HAS_DCOR
    BCSCTL2 = 0x01;
#else
    BCSCTL2 = 0x00;
#endif

    WDTCTL = WDTPW + WDTHOLD;             /* Stop WDT */
    BCSCTL1 |= DIVA1 + DIVA0;             /* ACLK = LFXT1CLK/8 */

    for (i = 0xffff; i > 0; i--);         /* Delay for XTAL to settle */

    CCTL2 = CCIS0 + CM0 + CAP;            /* Define CCR2, CAP, ACLK */
    TACTL = TASSEL1 + TACLR + MC1;        /* SMCLK, continous mode */


    while (1) {
        unsigned int compare;

        while ((CCTL2 & CCIFG) != CCIFG);   /* Wait until capture occured! */

        CCTL2 &= ~CCIFG;                    /* Capture occured, clear flag */
        compare = CCR2;                     /* Get current captured SMCLK */
        compare = compare - oldcapture;     /* SMCLK difference */
        oldcapture = CCR2;                  /* Save current captured SMCLK */

        if (delta == compare) {
            break;                            /* if equal, leave "while(1)" */
        }
        else if (delta < compare) {         /* DCO is too fast, slow it down */
            DCOCTL--;

            if (DCOCTL == 0xFF) {             /* Did DCO role under? */
                BCSCTL1--;
            }
        }
        else {                              /* -> Select next lower RSEL */
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
#endif
}

void board_init(void)
{
    msp430_cpu_init();
    msb_ports_init();

    msp430_set_cpu_speed(CLOCK_CORECLOCK);

    /* finally initialize STDIO over UART */
    uart_stdio_init();
}
