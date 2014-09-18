/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samr21xxxA
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @}
 */

#include "cpu.h"

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY); //wait for DFLL to be ready
    SYSCTRL->DFLLCTRL.bit.ENABLE = 1;

    /* disable the watchdog timer */
    WDT->CTRL.bit.ENABLE = 0;
    /* GCLK setup */
    PM->APBAMASK.reg |= PM_APBAMASK_GCLK;
    /*Set up main clock generator */
    GCLK_GENDIV_Type gendiv;
        gendiv.bit.ID = 1;
        gendiv.bit.DIV = 0;
    GCLK->GENDIV = gendiv;
    while(GCLK->STATUS.bit.SYNCBUSY);
    
    GCLK_GENCTRL_Type genctrl;
        genctrl.bit.ID = 1; //Generator 0, TODO: is this correct
        genctrl.bit.SRC = GCLK_SOURCE_DFLL48M;
        genctrl.bit.GENEN = true;
        genctrl.bit.IDC = 1;
        genctrl.bit.OOV = 0;
        genctrl.bit.DIVSEL = 0;
        genctrl.bit.RUNSTDBY = 0;
    GCLK->GENCTRL = genctrl;

    while(GCLK->STATUS.bit.SYNCBUSY);
    GCLK_CLKCTRL_Type clkctrl = 
    {
    	.bit.ID = 1, // DFLL48M ref
    	.bit.GEN = 0,
    	.bit.CLKEN = 1,
    	.bit.WRTLOCK = 0
    };
    GCLK->CLKCTRL = clkctrl;
   	while(GCLK->STATUS.bit.SYNCBUSY);
}
