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
 * @ingroup     cpu_stm32
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
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#include "stmclk.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef PM_STOP_CONFIG
/**
 * @brief Define config flags for stop mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F1)
#define PM_STOP_CONFIG  (PWR_CR_LPDS)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
/* Enable ultra low-power and clear wakeup flags */
#define PM_STOP_CONFIG  (PWR_CR_LPSDSR | PWR_CR_ULP | PWR_CR_CWUF)
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32G4) || \
      defined(CPU_FAM_STM32L5)
#define PM_STOP_CONFIG  (PWR_CR1_LPMS_STOP1)
#elif defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G0)
#define PM_STOP_CONFIG  (PWR_CR1_LPMS_0)
#elif defined(CPU_FAM_STM32F7)
#define PM_STOP_CONFIG  (PWR_CR1_LPDS | PWR_CR1_FPDS | PWR_CR1_LPUDS)
#elif defined(CPU_FAM_STM32WB)
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
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32G4) || \
      defined(CPU_FAM_STM32L5)
#define PM_STANDBY_CONFIG   (PWR_CR1_LPMS_STANDBY)
#elif defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G0)
#define PM_STANDBY_CONFIG   (PWR_CR1_LPMS_0 | PWR_CR1_LPMS_1)
#elif defined(CPU_FAM_STM32F7)
#define PM_STANDBY_CONFIG   (PWR_CR1_PDDS | PWR_CR1_CSBF)
#else
#define PM_STANDBY_CONFIG   (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF)
#endif
#endif

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
#define PWR_CR_REG     PWR->CR1
#define PWR_WUP_REG    PWR->CR3
/* Allow overridable SRAM2 retention mode using CFLAGS */
#ifndef STM32L4_SRAM2_RETENTION
/* Disable SRAM2 retention by default for maximum power saving */
#define STM32L4_SRAM2_RETENTION  (0)
#endif
#elif defined(CPU_FAM_STM32F7)
#define PWR_CR_REG     PWR->CR1
#define PWR_WUP_REG    PWR->CSR2
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
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32L5)
#if STM32L4_SRAM2_RETENTION
            PWR->CR3 |= PWR_CR3_RRS;
#else
            PWR->CR3 &= ~PWR_CR3_RRS;
#endif
#endif
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5)
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
