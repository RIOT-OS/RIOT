/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "nrf_clock.h"
#include "nrfx_riot.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

/**
 * @brief    LFCLK Clock selection configuration guard
*/
#if ((CLOCK_LFCLK != CLOCK_LFCLKSRC_SRC_RC)   && \
     (CLOCK_LFCLK != CLOCK_LFCLKSRC_SRC_Xtal) && \
     (CLOCK_LFCLK != CLOCK_LFCLKSRC_SRC_Synth))
#error "LFCLK init: CLOCK_LFCLK has invalid value"
#endif

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* Enable the DC/DC power converter */
    nrfx_dcdc_init();
    /* setup the HF clock */
    clock_init_hf();
    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();
    /* trigger static peripheral initialization */
    periph_init();
}
