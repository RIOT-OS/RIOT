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
    /* pm clk enable */
    PM->APBAMASK.reg = PM_APBAMASK_PM;
    /* port clk enable */
    PM->APBBMASK.reg |= PM_APBBMASK_PORT;
    /* disable the watchdog timer */
    WDT->CTRL.bit.ENABLE = 0;

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}
