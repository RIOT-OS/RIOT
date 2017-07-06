/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L0)
#include "stmclk.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef PM_STOP_CONFIG
/**
 * @brief Define config flags for stop mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#define PM_STOP_CONFIG (PWR_CR_LPDS | PWR_CR_FPDS)
#endif

void pm_set(unsigned mode)
{
    int deep = 0;

/* I just copied it from stm32f1/2/4, but I suppose it would work for the
 * others... /KS */
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L0)
    switch (mode) {
        case STM32_PM_STANDBY:
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF);
            /* Enable WKUP pin to use for wakeup from standby mode */
#if defined(CPU_FAM_STM32L0)
            PWR->CSR |= (PWR_CSR_EWUP1 | PWR_CSR_EWUP2);
#if !defined(CPU_MODEL_STM32L053R8)
            /* STM32L053 only have 2 wake pins */
            PWR->CSR |= PWR_CSR_EWUP3;
#endif
#else
            PWR->CSR |= PWR_CSR_EWUP;
#endif
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
        case STM32_PM_STOP:
#if defined(CPU_FAM_STM32L0)
            /* Clear PDDS to enter stop mode on */
            /*
             * Regarding LPSDSR, it's up to the user to configure it :
             * 0: Voltage regulator on during Deepsleep/Sleep/Low-power run mode
             * 1: Voltage regulator in low-power mode during
             *    Deepsleep/Sleep/Low-power run mode
             */
            PWR->CR &= ~(PWR_CR_PDDS);
#else
            /* Clear PDDS and LPDS bits to enter stop mode on */
            /* deepsleep with voltage regulator on */
            PWR->CR &= ~(PWR_CR_PDDS | PWR_CR_LPDS);
            PWR->CR |= PM_STOP_CONFIG;
#endif
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
    }
#else
    (void) mode;
#endif

    cortexm_sleep(deep);

#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L0)
    if (deep) {
        /* Re-init clock after STOP */
        stmclk_init_sysclk();
    }
#endif
}

#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L0)
void pm_off(void)
{
    irq_disable();
    pm_set(0);
}
#endif
