/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_rngb
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se> (adaption for Freescale's RNGB)
 * @author      Johann Fischer <j.fischer@phytec.de> (adaption for Freescale's RNGA)
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/random.h"
#include "periph_conf.h"

#if RANDOM_NUMOF
#ifdef KINETIS_RNGB


void random_init(void)
{
    random_poweron();
}

int random_read(char *buf, unsigned int num)
{
    unsigned int count = 0;

    while (count < num) {
        uint32_t tmp;

        /* wait for random data to be ready to read */
        while (!(KINETIS_RNGB->SR & RNG_SR_FIFO_LVL_MASK));

        tmp = KINETIS_RNGB->OUT;

        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            buf[count++] = (char)tmp;
            tmp = tmp >> 8;
        }
    }

    return (int)count;
}

void random_poweron(void)
{
    RANDOM_CLKEN();

    if ((KINETIS_RNGB->VER & RNG_VER_TYPE_MASK) != 0b0001) {
        /* Wrong type of RNG */
        /* TODO: Handle */
    }

    /* Software reset, bit is self-clearing */
    BITBAND_REG32(KINETIS_RNGB->CMD, RNG_CMD_SR_SHIFT) = 1;
    /* Set up automatic reseed */
    KINETIS_RNGB->CR = RNG_CR_AR_MASK | RNG_CR_MASKERR_MASK | RNG_CR_MASKDONE_MASK;
}

void random_poweroff(void)
{
    KINETIS_RNGB->CR = 0;
    RANDOM_CLKDIS();
}

/*
void isr_rng(void)
{
}
*/

#endif /* KINETIS_RNGB */
#endif /* RANDOM_NUMOF */
