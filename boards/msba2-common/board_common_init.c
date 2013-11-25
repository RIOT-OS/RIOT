/******************************************************************************
Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
    http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
    scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @ingroup msba2
 * @{
 */

/**
 * @file
 * @brief       MSB-A2 board initialization
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Heiko Will
 * @author      Kaspar Schleiser
 * @author      Michael Baar <baar@inf.fu-berlin.de>
 * @author 		Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @note        $Id$
 */
#include <board.h>
#include <lpc23xx.h>
#include <VIC.h>
#include <cpu.h>
#include <config.h>
#include <string.h>
#include <flashrom.h>

#define PCRTC         BIT9
#define CL_CPU_DIV    4

#define WD_INTERVAL    10      ///< number of seconds before WD triggers

/*---------------------------------------------------------------------------*/
/**
 * @brief   Enabling MAM and setting number of clocks used for Flash memory fetch
 * @internal
 */
static void
init_mam(void)
{
    MAMCR  = 0x0000;
    MAMTIM = 0x0003;
    MAMCR  = 0x0002;
}
/*---------------------------------------------------------------------------*/
static inline void
pllfeed(void)
{
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}


void init_clks2(void)
{
    // Wait for the PLL to lock to set frequency
    while (!(PLLSTAT & BIT26));

    // Connect the PLL as the clock source
    PLLCON = 0x0003;
    pllfeed();

    /* Check connect bit status */
    while (!(PLLSTAT & BIT25));
}

void watchdog_init(void)
{
    WDCLKSEL = 0;                                   // clock source: RC oscillator
    WDMOD &= ~WDTOF;                                // clear time-out flag
    WDTC = (F_RC_OSCILLATOR / 4) * WD_INTERVAL;
}


void bl_init_clks(void)
{
    watchdog_init();
    PCONP = PCRTC;          // switch off everything except RTC
    init_clks1();
    init_clks2();
    init_mam();
}

/*---------------------------------------------------------------------------*/
