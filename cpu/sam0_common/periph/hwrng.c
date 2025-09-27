/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/hwrng.h"

void hwrng_init(void)
{
    /* Enable the MCLK */
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_TRNG;

    /* Enable the TRNG */
    TRNG->CTRLA.reg |= TRNG_CTRLA_ENABLE;

}

uint32_t hwrand(void)
{
    while (!(TRNG->INTFLAG.reg & TRNG_INTFLAG_DATARDY)) {}
    return TRNG->DATA.reg;
}

void hwrng_read(void *buf, unsigned int num)
{
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    while (count < num) {
        /* read next 4 bytes of random data */
        uint32_t tmp = hwrand();

        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }
}
