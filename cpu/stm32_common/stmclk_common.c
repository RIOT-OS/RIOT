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

#include "periph_cpu.h"

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7)
#define REG_PWR_CR          CR1
#define BIT_CR_DBP          PWR_CR1_DBP
#else
#define REG_PWR_CR          CR
#define BIT_CR_DBP          PWR_CR_DBP
#endif

#if defined (CPU_FAM_STM32L4)
#define BIT_APB_PWREN       RCC_APB1ENR1_PWREN
#else
#define BIT_APB_PWREN       RCC_APB1ENR_PWREN
#endif

void stmclk_dbp_unlock(void)
{
    periph_clk_en(APB1, BIT_APB_PWREN);
    PWR->REG_PWR_CR |= BIT_CR_DBP;
}

void stmclk_dbp_lock(void)
{
    PWR->REG_PWR_CR &= ~(BIT_CR_DBP);
    periph_clk_dis(APB1, BIT_APB_PWREN);
}
