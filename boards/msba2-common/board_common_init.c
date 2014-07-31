/*
 * Copyright (C) 2008-2009, Freie Universitaet Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup msba2
 * @{
 */

/**
 * @file
 * @brief       MSB-A2 board initialization
 *
 * @author      Heiko Will
 * @author      Kaspar Schleiser
 * @author      Michael Baar <baar@inf.fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @note        $Id$
 */
#include <string.h>
#include "board.h"
#include "msba2_common.h"
#include "lpc23xx.h"
#include "cpu.h"
#include "config.h"

#define PCRTC           BIT9
#define CL_CPU_DIV      4

#define WD_INTERVAL     10      ///< number of seconds before WD triggers

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
