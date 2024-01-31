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
#include "periph/cpu_pm.h"
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
#elif defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G0) || \
      defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32C0)
#define PM_STOP_CONFIG  (PWR_CR1_LPMS_0)
#elif defined(CPU_FAM_STM32F7)
#define PM_STOP_CONFIG  (PWR_CR1_LPDS | PWR_CR1_FPDS | PWR_CR1_LPUDS)
#elif defined(CPU_FAM_STM32MP1)
#define PM_STOP_CONFIG  (0)
#elif defined(CPU_FAM_STM32U5)
#define PM_STOP_CONFIG  (0)
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
#elif defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G0) || \
      defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32C0)
#define PM_STANDBY_CONFIG   (PWR_CR1_LPMS_0 | PWR_CR1_LPMS_1)
#elif defined(CPU_FAM_STM32F7)
#define PM_STANDBY_CONFIG   (PWR_CR1_PDDS | PWR_CR1_CSBF)
#elif defined(CPU_FAM_STM32MP1)
#define PM_STANDBY_CONFIG   (0)
#elif defined(CPU_FAM_STM32U5)
#define PM_STANDBY_CONFIG   (0)
#else
#define PM_STANDBY_CONFIG   (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF)
#endif
#endif

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32C0)
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
#elif defined(CPU_FAM_STM32MP1)
#define PWR_CR_REG     PWR->CR1
#define PWR_WUP_REG    PWR->MCUWKUPENR
#else
#define PWR_CR_REG     PWR->CR
#define PWR_WUP_REG    PWR->CSR
#endif

void pm_set(unsigned mode)
{
    int deep;

    switch (mode) {
#if !defined(CPU_FAM_STM32MP1)
        case STM32_PM_STANDBY:
            PWR_CR_REG &= ~(PM_STOP_CONFIG | PM_STANDBY_CONFIG);
            PWR_CR_REG |= PM_STANDBY_CONFIG;
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32WL)
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
            pm_backup_regulator_on();
            break;
#endif
        case STM32_PM_STOP:
            PWR_CR_REG &= ~(PM_STOP_CONFIG | PM_STANDBY_CONFIG);
            PWR_CR_REG |= PM_STOP_CONFIG;
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
        default:
            deep = 0;
            break;
    }

    cortexm_sleep(deep);

    if (deep) {
        /* Re-init clock after STOP */
#if !defined(CPU_FAM_STM32MP1) || IS_USED(MODULE_STM32MP1_ENG_MODE)
        stmclk_init_sysclk();
#endif
    }
}

/**
 * @name   Registers and related configuration bits to retain
 *         the backup domain registers, using the backup regulator
 * @{
 */
#if defined(PWR_CSR1_BRE)
#define PWR_BACKUP_REGULATOR_REG    PWR->CSR1
#define BKPREG_CONFIG               (PWR_CSR1_BRE | PWR_CSR1_EIWUP)
#define BKPREG_READY                (PWR_CSR1_BRR)
#elif defined(PWR_CSR_BRE)
#define PWR_BACKUP_REGULATOR_REG    PWR->CSR
#define BKPREG_CONFIG               (PWR_CSR_BRE)
#define BKPREG_READY                (PWR_CSR_BRR)
#elif defined(PWR_CR2_BREN)
#define PWR_BACKUP_REGULATOR_REG    PWR->CR2
#define BKPREG_CONFIG               (PWR_CR2_BREN)
#define BKPREG_READY                (PWR_CR2_BRRDY)
#endif
/** @} */

bool pm_backup_regulator_is_on(void)
{
#if defined(PWR_BACKUP_REGULATOR_REG)
    return (PWR_BACKUP_REGULATOR_REG & BKPREG_READY) == BKPREG_READY;
#else
    return false;
#endif
}

void pm_backup_regulator_on(void)
{
#if defined(PWR_BACKUP_REGULATOR_REG)
    bool locked = stmclk_dbp_is_locked();
    if (locked) {
        stmclk_dbp_unlock();
    }
    PWR_BACKUP_REGULATOR_REG |= BKPREG_CONFIG;
    while (!(PWR_BACKUP_REGULATOR_REG & BKPREG_READY)){}
    if (locked) {
        stmclk_dbp_lock();
    }
#endif
}

void pm_backup_regulator_off(void)
{
#if defined(PWR_BACKUP_REGULATOR_REG)
    bool locked = stmclk_dbp_is_locked();
    if (locked) {
        stmclk_dbp_unlock();
    }
    PWR_BACKUP_REGULATOR_REG &= ~BKPREG_CONFIG;
    if (locked) {
        stmclk_dbp_lock();
    }
#endif
}
