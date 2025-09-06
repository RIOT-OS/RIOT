/*
 * SPDX-FileCopyrightText: 2017 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include "cpu.h"

#include "periph_conf.h"
#include "periph/hwrng.h"

#include "em_cmu.h"

#include <string.h>

void hwrng_init(void)
{
    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_TRNG0, true);

    /* reset and initialize peripheral */
    TRNG0->CONTROL = TRNG_CONTROL_ENABLE |
                     TRNG_CONTROL_REPCOUNTIEN |
                     TRNG_CONTROL_APT64IEN |
                     TRNG_CONTROL_APT4096IEN |
                     TRNG_CONTROL_PREIEN |
                     TRNG_CONTROL_ALMIEN;

    /* wait for TRNG to become available */
    while (TRNG0->FIFOLEVEL == 0) {}
}

void hwrng_read(void *buf, unsigned int num)
{
    uint32_t *out_buf = (uint32_t *) buf;
    uint32_t tmp;

    /* read known good available data */
    while (num >= 4) {
        *out_buf++ = TRNG0->FIFO;
        num -= 4;
    }

    /* read remaining data (not multiple of 4) */
    if (num) {
        tmp = TRNG0->FIFO;
        memcpy((uint8_t *)out_buf, (const uint8_t *) &tmp, num);
    }
}
