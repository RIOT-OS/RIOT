/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    MCLK->APBCMASK.bit.TRNG_ = 1;

    /* Enable the TRNG */
    TRNG->CTRLA.bit.ENABLE = 1;
}

uint32_t hwrng_uint32(void)
{
    while (!TRNG->INTFLAG.bit.DATARDY) {}
    return TRNG->DATA.reg;
}
