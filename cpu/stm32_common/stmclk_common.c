/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
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

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7)
#define REG_PWR_CR          CR1
#define BIT_CR_DBP          PWR_CR1_DBP
#else
#define REG_PWR_CR          CR
#define BIT_CR_DBP          PWR_CR_DBP
#endif

#if defined (CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define REG_LSE             CSR
#define BIT_LSEON           RCC_CSR_LSEON
#define BIT_LSERDY          RCC_CSR_LSERDY
#else
#define REG_LSE             BDCR
#define BIT_LSEON           RCC_BDCR_LSEON
#define BIT_LSERDY          RCC_BDCR_LSERDY
#endif

#ifndef CLOCK_HSE
#define CLOCK_HSE   (0U)
#endif
#ifndef CLOCK_LSE
#define CLOCK_LSE   (0U)
#endif

void stmclk_enable_hsi(void)
{
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}
}

void stmclk_disable_hsi(void)
{
    /* we only disable the HSI clock if not used as input for the PLL and if
     * not used directly as system clock */
    if (CLOCK_HSE) {
        if ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
            RCC->CR &= ~(RCC_CR_HSION);
        }
    }
}

void stmclk_enable_lfclk(void)
{
    if (CLOCK_LSE) {
        stmclk_dbp_unlock();
        RCC->REG_LSE |= BIT_LSEON;
        while (!(RCC->REG_LSE & BIT_LSERDY)) {}
        stmclk_dbp_lock();
    }
    else {
        RCC->CSR |= RCC_CSR_LSION;
        while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
    }
}

void stmclk_disable_lfclk(void)
{
    if (CLOCK_LSE) {
        stmclk_dbp_unlock();
        RCC->REG_LSE &= ~(BIT_LSEON);
        while (!(RCC->REG_LSE & BIT_LSERDY)) {}
        stmclk_dbp_lock();
    }
    else {
        RCC->CSR &= ~(RCC_CSR_LSION);
    }
}

void stmclk_dbp_unlock(void)
{
    PWR->REG_PWR_CR |= BIT_CR_DBP;
}

void stmclk_dbp_lock(void)
{
    PWR->REG_PWR_CR &= ~(BIT_CR_DBP);
}
