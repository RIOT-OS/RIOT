/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf53
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "assert.h"
#include "cpu.h"
#include "kernel_init.h"
#include "nrf_clock.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"
#include "board.h"

/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize hf clock */
static_assert((CLOCK_CORECLOCK == MHZ(128)) || CLOCK_CORECLOCK == MHZ(64));
#if (CLOCK_CORECLOCK == MHZ(128))
    NRF_CLOCK_S->HFCLKCTRL = CLOCK_HFCLKCTRL_HCLK_Div1;
#else
    NRF_CLOCK_S->HFCLKCTRL = CLOCK_HFCLKCTRL_HCLK_Div2;
#endif
    /* Selects external 32MHz oscillator as clock source*/
#if (CLOCK_HFCLK == CLOCK_HFCLKSRC_SRC_HFINT)
    NRF_CLOCK_S->HFCLKSRC = CLOCK_HFCLKSRC_SRC_HFINT;
#else
    NRF_CLOCK_S->HFCLKSRC = CLOCK_HFCLKSRC_SRC_HFXO;
#endif
    NRF_CLOCK_S->TASKS_HFCLKSTART = 1;

    /* Enable instruction cache */
    NRF_CACHE_S->ENABLE = CACHE_ENABLE_ENABLE_Enabled;
    /* For now, disable the unused network core */
    NRF_RESET_S->NETWORK.FORCEOFF = 1;

    /* call cortexm default initialization */
    cortexm_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
