/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_mcg
 * @{
 *
 * @file
 * @brief       Implementation of the Kinetis Multipurpose Clock Generator
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @}
 */

#include <stdint.h>
#include "mcg.h"
#include "periph_conf.h"

#if KINETIS_CPU_USE_MCG

/* MCG neighbor modes matrix */
static uint8_t mcg_pm[8] = {0x02, 0x15, 0x12, 0x20, 0xe6, 0xd8, 0xb0, 0xf0};

static uint8_t current_mode = KINETIS_MCG_FEI;

#define KINETIS_MCG_FLL_FACTOR_640        0
#define KINETIS_MCG_FLL_FACTOR_732        (MCG_C4_DRST_DRS(0) | MCG_C4_DMX32_MASK)
#define KINETIS_MCG_FLL_FACTOR_1280       (MCG_C4_DRST_DRS(1))
#define KINETIS_MCG_FLL_FACTOR_1464       (MCG_C4_DRST_DRS(1) | MCG_C4_DMX32_MASK)
#define KINETIS_MCG_FLL_FACTOR_1920       (MCG_C4_DRST_DRS(2))
#define KINETIS_MCG_FLL_FACTOR_2197       (MCG_C4_DRST_DRS(2) | MCG_C4_DMX32_MASK)
#define KINETIS_MCG_FLL_FACTOR_2560       (MCG_C4_DRST_DRS(3))
#define KINETIS_MCG_FLL_FACTOR_2929       (MCG_C4_DRST_DRS(3) | MCG_C4_DMX32_MASK)

#ifndef KINETIS_MCG_DCO_RANGE
#define KINETIS_MCG_DCO_RANGE             (48000000U)
#endif

/* Default DCO_RANGE should be defined in periph_conf.h */
#if (KINETIS_MCG_DCO_RANGE == 24000000U)
#define KINETIS_MCG_FLL_FACTOR_FEI        KINETIS_MCG_FLL_FACTOR_640
#define KINETIS_MCG_FLL_FACTOR_FEE        KINETIS_MCG_FLL_FACTOR_732
#elif (KINETIS_MCG_DCO_RANGE == 48000000U)
#define KINETIS_MCG_FLL_FACTOR_FEI        KINETIS_MCG_FLL_FACTOR_1280
#define KINETIS_MCG_FLL_FACTOR_FEE        KINETIS_MCG_FLL_FACTOR_1464
#elif (KINETIS_MCG_DCO_RANGE == 72000000U)
#define KINETIS_MCG_FLL_FACTOR_FEI        KINETIS_MCG_FLL_FACTOR_1920
#define KINETIS_MCG_FLL_FACTOR_FEE        KINETIS_MCG_FLL_FACTOR_2197
#elif (KINETIS_MCG_DCO_RANGE == 96000000U)
#define KINETIS_MCG_FLL_FACTOR_FEI        KINETIS_MCG_FLL_FACTOR_2560
#define KINETIS_MCG_FLL_FACTOR_FEE        KINETIS_MCG_FLL_FACTOR_2929
#else
#error "KINETIS_MCG_DCO_RANGE is wrong"
#endif

#ifndef KINETIS_MCG_USE_FAST_IRC
#define KINETIS_MCG_USE_FAST_IRC          0
#endif

#if (KINETIS_MCG_USE_ERC == 0)
#define KINETIS_MCG_USE_ERC               0
#define KINETIS_MCG_USE_PLL               0
#define KINETIS_MCG_ERC_FREQ              0
#define KINETIS_MCG_ERC_FRDIV             0
#define KINETIS_MCG_ERC_RANGE             0
#define KINETIS_MCG_ERC_OSCILLATOR        0
#endif

#if (KINETIS_MCG_USE_PLL == 0)
#define KINETIS_MCG_PLL_PRDIV             0
#define KINETIS_MCG_PLL_VDIV0             0
#define KINETIS_MCG_PLL_FREQ              0
#endif

#ifndef KINETIS_MCG_OSC_CLC
#define KINETIS_MCG_OSC_CLC               0
#endif

#ifndef KINETIS_MCG_ERC_OSCILLATOR
#error "KINETIS_MCG_ERC_OSCILLATOR not defined in periph_conf.h"
#endif

#ifndef KINETIS_MCG_ERC_FREQ
#error "KINETIS_MCG_ERC_FREQ not defined in periph_conf.h"
#endif

