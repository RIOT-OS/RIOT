/*
 * board_init.c - initialization of the AVSEXTREM-BOARD.
 * Copyright (C) 2013 Heiko Will <hwill@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       avsextrem board initialization
 *
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Michael Baar
 * @author      Zakaria Kasmi
 * @author      Marco Ziegert
 * @author      Stefan Pfeiffer
 *
 * @note        $Id: board_init.c 1071 2013-09-19 16:50:56 kasmi $
 */
#include "cpu.h"

/*---------------------------------------------------------------------------*/
void board_init(void)
{
    //PTTU:

    /*Turn Board on*/
    //  PINMODE0 |= BIT1;
    //  FIO0DIR |= BIT27;
    //  FIO0CLR = BIT27;
    //  0.27

    /* 5V*/
    //  FIO1DIR |= BIT28; // Synch
    //  FIO1SET = BIT28;  // No Powersave
    //  1.28

    //  FIO1DIR |= BIT27; // 5V off
    //  FIO1CLR = BIT27;
    //  1.27

    /* Disable Resistors on Buttons */
    //  PINMODE4 |= BIT9 + BIT11;
    //

    /* Disable Resistors on LED - and Ports to output*/
    PINMODE7 |= BIT19 + BIT21; //3.25 + 3.26
    PINMODE2 |= BIT1;          //1.0
    FIO1DIR |= BIT0;
    FIO3DIR |= BIT25 + BIT26;
    FIO1SET = BIT0;        //all off
    FIO3SET = BIT25 + BIT26;

    // Config and Disable PA
    //  FIO1DIR |= BIT25 + BIT26 + BIT22;
    //  FIO1SET = BIT26;
    //  FIO1CLR = BIT25;
    //  FIO1CLR = BIT22;  // PA /Shutdown
    // Important: First put this Port as DA 2.0V and then turn on PA!!
    //  FIO0DIR |= BIT26;
    //  FIO0SET = BIT26;
    //  1.22 + 1.25 + 1.26

    // Configure GPS
    PINMODE3 |= BIT3 + BIT7;   // No Pullup on 1.17 & 1.19
    PINMODE9 |= BIT27 + BIT25; // No Pullup for Uart
    FIO1DIR |= BIT17;
    FIO1CLR = BIT17;           // Turn off GPS
    FIO1DIR |= BIT19;
    FIO1CLR = BIT19;           // Hold in Reset
    PINSEL9 |= BIT24 + BIT25 + BIT26 + BIT27; //4.28 & 4.29 as Uart3

    // Nanotron
    FIO2DIR &= ~BIT8;   // nanotron uC IRQ as input
    FIO1DIR |= BIT15;   // nanotron power on reset
    FIO1DIR &= ~BIT14;  // nanotron uC RESET as input
    FIO1DIR &= ~BIT10;  // nanotron uC Vcc as input
    FIO1DIR |= BIT9;    // nanotron ENABLE as output
    FIO1DIR &= ~BIT4;   // nanotron Rx/Tx as input

    FIO1CLR = BIT15;
    FIO1CLR = BIT9; // Enable power

    PINMODE1 |= BIT1;   // No Pullup for CS
    FIO0DIR |= BIT16;   // CS as output
    FIO0SET = BIT16;    // drive cs inactive
    FIO0DIR |= BIT18 + BIT15;   // SPi Output

    // RFID
    //  FIO1DIR |= BIT1;    // RFID Power
    //  FIO1CLR = BIT1;
    //
    //  FIO0DIR |= BIT1;    // RFID Reset
    //  FIO0SET = BIT1;     // Hold in Reset
    //
    //  FIO0DIR &= ~BIT10;  // LED as INPUT
    //  FIO0DIR &= ~BIT11;  // DATA as INPUT
    //  PINMODE0 |= BIT19 + BIT21; // No Pullups
    //  1.1 + 0.1 + 0.10 + 0.11

    // LTC4150 ARM
    FIO0DIR |= BIT5;
    FIO0CLR = BIT5;

    // LTC4150 System
    //  FIO0DIR |= BIT24;
    //  FIO0CLR = BIT24;
    //  0.23 + 0.24

    // Battery Voltage (AD)
    PINMODE1 |= BIT19;        //0.25
    PINSEL1 &= ~BIT19;
    PINSEL1 |= BIT18;

    //cc1100
    FIO0DIR |= BIT6 + BIT7 + BIT9;
    FIO0SET = BIT6;
    FIO0SET = BIT7 + BIT9;

    //SMB380
    FIO1DIR |= BIT20 + BIT21 + BIT24; //SCK + CSB + SDI
    FIO1SET = BIT21;
    FIO1SET = BIT20 + 24;

    //SHT11
    FIO1DIR |= BIT25;            //1.25
    PINSEL3 &= ~(BIT12 | BIT13 | BIT18 | BIT19);

    //SD
    FIO0DIR &= ~(BIT19 | BIT20 | BIT21 | BIT22) ; //0.19 0.20 0.21 0.22 as input
    PINMODE1 |= (BIT7) | (BIT9) | (BIT11) | (BIT13); // no resistors
    FIO2DIR &= ~(BIT11 + BIT12 + BIT13); //2.11 2.12 2.13 as input
    PINMODE4 |= (BIT23) | (BIT25) | (BIT27); // no resistors
}
