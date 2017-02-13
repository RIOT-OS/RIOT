/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common_rngb
 * @{
 *
 * @file
 * @brief       HWRNG interface implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se> (adaption for Freescale's RNGB)
 * @author      Johann Fischer <j.fischer@phytec.de> (adaption for Freescale's RNGA)
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/hwrng.h"
#include "periph_conf.h"

#ifdef KINETIS_RNGB

void hwrng_init(void)
{
    /* nothing to be done here */
}

void hwrng_read(void *buf, unsigned int num)
{
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    HWRNG_CLKEN();

    if ((KINETIS_RNGB->VER & RNG_VER_TYPE_MASK) != 0b0001) {
        /* Wrong type of RNG */
        /* TODO: Handle */
    }

    /* Software reset, bit is self-clearing */
    BITBAND_REG32(KINETIS_RNGB->CMD, RNG_CMD_SR_SHIFT) = 1;
    /* Set up automatic reseed */
    KINETIS_RNGB->CR = RNG_CR_AR_MASK | RNG_CR_MASKERR_MASK | RNG_CR_MASKDONE_MASK;

    while (count < num) {
        uint32_t tmp;

        /* wait for random data to be ready to read */
        while (!(KINETIS_RNGB->SR & RNG_SR_FIFO_LVL_MASK));

        tmp = KINETIS_RNGB->OUT;

        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }

    KINETIS_RNGB->CR = 0;
    HWRNG_CLKDIS();
}

#endif /* KINETIS_RNGB */
