/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf54l
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "periph_conf.h"
#include "stdio_base.h"

static_assert((CLOCK_CORECLOCK == MHZ(128)) || (CLOCK_CORECLOCK == MHZ(64)),
              "the nRF54L CPU only runs at 64 MHz or 128 MHz");

/**
 * @brief   Initialize the CPU
 *
 * The CPU core runs at 64 MHz out of reset with the instruction cache
 * disabled: set the core clock to CLOCK_CORECLOCK and enable the cache. The
 * basic peripherals (GPIO, UARTE2x and TIMER2x) are clocked from the 16 MHz
 * peripheral clock and need no clock initialization.
 */
void cpu_init(void)
{
    uint32_t freq = (CLOCK_CORECLOCK == MHZ(128))
                  ? OSCILLATORS_PLL_FREQ_FREQ_CK128M
                  : OSCILLATORS_PLL_FREQ_FREQ_CK64M;
    NRF_OSCILLATORS->PLL.FREQ = freq;
    /* the CURRENTFREQ values match the FREQ values */
    while (NRF_OSCILLATORS->PLL.CURRENTFREQ != freq) {}

    /* enable the instruction cache, RRAM fetches are slow without it */
    NRF_ICACHE->ENABLE = CACHE_ENABLE_ENABLE_Enabled;

    /* call cortexm default initialization */
    cortexm_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
