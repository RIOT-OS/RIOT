/*
 * SPDX-FileCopyrightText: 2008-2009 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc23xx
 * @{
 * @file
 * @brief       LPC23XX family clock initialization
 *
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Michael Baar <baar@inf.fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @note        $Id$
 *
 * @}
 */
#include <string.h>
#include "board.h"
#include "cpu.h"
#include "periph_conf.h"
#include "lpc23xx.h"

#define CL_CPU_DIV      4

#define WD_INTERVAL     10      /**< number of seconds before WD triggers */

/**
 * @brief Feed sequence for PLL register
 */
static inline void pllfeed(void)
{
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}

/**
 * @brief   Enabling MAM and setting number of clocks used for Flash memory fetch
 * @internal
 */
void cpu_init_mam(void)
{
    MAMCR  = 0x0000;
    MAMTIM = 0x0003;
    MAMCR  = 0x0002;
}

void cpu_init_pll(void)
{
    /* Disconnect PLL */
    PLLCON &= ~0x0002;
    pllfeed();

    while (PLLSTAT & BIT25) {}          /* wait until PLL is disconnected before
                                         * disabling - deadlock otherwise */
    /* Disable PLL */
    PLLCON &= ~0x0001;
    pllfeed();

    while (PLLSTAT & BIT24) {}          /* wait until PLL is disabled */

    SCS |= 0x10;                        /* main OSC between 15MHz and 24MHz (more stable in tests) */
    SCS |= 0x20;                        /* Enable main OSC */

    while (!(SCS & 0x40)) {}            /* Wait until main OSC is usable */

#ifdef XTAL_HZ
    /* select main OSC (XTAL_HZ) as the PLL clock source */
    CLKSRCSEL = 0x0001;
#else
    /* use the internal RC OSC as the PLL clock source */
#define XTAL_HZ F_RC_OSCILLATOR
#endif

    /* Setting Multiplier and Divider values */
    /* Fcco = (2 * Fin *  M)/ N = 288 MHz */
    PLLCFG = PLLCFG_M(F_CCO/(XTAL_HZ)) | PLLCFG_N(2);
    pllfeed();

    /* Enabling the PLL */
    PLLCON = 0x0001;
    pllfeed();

    /* Set clock divider to 4 (value+1) */
    CCLKCFG = CL_CPU_DIV - 1;           /* Fcpu = 72 MHz */

    /* Wait for the PLL to lock to set frequency */
    while (!(PLLSTAT & BIT26)) {}

    /* Connect the PLL as the clock source */
    PLLCON = 0x0003;
    pllfeed();

    /* Check connect bit status */
    while (!(PLLSTAT & BIT25)) {}
}

static void watchdog_init(void)
{
    WDCLKSEL = 0;    /* clock source: RC oscillator */
    WDMOD &= ~WDTOF; /* clear time-out flag */
    WDTC = (F_RC_OSCILLATOR / 4) * WD_INTERVAL;
}

void cpu_init_clks(void)
{
    watchdog_init();
    PCONP = PCRTC; /* switch off everything except RTC */
    cpu_init_pll();
    cpu_init_mam();
}
