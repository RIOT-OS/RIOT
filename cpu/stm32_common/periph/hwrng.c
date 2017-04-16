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

void hwrng_init(void)
{
    /* no need for initialization */
}

void hwrng_read(void *buf, unsigned int num)
{
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    /* power on and enable the device */
#if defined(CPU_MODEL_STM32F410RB)
    periph_clk_en(AHB1, RCC_AHB1ENR_RNGEN);
#elif defined(CPU_FAM_STM32L0)
    periph_clk_en(AHB, RCC_AHBENR_RNGEN);
#else
    periph_clk_en(AHB2, RCC_AHB2ENR_RNGEN);
#endif
    RNG->CR = RNG_CR_RNGEN;

    /* get random data */
    while (count < num) {
        /* wait for random data to be ready to read */
        while (!(RNG->SR & RNG_SR_DRDY)) {}
        /* read next 4 bytes */
        uint32_t tmp = RNG->DR;
        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }

    /* finally disable the device again */
    RNG->CR = 0;
#if defined(CPU_MODEL_STM32F410RB)
    periph_clk_dis(AHB1, RCC_AHB1ENR_RNGEN);
#elif defined(CPU_FAM_STM32L0)
    periph_clk_dis(AHB, RCC_AHBENR_RNGEN);
#else
    periph_clk_dis(AHB2, RCC_AHB2ENR_RNGEN);
#endif
}

#endif /* RNG */
