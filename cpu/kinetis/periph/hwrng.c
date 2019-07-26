/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_hwrng
 *
 * @{
 *
 * @file
 * @brief       HWRNG interface implementation
 *
 * @author      Johann Fischer <j.fischer@phytec.de> (adaption for Freescale's RNGA)
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/hwrng.h"
#include "periph_conf.h"
#include "bit.h"

#ifdef KINETIS_RNGA

void hwrng_poweron(void)
{
    /* power on and enable the device */
    HWRNG_CLKEN();
    KINETIS_RNGA->CR = RNG_CR_INTM_MASK | RNG_CR_HA_MASK | RNG_CR_GO_MASK;

    /* self-seeding */
    while (!(KINETIS_RNGA->SR & RNG_SR_OREG_LVL_MASK));

    /* seed the HWRNG  */
    KINETIS_RNGA->ER = KINETIS_RNGA->OR ^ (uint32_t) &hwrng_poweron;
}

void hwrng_poweroff(void)
{
    /* power off the device */
    KINETIS_RNGA->CR = 0;
    HWRNG_CLKDIS();
}

uint32_t hwrng_uint32(void)
{
    /* wait for random data to be ready to read */
    while (!(KINETIS_RNGA->SR & RNG_SR_OREG_LVL_MASK));
    return KINETIS_RNGA->OR;
}

#endif /* KINETIS_RNGA */
