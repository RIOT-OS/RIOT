/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_ezr32wg
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"

/**
 * @brief   Configure clock sources and the CPU frequency
 *
 * On the ezr32wg cpu, we basically have two options for selecting the main
 * clock source, using an external clock source (e.g. a crystal), or using the
 * internal RC oscillator (enabled by default).
 */
static void clk_init(void)
{
    /* reset the clock to it's default values */
    CMU->CTRL = _CMU_CTRL_RESETVALUE;

#ifdef CLOCK_HFXO
    /* configure clock system for external oscillator */
#if CLOCK_HFXO > 32000000
    CMU->CTRL |= CMU_CTRL_HFXOBUFCUR_BOOSTABOVE32MHZ;
#endif
    /* enable the oscillator */
    CMU->OSCENCMD = CMU_OSCENCMD_HFXOEN;
    /* wait for external oscillator to be stable */
    while (!(CMU->STATUS & CMU_STATUS_HFXORDY));
    /* switch to the external oscillator */
    CMU->CMD = CMU_CMD_HFCLKSEL_HFXO;
    /* wait until clock source is applied */
    while (!(CMU->STATUS & CMU_STATUS_HFXOSEL));
#endif

    /* set the core clock divider */
    CMU->HFCORECLKDIV = (CLOCK_HFCORECLKDIV - 1);
    /* configure the high speed peripheral clock: set divider and enable it */
    CMU->HFPERCLKDIV = (CMU_HFPERCLKDIV_HFPERCLKEN | (CLOCK_HFPERCLKDIV - 1));
}

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* Initialise clock sources and generic clocks */
    clk_init();
    /* trigger static peripheral initialization */
    periph_init();
}
