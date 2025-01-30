/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @ingroup     drivers_periph_hwrng
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
#include "periph/hwrng.h"

/**
 * @name KEY needs to be written to the config register when en|disabling
 *       random number generation
 */
#define KEY             (0x524e4700)

void hwrng_init(void)
{
    /* no need for initialization */
}

void hwrng_read(void *buf, unsigned int num)
{
    unsigned count = 0;
    uint8_t *b = (uint8_t *)buf;

    /* enable clock signal for TRNG module */
    PMC->PMC_PCER1 = PMC_PCER1_PID41;
    /* enable the generation of random numbers */
    TRNG->TRNG_CR = (KEY | TRNG_CR_ENABLE);

    while (count < num) {
        /* wait until new value is generated -> takes up to 84 cycles */
        while (!(TRNG->TRNG_ISR & TRNG_ISR_DATRDY)) {}
        /* read 4 byte of random data */
        uint32_t tmp = TRNG->TRNG_ODATA;
        /* extract copy bytes to result */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }

    /* disable the generation of random numbers */
    TRNG->TRNG_CR = KEY;
    /* disable clock signal for TRNG module */
    PMC->PMC_PCDR1 = PMC_PCER1_PID41;
}
