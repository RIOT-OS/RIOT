/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

void hwrng_init(void)
{
    /* nothing to do here */
}

void hwrng_read(void *buf, unsigned int num)
{
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    /* power on and enable the device */
    HWRNG_CLKEN();
    KINETIS_RNGA->CR = RNG_CR_INTM_MASK | RNG_CR_HA_MASK | RNG_CR_GO_MASK;

    /* self-seeding */
    while (!(KINETIS_RNGA->SR & RNG_SR_OREG_LVL_MASK));

    KINETIS_RNGA->ER = KINETIS_RNGA->OR ^ (uint32_t)buf;

    while (count < num) {
        /* wait for random data to be ready to read */
        while (!(KINETIS_RNGA->SR & RNG_SR_OREG_LVL_MASK));

        uint32_t tmp = KINETIS_RNGA->OR;

        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }

    /* power off the device */
    KINETIS_RNGA->CR = 0;
    HWRNG_CLKDIS();
}

#endif /* KINETIS_RNGA */
