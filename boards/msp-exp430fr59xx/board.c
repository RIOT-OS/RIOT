/*
 * board.c - Board initialization for the MSP430FR5969 "Wolverine"
 * Copyright (C) 2018 INRIA
 *
 * Author : Loïc Saos <Loic.Saos@insa-lyon.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_wolverine
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Wolverine
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "stdio_uart.h"
#include "periph/gpio.h"

static void _ports_init(void)
{
    P1DIR = 0;
    P1OUT = 0;
    P1REN = 0;
    P1SEL0 = 0;
    P1SEL1 = 0;
    P1SELC = 0;
    P1IE = 0;
    P1IFG = 0;
    P1IES = 0;

    P2DIR = 0;
    P2OUT = 0;
    P2REN = 0;
    P2SEL0 = 0;
    P2SEL1 = 0;
    P2SELC = 0;
    P2IE = 0;
    P2IFG = 0;
    P2IES = 0;

    P3DIR = 0;
    P3OUT = 0;
    P3REN = 0;
    P3SEL0 = 0;
    P3SEL1 = 0;
    P3SELC = 0;
    P3IE = 0;
    P3IFG = 0;
    P3IES = 0;

    P4DIR = 0;
    P4OUT = 0;
    P4REN = 0;
    P4SEL0 = 0;
    P4SEL1 = 0;
    P4SELC = 0;
    P4IE = 0;
    P4IFG = 0;
    P4IES = 0;

    PJDIR = 0xff;
    PJOUT = 0;
    PJSEL0 = BIT4 | BIT5; // For XT1
    PJSEL1 = 0;
    PJSELC = 0;

    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
}

void board_init_dco(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    // Configure GPIO
    PJSEL0 = BIT4 | BIT5; // For XT1

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers

    CSCTL1 = DCOFSEL_6;                     // Set DCO to 8MHz
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers

    CSCTL4 &= ~LFXTOFF;
    do {
        CSCTL5 &= ~LFXTOFFG;    // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);  // Test oscillator fault flag

    CSCTL0_H = 0;               // Lock CS registers
}

void board_init(void)
{
    board_init_dco();

    msp430_cpu_init();

    _ports_init();

    stdio_init();

    __enable_interrupt();
}
