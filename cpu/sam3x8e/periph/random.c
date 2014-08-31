/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/random.h"

/* only compile this driver if enabled in the board's periph_conf.h */
#if RANDOM_NUMOF

/**
 * @name KEY needs to be written to the config register when en|disabling
 *       random number generation
 */
#define KEY             (0x524e4700)

void random_init(void)
{
    random_poweron();
}

int random_read(char *buf, unsigned int num)
{
    uint32_t tmp;
    int count = 0;

    while (count < num) {
        /* wait until new value is generated -> takes up to 84 cycles */
        while (!(TRNG->TRNG_ISR & TRNG_ISR_DATRDY));
        /* read 4 byte of random data */
        tmp = TRNG->TRNG_ODATA;
        /* extract copy bytes to result */
        for (int i = 0; i < 4 && count < num; i++) {
            buf[count++] = (char)tmp;
            tmp = tmp >> 8;
        }
    }

    return count;
}

void random_poweron(void)
{
    /* enable clock signal for TRNG module */
    PMC->PMC_PCER1 |= PMC_PCER1_PID41;
    /* enable the generation of random numbers */
    TRNG->TRNG_CR |= (KEY | TRNG_CR_ENABLE);
}

void random_poweroff(void)
{
    /* disable the generation of random numbers */
    TRNG->TRNG_CR &= ~(KEY | TRNG_CR_ENABLE);
    /* disable clock signal for TRNG module */
    PMC->PMC_PCER1 &= ~(PMC_PCER1_PID41);
}

#endif /* RANDOM_NUMOF */
