/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

void pm_set(unsigned mode)
{
    switch (mode) {
        case 0:                  /* STM Sleep mode */
            /* Reset SLEEPDEEP bit of system control block */
            SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
            break;
        case 1:                 /* STM Stop mode */
            /* Clear PDDS and LPDS bits to enter stop mode on */
            /* deepsleep with voltage regulator on */
            PWR->CR &= ~(PWR_CR_PDDS | PWR_CR_LPDS);
            /* Set SLEEPDEEP bit of system control block */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            break;
        default:
            DEBUG("pm: invalid power mode selected.\n");
    }

    /* Executes a device DSB (Data Synchronization Barrier) */
    __DSB();
    /* Enter standby mode */
    __WFI();
}

void pm_off(void)
{
    /* Set PDDS to enter standby mode on deepsleep and clear flags */
    PWR->CR |= (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF);
    /* Enable WKUP pin to use for wakeup from standby mode */
    PWR->CSR |= PWR_CSR_EWUP;
    /* Set SLEEPDEEP bit of system control block */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    __DSB();
    __WFI();
}
