/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup boards_pttu
 * @{
 */

/**
 * @file
 * @brief   PTTU board initialization
 *
 * @author  Heiko Will
 * @author  Kaspar Schleiser
 *
 */
#include "board.h"
#include "msba2_common.h"
#include "VIC.h"
#include "cpu.h"

#define CL_CPU_DIV    4

/*---------------------------------------------------------------------------*/
void init_clks1(void)
{
    // Disconnect PLL
    PLLCON &= ~0x0002;
    pllfeed();
    while (PLLSTAT & BIT25);            // wait until PLL is disconnected before disabling - deadlock otherwise

    // Disable PLL
    PLLCON &= ~0x0001;
    pllfeed();
    while (PLLSTAT & BIT24);            // wait until PLL is disabled

    SCS |= 0x20;                        // Enable main OSC
    while( !(SCS & 0x40) );             // Wait until main OSC is usable

    /* select main OSC, 16MHz, as the PLL clock source */
    CLKSRCSEL = 0x0001;

    // Setting Multiplier and Divider values
    PLLCFG = 0x0008;                    // M=9 N=1 Fcco = 288 MHz
    pllfeed();

    // Enabling the PLL */
    PLLCON = 0x0001;
    pllfeed();

    /* Set clock divider to 4 (value+1) */
    CCLKCFG = CL_CPU_DIV - 1;           // Fcpu = 72 MHz

#if USE_USB
    USBCLKCFG = USBCLKDivValue;     /* usbclk = 288 MHz/6 = 48 MHz */
#endif
}

void bl_init_ports(void)
{
    gpio_init_ports();

    /* UART0 */
    PINSEL0 |= BIT4 + BIT6;                                 // RxD0 and TxD0
    PINSEL0 &= ~(BIT5 + BIT7);

    /*Turn Board on*/
    PINMODE0 |= BIT1;
    FIO0DIR |= BIT27;
    FIO0CLR = BIT27;

    /* 5V*/
    FIO1DIR |= BIT28; // Synch
    FIO1SET = BIT28;  // No Powersave

    FIO1DIR |= BIT27; // 5V off
    FIO1CLR = BIT27;

    /* Disable Resistors on Buttons */
    PINMODE4 |= BIT9 + BIT11;

    /* Disable Resistors on LED - and Ports to output*/
    PINMODE7 |= BIT19 + BIT21;
    PINMODE2 |= BIT1;
    FIO1DIR |= BIT0;
    FIO3DIR |= BIT25 + BIT26;
    FIO1SET = BIT0;
    FIO3SET = BIT25 + BIT26;

    // Config and Disable PA
    FIO1DIR |= BIT25 + BIT26 + BIT22;
    FIO1SET = BIT26;
    FIO1CLR = BIT25;
    FIO1CLR = BIT22; // PA /Shutdown
    FIO0DIR |= BIT26; // **  // Important: First put this Port as DA 2.0V and then turn on PA!!
    FIO0SET = BIT26; // **

    // Configure GPS
    PINMODE3 |= BIT3 + BIT7; // No Pullup on 1.17 & 1.19
    PINMODE9 |= BIT27 + BIT25; // No Pullup for Uart
    FIO1DIR |= BIT17;
    FIO1CLR = BIT17; // Turn off GPS
    FIO1DIR |= BIT19;
    FIO1CLR = BIT19; // Hold in Reset
    PINSEL9 |= BIT24 + BIT25 + BIT26 + BIT27; //4.28 & 4.29 as Uart3

    // Nanotron
    FIO2DIR &= ~BIT8;   // nanotron uC IRQ as input
    FIO1DIR |= BIT15;   // nanotron power on reset
    FIO1DIR &= ~BIT14;  // nanotron uC RESET as input
    FIO1DIR &= ~BIT10;  // nanotron uC Vcc as input
    FIO1DIR |= BIT9;    // nanotron ENABLE as output
    FIO1DIR &= ~BIT4;   // nanotron Rx/Tx as input

    FIO1CLR = BIT15;
    FIO1CLR = BIT9;     // Enable power

    PINMODE1 |= BIT1;   // No Pullup for CS
    FIO0DIR |= BIT16;   // CS as output
    FIO0SET = BIT16;    // drive cs inactive
    FIO0DIR |= BIT18 + BIT15;   // SPi Output

    // RFID
    FIO1DIR |= BIT1;    // RFID Power
    FIO1CLR = BIT1;     //

    FIO0DIR |= BIT1;    // RFID Reset
    FIO0SET = BIT1;     // Hold in Reset

    FIO0DIR &= ~BIT10;  // LED as INPUT
    FIO0DIR &= ~BIT11;  // DATA as INPUT
    PINMODE0 |= BIT19 + BIT21; // No Pullups

    // LTC4150 ARM
    FIO0DIR |= BIT5;
    FIO0CLR = BIT5;

    // LTC4150 System
    FIO0DIR |= BIT24;
    FIO0CLR = BIT24;

    // Battery Voltage (AD)
    PINMODE1 |= BIT19;
    PINSEL1 &= ~BIT19;
    PINSEL1 |= BIT18;

    //cc1100
    FIO0DIR |= BIT6 + BIT7 + BIT9;
    FIO0SET = BIT6;
    FIO0SET = BIT7 + BIT9;

    //SD
    FIO2DIR |= BIT12 + BIT13 + BIT11;
    FIO0DIR |= BIT20 + BIT22 + BIT21;

    //Tetra
    FIO2DIR |= BIT0 + BIT7;


    // No Pullups on any port
    int nopullup = BIT1 + BIT3 + BIT5 + BIT7 + BIT9 + BIT11 + BIT13 + BIT15 + BIT17 + BIT19 + BIT21 + BIT23 + BIT25 + BIT27 + BIT29 + BIT31;
    PINMODE0 = nopullup - BIT13 - BIT15 - BIT17 - BIT19;
    PINMODE1 = BIT1 + BIT3 + BIT5 + BIT7 + BIT9 + BIT11 + BIT13 + BIT15 + BIT17 + BIT19 + BIT21;
    PINMODE2 = nopullup;
    PINMODE3 = nopullup;
    PINMODE4 = nopullup;
    PINMODE5 = nopullup;
    PINMODE6 = nopullup;
    PINMODE7 = nopullup;
    PINMODE8 = nopullup;
    PINMODE9 = nopullup;
}

/** @} */
