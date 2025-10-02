/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of common STM32 clock configuration functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32WL)
#define REG_PWR_CR          CR1
#define BIT_CR_DBP          PWR_CR1_DBP
#elif defined(CPU_FAM_STM32U5)
#define REG_PWR_CR          DBPR
#define BIT_CR_DBP          PWR_DBPR_DBP
#else
#define REG_PWR_CR          CR
#define BIT_CR_DBP          PWR_CR_DBP
#endif

#if defined (CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define REG_LSE             CSR
#define BIT_LSEON           RCC_CSR_LSEON
#define BIT_LSERDY          RCC_CSR_LSERDY
#elif defined(CPU_FAM_STM32C0)
#define REG_LSE             CSR1
#define BIT_LSEON           RCC_CSR1_LSEON
#define BIT_LSERDY          RCC_CSR1_LSERDY
#else
#define REG_LSE             BDCR
#define BIT_LSEON           RCC_BDCR_LSEON
#define BIT_LSERDY          RCC_BDCR_LSERDY
#endif

#if defined (CPU_FAM_STM32WB)
#define RCC_CFGR_SWS_HSI        RCC_CFGR_SWS_0
#define RCC_CSR_LSION           RCC_CSR_LSI1ON
#define RCC_CSR_LSIRDY          RCC_CSR_LSI1RDY
#endif

#if defined (CPU_FAM_STM32U5)
#define RCC_CSR_LSION           RCC_BDCR_LSION
#define RCC_CSR_LSIRDY          RCC_BDCR_LSIRDY
#endif

#if defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5)
#define RCC_CFGR_SWS_HSI        RCC_CFGR_SWS_0
#endif

void stmclk_enable_hsi(void)
{
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}
}

void stmclk_disable_hsi(void)
{
    RCC->CR &= ~RCC_CR_HSION;
}

void stmclk_enable_lfclk(void)
{
    if (IS_ACTIVE(CONFIG_BOARD_HAS_LSE)) {
        stmclk_dbp_unlock();
        RCC->REG_LSE |= BIT_LSEON;
        while (!(RCC->REG_LSE & BIT_LSERDY)) {}

    /* Set LSE system clock enable bit. This is required if LSE is to be used by
       USARTx, LPUARTx, LPTIMx, TIMx, RNG, system LSCO, MCO, MSI PLL mode */
#if defined(CPU_FAM_STM32WL) || defined (CPU_FAM_STM32L5) || \
    defined(CPU_FAM_STM32U5)
        RCC->BDCR |= RCC_BDCR_LSESYSEN;
        while (!(RCC->BDCR & RCC_BDCR_LSESYSRDY)) {}
#endif
        stmclk_dbp_lock();
    }
    else {
#if defined(CPU_FAM_STM32C0)
        RCC->CSR2 |= RCC_CSR2_LSION;
        while (!(RCC->CSR2 & RCC_CSR2_LSIRDY)) {}
#else
        RCC->CSR |= RCC_CSR_LSION;
        while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
#endif
    }
}

void stmclk_disable_lfclk(void)
{
    if (IS_ACTIVE(CONFIG_BOARD_HAS_LSE)) {
        stmclk_dbp_unlock();
        RCC->REG_LSE &= ~(BIT_LSEON);
        while (!(RCC->REG_LSE & BIT_LSERDY)) {}
        stmclk_dbp_lock();
    }
    else {
#if defined(CPU_FAM_STM32C0)
        RCC->CSR2 &= ~(RCC_CSR2_LSION);
#else
        RCC->CSR &= ~(RCC_CSR_LSION);
#endif
    }
}

void stmclk_dbp_unlock(void)
{
#if !defined(CPU_FAM_STM32C0) /* CPU_FAM_STM32C0 does not support DBP */
    PWR->REG_PWR_CR |= BIT_CR_DBP;
#endif
}

void stmclk_dbp_lock(void)
{
#if !defined(CPU_FAM_STM32C0) /* CPU_FAM_STM32C0 does not support DBP */
    if (!IS_ACTIVE(CPU_HAS_BACKUP_RAM)) {
/*  The DBP must be unlocked all the time, if we modify
    backup RAM content by comfortable BACKUP_RAM variables */
        PWR->REG_PWR_CR &= ~(BIT_CR_DBP);
    }
#endif
}

bool stmclk_dbp_is_locked(void)
{
#if !defined(CPU_FAM_STM32C0) /* CPU_FAM_STM32C0 does not support DBP */
    return !(PWR->REG_PWR_CR & BIT_CR_DBP);
#else
    return 0;
#endif
}
