/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

uint32_t hwrng_uint32(void)
{
    return TRNG0->FIFO;
}