#ifndef KINETIS_MCG_ERC_FRDIV
#error "KINETIS_MCG_ERC_FRDIV not defined in periph_conf.h"
#endif

#ifndef KINETIS_MCG_ERC_RANGE
#error "KINETIS_MCG_ERC_RANGE not defined in periph_conf.h"
#endif

#ifndef KINETIS_MCG_PLL_PRDIV
#error "KINETIS_MCG_PLL_PRDIV not defined in periph_conf.h"
#endif

#ifndef KINETIS_MCG_PLL_VDIV0
#error "KINETIS_MCG_PLL_VDIV0 not defined in periph_conf.h"
#endif

#ifndef KINETIS_MCG_PLL_FREQ
#error "KINETIS_MCG_PLL_FREQ not defined in periph_conf.h"
#endif

/**
 * @brief Disable Phase Locked Loop (PLL)
 *
 */
static inline void kinetis_mcg_disable_pll(void)
{
    MCG->C5 = (uint8_t)0;
    MCG->C6 = (uint8_t)0;
}

/**
 * @brief Set Frequency Locked Loop (FLL) factor.
 *
 * The FLL will lock the DCO frequency to the FLL factor.
 * FLL factor will be selected by DRST_DRS and DMX32 bits
 * and depends on KINETIS_MCG_DCO_RANGE value.
 */
static inline void kinetis_mcg_set_fll_factor(uint8_t factor)
{
    MCG->C4 &= ~(uint8_t)(MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK);
    MCG->C4 |= (uint8_t)(factor);
}

/**
 * @brief Enable Oscillator module
 *
 * The module settings depend on KINETIS_MCG_ERC_RANGE
 * KINETIS_MCG_ERC_OSCILLATOR values.
 */
static void kinetis_mcg_enable_osc(void)
{
    if (KINETIS_MCG_ERC_RANGE == 1) {
        /* select high frequency range and oscillator clock */
        MCG->C2 = (uint8_t)(MCG_C2_RANGE0(1));
    }
    else if (KINETIS_MCG_ERC_RANGE == 2) {
        /* select very high frequency range and osciallor clock */
        MCG->C2 = (uint8_t)(MCG_C2_RANGE0(2));
    }
    else {
        /* select low frequency range and osciallor clock */
        MCG->C2 = (uint8_t)(MCG_C2_RANGE0(0));
    }

    OSC0->CR = (uint8_t)(OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK
                         | (KINETIS_MCG_OSC_CLC & 0xf));

    /* Enable Oscillator */
    if (KINETIS_MCG_ERC_OSCILLATOR) {
        MCG->C2 |= (uint8_t)(MCG_C2_EREFS0_MASK);

        /* wait fo OSC initialization */
        while ((MCG->S & MCG_S_OSCINIT0_MASK) == 0);
    }
}

/**
 * @brief Initialize the FLL Engaged Internal Mode.
 *
 * MCGOUTCLK is derived from the FLL clock.
 * Clock source is the 32kHz slow Internal Reference Clock.
 * The FLL loop will lock the DCO frequency to the FLL-Factor.
 */
static void kinetis_mcg_set_fei(void)
{
    kinetis_mcg_set_fll_factor(KINETIS_MCG_FLL_FACTOR_FEI);
    /* enable and select slow internal reference clock */
    MCG->C1 = (uint8_t)(MCG_C1_CLKS(0) | MCG_C1_IREFS_MASK);

    /* set to defaults */
    MCG->C2 = (uint8_t)0;

    /* source of the FLL reference clock shall be internal reference clock */
    while ((MCG->S & MCG_S_IREFST_MASK) == 0);

    /* Wait until output of the FLL is selected */
    while (MCG->S & (MCG_S_CLKST_MASK));

    kinetis_mcg_disable_pll();
    current_mode = KINETIS_MCG_FEI;
}

/**
 * @brief Initialize the FLL Engaged External Mode.
 *
 * MCGOUTCLK is derived from the FLL clock.
 * Clock source is the external reference clock (IRC or oscillator).
 * The FLL loop will lock the DCO frequency to the FLL-Factor.
 */
