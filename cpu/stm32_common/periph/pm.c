/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *               2017-2019 OTA keys S.A.
 *               2019 Inria
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
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#include "stmclk.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef PM_STOP_CONFIG
/**
 * @brief Define config flags for stop mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#if defined(CPU_FAM_STM32F0)
#define PM_STOP_CONFIG  (PWR_CR_LPDS)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define PM_STOP_CONFIG  (PWR_CR_LPSDSR | PWR_CR_ULP)
#elif defined(CPU_FAM_STM32L4)
#define PM_STOP_CONFIG  (PWR_CR1_LPMS_STOP1)
#else
#define PM_STOP_CONFIG  (PWR_CR_LPDS | PWR_CR_FPDS)
#endif
#endif

#ifndef PM_STANDBY_CONFIG
/**
 * @brief Define config flags for standby mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define PM_STANDBY_CONFIG   (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF | PWR_CR_ULP)
#elif defined(CPU_FAM_STM32L4)
#define PM_STANDBY_CONFIG   (PWR_CR1_LPMS_STANDBY)
#else
#define PM_STANDBY_CONFIG   (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF)
#endif
#endif

#if defined(CPU_FAM_STM32L4)
#define PWR_CR_REG     PWR->CR1
#define PWR_WUP_REG    PWR->CR3
#else
#define PWR_CR_REG     PWR->CR
#define PWR_WUP_REG    PWR->CSR
#endif

void pm_set(unsigned mode)
{
    int deep;

    switch (mode) {
#ifdef STM32_PM_STANDBY
        case STM32_PM_STANDBY:
            PWR_CR_REG &= ~(PM_STOP_CONFIG | PM_STANDBY_CONFIG);
            PWR_CR_REG |= PM_STANDBY_CONFIG;
#if defined(CPU_FAM_STM32L4)
            /* Disable SRAM2 retention for maximum power saving */
            PWR->CR3 &= ~PWR_CR3_RRS;
            /* Clear flags */
            PWR->SCR |= PWR_SCR_CSBF;
#endif
            /* Enable WKUP pin to use for wakeup from standby mode */
            PWR_WUP_REG |= PM_EWUP_CONFIG;
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
#endif
#ifdef STM32_PM_STOP
        case STM32_PM_STOP:
            PWR_CR_REG &= ~(PM_STOP_CONFIG | PM_STANDBY_CONFIG);
            PWR_CR_REG |= PM_STOP_CONFIG;
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
#endif
        default:
            deep = 0;
            break;
    }

    cortexm_sleep(deep);

    if (deep) {
        /* Re-init clock after STOP */
        stmclk_init_sysclk();
    }
}

void pm_off(void)
{
    irq_disable();
    pm_set(0);
}
