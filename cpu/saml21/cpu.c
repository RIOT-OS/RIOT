/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml21
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization for Atmel SAML21 MCUs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "cpu.h"

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    /* disable the watchdog timer */
    WDT->CTRLA.bit.ENABLE = 0;

    /* initialize the Cortex-M core */
    cortexm_init();

    /* turn on MCLK */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_GCLK;

    /* Software reset the GCLK module to ensure it is re-initialized correctly */
    GCLK->CTRLA.reg = GCLK_CTRLA_SWRST;
    while (GCLK->CTRLA.reg & GCLK_CTRLA_SWRST);
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST);

    /* set OSC16M to 16MHz */
    OSCCTRL->OSC16MCTRL.bit.FSEL = 3;

    /* Select the correct generator */
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(0));
    GCLK->GENCTRL[0].reg = (
            GCLK_GENCTRL_GENEN          /* enable gclk */
            | GCLK_GENCTRL_SRC_OSC16M
            );
}
