/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 *
 * @}
 */

#include "cpu.h"
#include "periph/hwrng.h"
#include "periph_conf.h"

/* ignore file in case no RNG device is defined */
#ifdef RNG

void hwrng_init(void)
{
    /* enable RNG reset state */
    periph_clk_en(AHB2, RCC_AHB2ENR_RNGEN);
    /* release RNG from reset state */
    periph_clk_dis(AHB2, RCC_AHB2ENR_RNGEN);
}


void hwrng_read(void *buf, unsigned int num)
{
    /* cppcheck-suppress variableScope */
    uint32_t tmp;
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    /* enable RNG reset state */
    periph_clk_en(AHB2, RCC_AHB2ENR_RNGEN);
    /* enable the RNG */
    RNG->CR |= RNG_CR_RNGEN;

    while (count < num) {
        /* wait for random data to be ready to read */
        while (!(RNG->SR & RNG_SR_DRDY));
        /* read next 4 bytes */
        tmp = RNG->DR;
        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }

    /* disable the RNG */
    RNG->CR &= ~RNG_CR_RNGEN;
    /* release RNG from reset state */
    periph_clk_dis(AHB2, RCC_AHB2ENR_RNGEN);
}

#endif /* RANDOM_NUMOF */
