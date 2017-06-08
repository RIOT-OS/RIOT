/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"

#include "em_chip.h"
#include "em_cmu.h"

/**
 * @brief   Configure clock sources and the CPU frequency
 *
 * On a EFM32 CPU, we basically have two options for selecting the main clock
 * source, using an external clock source (HFXO), or using the internal RC
 * oscillator (HFRCO, enabled by default).
 *
 * The clocks for the LFA, LFB, LFE and HFPER are also configured. They are
 * also disabled until needed by peripherals, to minimize power consumption.
 */
static void clk_init(void)
{
    /* set the HF clock source */
    CMU_ClockSelectSet(cmuClock_HF, CLOCK_HF);
    CMU_ClockDivSet(cmuClock_CORE, CLOCK_CORE_DIV);

    /* disable the HFRCO if external crystal is used */
    if (CLOCK_HF == cmuSelect_HFXO) {
        CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
    }

    /* set the LFA clock source */
    CMU_ClockSelectSet(cmuClock_LFA, CLOCK_LFA);

    /* set the LFB clock source */
    CMU_ClockSelectSet(cmuClock_LFB, CLOCK_LFB);

    /* set the LFE clock source */
#ifdef _SILICON_LABS_32B_PLATFORM_2
    CMU_ClockSelectSet(cmuClock_LFE, CLOCK_LFE);
#endif
}

void cpu_init(void)
{
    /* apply errata that may be applicable (see em_chip.h) */
    CHIP_Init();
    /* initialize the Cortex-M core */
    cortexm_init();
    /* Initialise clock sources and generic clocks */
    clk_init();
}
