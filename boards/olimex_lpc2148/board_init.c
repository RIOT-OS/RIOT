/*
 * bl_board_init.c
 *
 *  Created on: 19.08.2008
 *      Author: heiko, kaspar
 */

#include "cpu.h"
#include "bits.h"
#include "VIC.h"

#define PLOCK 0x400

static void feed(void)
{
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
}

void bl_init_clks(void)
{

    // 				Setting the Phased Lock Loop (PLL)
    //               ----------------------------------
    //
    // Olimex LPC-P2148 has a 12.0000 mhz crystal
    //
    // We'd like the LPC2148 to run at 60 mhz (has to be an even multiple of crystal)
    //
    // According to the Philips LPC2148 manual:   M = cclk / Fosc	where:	M    = PLL multiplier (bits 0-4 of PLLCFG)
    //																		cclk = 60000000 hz
    //																		Fosc = 12000000 hz
    //
    // Solving:	M = 60000000 / 12000000 = 5
    //
    //			Note: M - 1 must be entered into bits 0-4 of PLLCFG (assign 4 to these bits)
    //
    //
    // The Current Controlled Oscilator (CCO) must operate in the range 156 mhz to 320 mhz
    //
    // According to the Philips LPC2148 manual:	Fcco = cclk * 2 * P    where:	Fcco = CCO frequency
    //																			cclk = 60000000 hz
    //																			P = PLL divisor (bits 5-6 of PLLCFG)
    //
    // Solving:	Fcco = 60000000 * 2 * P
    //			P = 2  (trial value)
    //			Fcco = 60000000 * 2 * 2
    //			Fcc0 = 240000000 hz    (good choice for P since it's within the 156 mhz to 320 mhz range)
    //
    // From Table 22 (page 34) of Philips LPC2148 manual    P = 2, PLLCFG bits 5-6 = 1  (assign 1 to these bits)
    //
    // Finally:      PLLCFG = 0  01  00100  =  0x24
    //
    // Final note: to load PLLCFG register, we must use the 0xAA followed 0x55 write sequence to the PLLFEED register
    //             this is done in the short function feed() below
    //

    // Setting Multiplier and Divider values
    PLL0CFG = 0x24;
    feed();

    // Enabling the PLL */
    PLL0CON = 0x1;
    feed();

    // Wait for the PLL to lock to set frequency
    while (!(PLL0STAT & PLOCK)) ;

    // Connect the PLL as the clock source
    PLL0CON = 0x3;
    feed();

    // Enabling MAM and setting number of clocks used for Flash memory fetch
    MAMTIM = 0x3;
    MAMCR = 0x2;

    // Setting peripheral Clock (pclk) to 1/2 System Clock (cclk)
    VPBDIV = PCLK_DIV;
}




void bl_init_ports(void)
{

}

