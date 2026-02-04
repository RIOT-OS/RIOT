/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Saurabh Singh
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_samd21
 * @ingroup     drivers_periph_pm
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

#define ENABLE_DEBUG 0
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
    int deep = 0;

    switch (mode) {
        case SAMD21_PM_STANDBY:
            /* Standby Mode
             * Potential Wake Up sources: asynchronous
             */
            deep = 1;
            break;
        case SAMD21_PM_IDLE_2:
            /* Sleep mode Idle 2
             * Potential Wake Up sources: asynchronous
             */
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_2;
            break;
        case SAMD21_PM_IDLE_1:
            /* Sleep mode Idle 1
             * Potential Wake Up sources: Synchronous (APB), asynchronous
             */
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_1;
            break;
        case SAMD21_PM_IDLE_0:
            /* Sleep mode Idle 0
             * Potential Wake Up sources: Synchronous (APB, AHB), asynchronous
            */
            PM->SLEEP.reg = SYSTEM_SLEEPMODE_IDLE_0;
            break;
    }

    sam0_cortexm_sleep(deep);
}
