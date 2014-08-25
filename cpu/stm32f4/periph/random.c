/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/random.h"
#include "periph_conf.h"

/* ignore file in case no RNG device is defined */
#if RANDOM_NUMOF

void random_init(void)
{
    random_poweron();
}

int random_read(char *buf, unsigned int num)
{
    uint32_t tmp;
    int count = 0;

    while (count < num) {
        /* wait for random data to be ready to read */
        while (!(RNG->SR & RNG_SR_DRDY));
        /* read next 4 bytes */
        tmp = RNG->DR;
        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            buf[count++] = (char)tmp;
            tmp = tmp >> 8;
        }
    }

    return count;
}

void random_poweron(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
    RNG->CR = RNG_CR_RNGEN;
}

void random_poweroff(void)
{
    RNG->CR = 0;
    RCC->AHB2ENR &= ~RCC_AHB2ENR_RNGEN;
}

#endif /* RANDOM_NUMOF */