static void kinetis_mcg_set_fee(void)
{
    kinetis_mcg_enable_osc();
    kinetis_mcg_set_fll_factor(KINETIS_MCG_FLL_FACTOR_FEE);

    /* select external reference clock and divide factor */
    MCG->C1 = (uint8_t)(MCG_C1_CLKS(0) | MCG_C1_FRDIV(KINETIS_MCG_ERC_FRDIV));

    /* Wait until output of FLL is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(0));

    kinetis_mcg_disable_pll();
    current_mode = KINETIS_MCG_FEE;
}

/**
 * @brief Initialize the FLL Bypassed Internal Mode.
 *
 * MCGOUTCLK is derived from 32kHz IRC or 4MHz IRC.
 * FLL output is not used.
 * FLL clock source is internal 32kHz IRC.
 * The FLL loop will lock the DCO frequency to the FLL-Factor.
 * Next useful mode: BLPI or FEI.
 */
static void kinetis_mcg_set_fbi(void)
{
    /* select IRC source */
    if (KINETIS_MCG_USE_FAST_IRC) {
        MCG->C2 = (uint8_t)(MCG_C2_IRCS_MASK);
    }
    else {
        MCG->C2 = (uint8_t)(0);
    }

    kinetis_mcg_set_fll_factor(KINETIS_MCG_FLL_FACTOR_FEI);
    /* enable and select slow internal reference clock */
    MCG->C1 = (uint8_t)(MCG_C1_CLKS(1) | MCG_C1_IREFS_MASK);

    /* Wait until output of IRC is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(1));

    /* source of the FLL reference clock shall be internal reference clock */
    while ((MCG->S & MCG_S_IREFST_MASK) == 0);

    kinetis_mcg_disable_pll();
    current_mode = KINETIS_MCG_FBI;
}

/**
 * @brief Initialize the FLL Bypassed External Mode.
 *
 * MCGOUTCLK is derived from external reference clock (oscillator).
 * FLL output is not used.
 * Clock source is the external reference clock (oscillator).
 * The FLL loop will lock the DCO frequency to the FLL-Factor.
 */
static void kinetis_mcg_set_fbe(void)
{
    kinetis_mcg_enable_osc();
    kinetis_mcg_set_fll_factor(KINETIS_MCG_FLL_FACTOR_FEE);

    /* FLL is not disabled in bypass mode */
    MCG->C2 &= ~(uint8_t)(MCG_C2_LP_MASK);

    /* select external reference clock and divide factor */
    MCG->C1 = (uint8_t)(MCG_C1_CLKS(2) | MCG_C1_FRDIV(KINETIS_MCG_ERC_FRDIV));

    /* Wait until ERC is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2));

    kinetis_mcg_disable_pll();
    current_mode = KINETIS_MCG_FBE;
}


/**
 * @brief Initialize the FLL Bypassed Low Power Internal Mode.
 *
 * MCGOUTCLK is derived from IRC.
 * FLL and PLL are disable.
 * Previous and next allowed mode is FBI.
 */
static void kinetis_mcg_set_blpi(void)
{
    MCG->C2 |= (uint8_t)(MCG_C2_LP_MASK);
    current_mode = KINETIS_MCG_BLPI;
}

/**
 * @brief Initialize the FLL Bypassed Low Power External Mode.
 *
 * MCGOUTCLK is derived from ERC.
 * FLL and PLL are disable.
 * Previous and next allowed mode: FBE or PBE.
 */
static void kinetis_mcg_set_blpe(void)
{
    MCG->C2 |= (uint8_t)(MCG_C2_LP_MASK);
    current_mode = KINETIS_MCG_BLPE;
}

/**
 * @brief Initialize the PLL Bypassed External Mode.
 *
 * MCGOUTCLK is derived from external reference clock (oscillator).
 * PLL output is not used.
 * Clock source is the external reference clock (oscillator).
 * The PLL loop will locks to VDIV times the frequency
 * corresponding by PRDIV.
 * Previous allowed mode are FBE or BLPE.
 */
static void kinetis_mcg_set_pbe(void)
{
    /* select external reference clock and divide factor */
    MCG->C1 = (uint8_t)(MCG_C1_CLKS(2) | MCG_C1_FRDIV(KINETIS_MCG_ERC_FRDIV));

    /* Wait until ERC is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2));

    /* PLL is not disabled in bypass mode */
    MCG->C2 &= ~(uint8_t)(MCG_C2_LP_MASK);

    /* set external reference devider */
    MCG->C5 = (uint8_t)(MCG_C5_PRDIV0(KINETIS_MCG_PLL_PRDIV));

    /* set external reference devider */
    MCG->C6 = (uint8_t)(MCG_C6_VDIV0(KINETIS_MCG_PLL_VDIV0));

    /* select PLL */
    MCG->C6 |= (uint8_t)(MCG_C6_PLLS_MASK);

    /* Wait until the source of the PLLS clock is PLL */
    while ((MCG->S & MCG_S_PLLST_MASK) == 0);

    /* Wait until PLL locked */
    while ((MCG->S & MCG_S_LOCK0_MASK) == 0);

    current_mode = KINETIS_MCG_PBE;
}

/**
 * @brief Initialize the PLL Engaged External Mode.
 *
 * MCGOUTCLK is derived from PLL.
 * PLL output is used.
 * Previous and next allowed mode is PBE.
 */
static void kinetis_mcg_set_pee(void)
{
    MCG->C1 &= ~(uint8_t)(MCG_C1_CLKS_MASK);

    /* Wait until output of the PLL is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3));

    current_mode = KINETIS_MCG_PEE;
}


int kinetis_mcg_set_mode(kinetis_mcg_mode_t mode)
{
    if (mode > KINETIS_MCG_FEI) {
        return -1;
    }

    if (mcg_pm[current_mode] & (1 << mode)) {
        if (mode == KINETIS_MCG_FEI) {
            kinetis_mcg_set_fei();
        }

        if (mode == KINETIS_MCG_FBI) {
            kinetis_mcg_set_fbi();
        }

        if (mode == KINETIS_MCG_FEE) {
            kinetis_mcg_set_fee();
        }

        if (mode == KINETIS_MCG_FBE) {
            kinetis_mcg_set_fbe();
        }

        if (mode == KINETIS_MCG_BLPI) {
            kinetis_mcg_set_blpi();
        }

        if (mode == KINETIS_MCG_BLPE) {
            kinetis_mcg_set_blpe();
        }

        if (mode == KINETIS_MCG_PBE) {
            kinetis_mcg_set_pbe();
        }

        if (mode == KINETIS_MCG_PEE) {
            kinetis_mcg_set_pee();
        }

        return 0;
    }

    switch (mode) {
        case KINETIS_MCG_PEE:
            /* cppcheck-suppress duplicateExpression */
            if (!(KINETIS_MCG_USE_ERC || KINETIS_MCG_USE_PLL)) {
                return -1;
            }

            if (current_mode == KINETIS_MCG_FEI) {
                /* set FBE -> PBE -> PEE */
                kinetis_mcg_set_fbe();
                kinetis_mcg_set_pbe();
                kinetis_mcg_set_pee();
                return 0;
            }

            if (current_mode == KINETIS_MCG_BLPE) {
                /* set PBE -> PEE */
                kinetis_mcg_set_pbe();
                kinetis_mcg_set_pee();
                return 0;
            }

            break;

        case KINETIS_MCG_BLPE:
            if (!KINETIS_MCG_USE_ERC) {
                return -1;
            }

            if (current_mode == KINETIS_MCG_PEE) {
                /* set PBE -> BLPE */
                kinetis_mcg_set_pbe();
                kinetis_mcg_set_blpe();
                return 0;
            }

            if (current_mode == KINETIS_MCG_FEE) {
                /* set FBE -> BLPE */
                kinetis_mcg_set_fbe();
                kinetis_mcg_set_blpe();
                return 0;
            }

            break;

        case KINETIS_MCG_BLPI:
            if (current_mode == KINETIS_MCG_FEE) {
                /* set FBI -> BLPI */
                kinetis_mcg_set_fbi();
                kinetis_mcg_set_blpi();
                return 0;
            }

            break;

        case KINETIS_MCG_FEE:
            if (!KINETIS_MCG_USE_ERC) {
                return -1;
            }

            if (current_mode == KINETIS_MCG_BLPE) {
                /* set FBE -> FEE */
                kinetis_mcg_set_fbe();
                kinetis_mcg_set_fee();
                return 0;
            }

            if (current_mode == KINETIS_MCG_BLPI) {
                /* set FBI -> FEE */
                kinetis_mcg_set_fbi();
                kinetis_mcg_set_fee();
                return 0;
            }

            break;

        default:
            break;
    }

    return -1;
}

#endif /* KINETIS_CPU_USE_MCG */
