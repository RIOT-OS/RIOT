/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @}
 */

#include "cpu.h"

void clk_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* pm clk enable */
    PM->APBAMASK.reg = PM_APBAMASK_PM;
    /* port clk enable */
    PM->APBBMASK.reg |= PM_APBBMASK_PORT;
    /* disable the watchdog timer */
    WDT->CTRL.bit.ENABLE = 0;

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);

    /* Initialise clock sources and generic clocks */
    clk_init();
}

/**
 * @brief Initialise clock sources and generic clocks
 */
void clk_init(void)
{
    PM->APBAMASK.reg |= PM_APBAMASK_SYSCTRL;
    PM->APBAMASK.reg |= PM_APBAMASK_GCLK;

    SYSCTRL->OSC8M.bit.PRESC = 0;
    SYSCTRL->OSC8M.bit.ONDEMAND = 1;
    SYSCTRL->OSC8M.bit.RUNSTDBY = 0;
    SYSCTRL->OSC8M.bit.ENABLE = 1;

    /* Software reset the module to ensure it is re-initialized correctly */
    GCLK->CTRL.reg = GCLK_CTRL_SWRST;
    while (GCLK->CTRL.reg & GCLK_CTRL_SWRST);

    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    /* Select the correct generator */
    *((uint8_t*)&GCLK->GENDIV.reg) = 0;
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
    GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_CLKCTRL_GEN_GCLK0);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
}
