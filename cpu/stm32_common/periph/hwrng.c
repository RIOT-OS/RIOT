/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/hwrng.h"

/* only build if the CPU actually provides a RNG peripheral */
#ifdef RNG

void hwrng_poweron(void)
{
    /* power on and enable the device */
#if defined(CPU_LINE_STM32F410Rx)
    periph_clk_en(AHB1, RCC_AHB1ENR_RNGEN);
#elif defined(CPU_FAM_STM32L0)
    periph_clk_en(AHB, RCC_AHBENR_RNGEN);
#else
    periph_clk_en(AHB2, RCC_AHB2ENR_RNGEN);
#endif
    RNG->CR = RNG_CR_RNGEN;
}

void hwrng_poweroff(void)
{
    /* finally disable the device again */
    RNG->CR = 0;
#if defined(CPU_LINE_STM32F410Rx)
    periph_clk_dis(AHB1, RCC_AHB1ENR_RNGEN);
#elif defined(CPU_FAM_STM32L0)
    periph_clk_dis(AHB, RCC_AHBENR_RNGEN);
#else
    periph_clk_dis(AHB2, RCC_AHB2ENR_RNGEN);
#endif
}

uint32_t hwrng_uint32(void)
{
    /* wait for random data to be ready to read */
    while (!(RNG->SR & RNG_SR_DRDY)) {}
    return RNG->DR;
}

#endif /* RNG */
