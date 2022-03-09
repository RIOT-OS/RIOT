/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
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
#include "stdio_base.h"

#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"

/**
 * @brief   Default settings for CMU initialization.
 */
#ifndef CMU_HFXOINIT
#define CMU_HFXOINIT        CMU_HFXOINIT_DEFAULT
#endif
#ifndef CMU_LFXOINIT
#define CMU_LFXOINIT        CMU_LFXOINIT_DEFAULT
#endif

/**
 * @brief   Default settings for EMU initialization
 */
#ifndef EMU_DCDCINIT
#define EMU_DCDCINIT        EMU_DCDCINIT_DEFAULT
#endif
#ifndef EMU_EM23INIT
#define EMU_EM23INIT        EMU_EM23INIT_DEFAULT
#endif
#ifndef EMU_EM4INIT
#define EMU_EM4INIT         EMU_EM4INIT_DEFAULT
#endif

#ifndef RIOTBOOT

#if defined(DCDC_PRESENT) && DCDC_COUNT > 0
/**
 * @brief   Initialize integrated DC-DC regulator
 */
static void dcdc_init(void)
{
#ifdef EMU_DCDCINIT_OFF
    EMU_DCDCPowerOff();
#else
    EMU_DCDCInit_TypeDef init_dcdc = EMU_DCDCINIT;

    EMU_DCDCInit(&init_dcdc);
#endif
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
 *
 * When selecting the HFXO, the HFRCO is disabled. The same applies for the
 * LFA, LFB and LFE branch, in case the LFXO is selected.
 */
static void clk_init(void)
{
    /* initialize HFXO with board-specific parameters before switching */
    if (CLOCK_HF == cmuSelect_HFXO) {
        CMU_HFXOInit_TypeDef init_hfxo = CMU_HFXOINIT;

        CMU_HFXOInit(&init_hfxo);
    }

    /* set (and enable) the HF clock source */
    CMU_ClockSelectSet(cmuClock_HF, CLOCK_HF);
    CMU_ClockDivSet(cmuClock_CORE, CLOCK_CORE_DIV);

    /* disable the HFRCO if external crystal is used */
    if (CLOCK_HF == cmuSelect_HFXO) {
        CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
    }

#if defined(_SILICON_LABS_32B_SERIES_1)
    /* disable LFRCO comparator chopping and dynamic element matching
     * else LFRCO has too much jitter for LEUART > 1800 baud */
    CMU->LFRCOCTRL &= ~(CMU_LFRCOCTRL_ENCHOP | CMU_LFRCOCTRL_ENDEM);
#endif

    /* initialize LFXO with board-specific parameters before switching */
    if (CLOCK_LFA == cmuSelect_LFXO || CLOCK_LFB == cmuSelect_LFXO ||
#if defined(_SILICON_LABS_32B_SERIES_1)
        CLOCK_LFE == cmuSelect_LFXO)
#else
        false)
#endif
    {
        CMU_LFXOInit_TypeDef init_lfxo = CMU_LFXOINIT;

        CMU_LFXOInit(&init_lfxo);
    }

    /* set (and enable) the LFA clock source */
    CMU_ClockSelectSet(cmuClock_LFA, CLOCK_LFA);

    /* set (and enable) the LFB clock source */
    CMU_ClockSelectSet(cmuClock_LFB, CLOCK_LFB);

#if defined(_SILICON_LABS_32B_SERIES_1)
    /* set (and enable) the LFE clock source */
    CMU_ClockSelectSet(cmuClock_LFE, CLOCK_LFE);
#endif

    /* disable the LFRCO if external crystal is used */
    if (CLOCK_LFA == cmuSelect_LFXO && CLOCK_LFB == cmuSelect_LFXO &&
#if defined(_SILICON_LABS_32B_SERIES_1)
        CLOCK_LFE == cmuSelect_LFXO)
#else
        true)
#endif
    {
        CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
    }
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
    EMU_EM23Init_TypeDef init_em23 = EMU_EM23INIT;

    EMU_EM23Init(&init_em23);

#if defined(_SILICON_LABS_32B_SERIES_1)
    /* initialize EM4 */
    EMU_EM4Init_TypeDef init_em4 = EMU_EM4INIT;

    EMU_EM4Init(&init_em4);
#endif
}

#endif

void cpu_init(void)
{
#ifndef RIOTBOOT
    /* apply errata that may be applicable (see em_chip.h) */
    CHIP_Init();
#endif

    /* initialize the Cortex-M core */
    cortexm_init();

#ifndef RIOTBOOT

#if defined(DCDC_PRESENT) && DCDC_COUNT > 0
    /* initialize dc-dc */
    dcdc_init();
#endif

    /* initialize clock sources and generic clocks */
    clk_init();

    /* initialize power management interface */
    pm_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
#endif
}
