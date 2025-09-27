/*
 * SPDX-FileCopyrightText: 2021 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf9160
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "nrf_clock.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

/**
 * @brief    LFCLK Clock selection configuration guard
*/
#if ((CLOCK_LFCLK != CLOCK_LFCLKSRC_SRC_LFRC) && \
     (CLOCK_LFCLK != CLOCK_LFCLKSRC_SRC_LFXO))
#error "LFCLK init: CLOCK_LFCLK has invalid value"
#endif

/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize hf clock */
    clock_init_hf();

#ifdef NVMC_ICACHECNF_CACHEEN_Msk
    /* enable instruction cache */
    NRF_NVMC_S->ICACHECNF = (NVMC_ICACHECNF_CACHEEN_Msk);
#endif

    /* call cortexm default initialization */
    cortexm_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
