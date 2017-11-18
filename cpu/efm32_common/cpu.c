/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
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
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"

#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"

#ifdef _SILICON_LABS_32B_SERIES_1
/**
 * @brief   Initialize integrated DC-DC regulator
 */
static void dcdc_init(void)
{
    EMU_DCDCInit_TypeDef init_dcdc = EMU_DCDCINIT_DEFAULT;

    EMU_DCDCInit(&init_dcdc);
}
#endif

/**
 * @brief   Configure clock sources and the CPU frequency
 *
 * On a EFM32 CPU, there are two options for selecting the main clock
 * source, using an external clock source (HFXO), or using the internal RC
 * oscillator (HFRCO, enabled by default).
 *
 * The clocks for the LFA, LFB, LFE and HFPER are also configured.
 */
static void clk_init(void)
{
    CMU_HFXOInit_TypeDef init_hfxo = CMU_HFXOINIT_DEFAULT;

    /* initialize HFXO with board-specific parameters before switching */
    if (CLOCK_HF == cmuSelect_HFXO) {
        CMU_HFXOInit(&init_hfxo);
    }

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
#ifdef _SILICON_LABS_32B_SERIES_1
    CMU_ClockSelectSet(cmuClock_LFE, CLOCK_LFE);
#endif
}

/**
 * @brief   Initialize sleep modes
 *
 * The EFM32 has several energy saving modes (EM0 - EM4), of which deeper
 * modes save more energy.s
 */
static void pm_init(void)
{
    /* initialize EM2 and EM3 */
    EMU_EM23Init_TypeDef init_em23 = EMU_EM23INIT_DEFAULT;

    EMU_EM23Init(&init_em23);

#ifdef _SILICON_LABS_32B_SERIES_1
    /* initialize EM4 */
    EMU_EM4Init_TypeDef init_em4 = EMU_EM4INIT_DEFAULT;

    EMU_EM4Init(&init_em4);
#endif
}

void cpu_init(void)
{
    /* apply errata that may be applicable (see em_chip.h) */
    CHIP_Init();

    /* initialize the Cortex-M core */
    cortexm_init();

#ifdef _SILICON_LABS_32B_SERIES_1
    /* initialize dc-dc */
    dcdc_init();
#endif

    /* initialize clock sources and generic clocks */
    clk_init();

    /* initialize power management interface */
    pm_init();

    /* trigger static peripheral initialization */
    periph_init();
}
