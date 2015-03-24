/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_rnga
 *
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation.
 *
 * @author      Johann Fischer <j.fischer@phytec.de> (adaption for Freescale's RNGA)
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/random.h"
#include "periph_conf.h"

#if RANDOM_NUMOF
#ifdef KINETIS_RNGA

void random_init(void)
{
    random_poweron();
}

int random_read(char *buf, unsigned int num)
{
    int count = 0;

    /* self-seeding */
    while (!(KINETIS_RNGA->SR & RNG_SR_OREG_LVL_MASK));

    KINETIS_RNGA->ER = KINETIS_RNGA->OR ^ (uint32_t)buf;

    while (count < num) {
        /* wait for random data to be ready to read */
        while (!(KINETIS_RNGA->SR & RNG_SR_OREG_LVL_MASK));

        uint32_t tmp = KINETIS_RNGA->OR;

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
    RANDOM_CLKEN();
    KINETIS_RNGA->CR = RNG_CR_INTM_MASK | RNG_CR_HA_MASK | RNG_CR_GO_MASK;
}

void random_poweroff(void)
{
    KINETIS_RNGA->CR = 0;
    RANDOM_CLKDIS();
}

/*
void isr_rng(void)
{
}
*/

#endif /* KINETIS_RNGA */
#endif /* RANDOM_NUMOF */
