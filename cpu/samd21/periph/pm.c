/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Saurabh Singh
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
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Saurabh Singh <saurabh@cezy.co>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

enum system_sleepmode {
    /**
     *  Idle 0 mode.
     *  Potential Wake Up sources: Synchronous(APB, AHB), asynchronous.
     */
    SYSTEM_SLEEPMODE_IDLE_0,
    /**
     *  Idle 1 mode.
     *  Potential Wake Up sources: Synchronous (APB), asynchronous
     */
    SYSTEM_SLEEPMODE_IDLE_1,
    /**
     *  Idle 2 mode.
     *  Potential Wake Up sources: Asynchronous
     */
    SYSTEM_SLEEPMODE_IDLE_2,
    /**
     * Standby mode.
     * Potential Wake Up sources: Asynchronous
     */
    SYSTEM_SLEEPMODE_STANDBY,
};

void pm_set(unsigned mode)
{
    switch (mode) {
        case 0:
            /* Standby Mode
             * Potential Wake Up sources: asynchronous
             */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            break;
        case 1:
            /* Sleep mode Idle 2
             * Potential Wake Up sources: asynchronous
             */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_2;
            break;
        case 2:
            /* Sleep mode Idle 1
             * Potential Wake Up sources: Synchronous (APB), asynchronous
             */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_1;
            break;
        case 3:
            /* Sleep mode Idle 0
             * Potential Wake Up sources: Synchronous (APB, AHB), asynchronous
            */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_0;
            break;
    }

    /* Executes a device DSB (Data Synchronization Barrier) */
    __DSB();
    /* Enter standby mode */
    __WFI();
}
