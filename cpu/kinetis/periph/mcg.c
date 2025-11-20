/*
 * SPDX-FileCopyrightText: 2015 PHYTEC Messtechnik GmbH
 * SPDX-FileCopyrightText: 2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     cpu_kinetis_mcg
 * @{
 *
 * @file
 * @brief       Implementation of the Kinetis Multipurpose Clock Generator
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 */

#include <stdint.h>
#include "periph_conf.h"
#include "mcg.h"
#include "bit.h"

/* Pathfinding for the clocking modes, this table lists the next mode in the
 * chain when moving from mode <first> to mode <second> */
static const uint8_t mcg_mode_routing[8][8] = {
    {0, 1, 2, 3, 2, 3, 3, 3}, /* from FEI */
    {0, 1, 2, 3, 2, 3, 3, 3}, /* from FEE */
    {0, 1, 2, 3, 4, 3, 3, 3}, /* from FBI */
    {0, 1, 2, 3, 2, 5, 6, 6}, /* from FBE */
    {2, 2, 2, 2, 4, 2, 2, 2}, /* from BLPI */
    {3, 3, 3, 3, 3, 5, 6, 6}, /* from BLPE */
    {3, 3, 3, 3, 3, 5, 6, 7}, /* from PBE */
    {6, 6, 6, 6, 6, 6, 6, 7}, /* from PEE */
};

/* The CPU is in FEI mode after hardware reset */
static kinetis_mcg_mode_t current_mode = KINETIS_MCG_MODE_FEI;

/**
 * @brief Disable Phase Locked Loop (PLL)
 */
static inline void kinetis_mcg_disable_pll(void)
{
#if KINETIS_HAVE_PLL
    bit_clear8(&MCG->C6, MCG_C6_PLLS_SHIFT);
#endif /* KINETIS_HAVE_PLL */
}

/**
 * @brief Set Frequency Locked Loop (FLL) factor.
 *
 * The FLL will lock the DCO frequency to the FLL factor.
 * FLL factor will be selected by DRST_DRS and DMX32 bits
 * and depends on KINETIS_MCG_DCO_RANGE value.
 */
static inline void kinetis_mcg_set_fll_factor(kinetis_mcg_fll_t factor)
{
    MCG->C4 = (MCG->C4 & ~(MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) | (uint8_t)factor;
}

/**
 * @brief Enable Oscillator module
 */
static void kinetis_mcg_enable_osc(void)
{
    /* Configure ERC range for the DCO input. */
    MCG->C2 = (MCG->C2 & ~MCG_C2_RANGE0_MASK) | clock_config.erc_range;

#if defined(OSC0)
    /* Kinetis CPU with OSC module */
    /* Enable Oscillator */
    if (clock_config.clock_flags & KINETIS_CLOCK_OSC0_EN) {
        /* Configure oscillator */
        OSC0->CR = OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK | clock_config.osc_clc;
        bit_set8(&MCG->C2, MCG_C2_EREFS0_SHIFT);

        /* wait for OSC initialization */
        while ((MCG->S & MCG_S_OSCINIT0_MASK) == 0) {}
    }
    else {
        bit_clear8(&MCG->C2, MCG_C2_EREFS0_SHIFT);
    }
#elif defined(RSIM)
    /* Kinetis CPU with a radio system integration module which can provide an
     * oscillator output. */

    /* The CPUs with RSIM (currently only KW41Z, KW31Z, KW21Z) ignore the EREFS0
     * bit in MCG_C2 because they have no OSC module. These CPUs need to use the
     * RF oscillator inside the RSIM module if an oscillator is needed. */
    /* The external reference clock line on these CPUs is permanently connected
     * to the RSIM clock output, thus the RSIM, instead of the MCG, controls the
     * external clock source selection. */

    if (clock_config.clock_flags & KINETIS_CLOCK_OSC0_EN) {
        /* Disable RF oscillator bypass, if it was enabled before */
        bit_clear32(&RSIM->RF_OSC_CTRL, RSIM_RF_OSC_CTRL_RF_OSC_BYPASS_EN_SHIFT);
    }
    else {
        /* Enable RF oscillator bypass, to use the EXTAL pin as external clock
         * source without the oscillator circuit */
        bit_set32(&RSIM->RF_OSC_CTRL, RSIM_RF_OSC_CTRL_RF_OSC_BYPASS_EN_SHIFT);
    }

    /* Enable RF oscillator circuit */
    /* Current setting is that the OSC only runs in RUN and WAIT modes, see ref.man. */
    RSIM->CONTROL = (RSIM->CONTROL & ~RSIM_CONTROL_RF_OSC_EN_MASK) | RSIM_CONTROL_RF_OSC_EN(1);

    /* Wait for oscillator ready signal */
    while((RSIM->CONTROL & RSIM_CONTROL_RF_OSC_READY_MASK) == 0) {}
#endif /* defined OSC0/RSIM */
}

/**
 * @brief   Initialize the 32 kHz reference clock (ERCLK32K)
 *
 * This will enable the RTC oscillator if enabled in the configuration.
 */
static void kinetis_mcg_init_erclk32k(void)
{
    /* Enable RTC oscillator if selected */
    if (clock_config.clock_flags & KINETIS_CLOCK_RTCOSC_EN) {
        RTC_CLKEN();
        if (!(RTC->CR & RTC_CR_OSCE_MASK)) {
            /* Only touch if it was previously not running. The RTC is not reset
             * by software resets, only by power on reset */
            RTC->CR = RTC_CR_OSCE_MASK | RTC_CR_SUP_MASK | clock_config.rtc_clc;
        }
    }
    /* Select ERCLK32K source */
    SIM->SOPT1 = (SIM->SOPT1 & ~SIM_SOPT1_OSC32KSEL_MASK) | clock_config.osc32ksel;
}

/**
 * @brief   Initialize the MCG internal reference clock (MCGIRCLK)
 *
 * This clock signal can be used for directly clocking certain peripherals, and
 * can be chosen as the MCG output clock (MCGOUTCLK).
 */
static void kinetis_mcg_init_mcgirclk(void)
{
    /* Configure internal reference clock */
    if (clock_config.clock_flags & KINETIS_CLOCK_USE_FAST_IRC) {
        /* Fast IRC divider setting */
        uint8_t tmp = MCG->SC;
        /* Avoid clearing w1c flags during writeback */
        tmp &= ~(MCG_SC_ATMF_MASK | MCG_SC_LOCS0_MASK);
        /* Write new FCRDIV setting */
        tmp &= ~MCG_SC_FCRDIV_MASK;
        tmp |= clock_config.fcrdiv;
        MCG->SC = tmp;
        bit_set8(&MCG->C2, MCG_C2_IRCS_SHIFT);
    }
    else {
        bit_clear8(&MCG->C2, MCG_C2_IRCS_SHIFT);
    }
    /* Enable/disable MCGIRCLK */
    /* MCGIRCLK can be used as an alternate clock source for certain modules */
    if (clock_config.clock_flags & KINETIS_CLOCK_MCGIRCLK_EN) {
        bit_set8(&MCG->C1, MCG_C1_IRCLKEN_SHIFT);
    }
    else {
        bit_clear8(&MCG->C1, MCG_C1_IRCLKEN_SHIFT);
    }

    if (clock_config.clock_flags & KINETIS_CLOCK_MCGIRCLK_STOP_EN) {
        /* Enable MCGIRCLK during STOP (but only when also IRCLKEN is set) */
        bit_set8(&MCG->C1, MCG_C1_IREFSTEN_SHIFT);
    }
    else {
        bit_clear8(&MCG->C1, MCG_C1_IREFSTEN_SHIFT);
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
    /* The internal reference clock frequency and source is configured during
     * initialization */
    /* Select the correct FLL multiplier for the target frequency */
    kinetis_mcg_set_fll_factor(clock_config.fll_factor_fei);
    /* select slow internal reference clock for FLL source and use FLL output clock */
    MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | (MCG_C1_CLKS(0) | MCG_C1_IREFS_MASK);

    /* Make sure FLL is enabled if we have somehow ended up in an unknown state */
    bit_clear8(&MCG->C2, MCG_C2_LP_SHIFT);

    /* source of the FLL reference clock shall be internal reference clock */
    while ((MCG->S & MCG_S_IREFST_MASK) == 0) {}

    /* Wait until output of the FLL is selected */
    while (MCG->S & (MCG_S_CLKST_MASK)) {}

    current_mode = KINETIS_MCG_MODE_FEI;
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
    kinetis_mcg_set_fll_factor(clock_config.fll_factor_fee);

    /* enable and select external reference clock */
    MCG->C1 = (MCG->C1 & ~(MCG_C1_CLKS_MASK | MCG_C1_IREFS_MASK)) | (MCG_C1_CLKS(0));

    /* Wait until output of FLL is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(0)) {}

    current_mode = KINETIS_MCG_MODE_FEE;
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
    /* The internal reference clock frequency and source is configured during
     * initialization */
    /* Select the correct FLL multiplier for the target frequency */
    kinetis_mcg_set_fll_factor(clock_config.fll_factor_fei);

    /* Re-enable FLL when coming from BLPI mode */
    bit_clear8(&MCG->C2, MCG_C2_LP_SHIFT);

    /* enable and select slow internal reference clock for the FLL */
    MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | (MCG_C1_CLKS(1) | MCG_C1_IREFS_MASK);

    /* Wait until output of IRC is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(1)) {}

    /* source of the FLL reference clock shall be internal reference clock */
    while ((MCG->S & MCG_S_IREFST_MASK) == 0) {}

    current_mode = KINETIS_MCG_MODE_FBI;
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
    /* Select the correct FLL multiplier for the target frequency */
    kinetis_mcg_set_fll_factor(clock_config.fll_factor_fee);

    /* Re-enable FLL when coming from BLPE mode */
    bit_clear8(&MCG->C2, MCG_C2_LP_SHIFT);

    /* select external reference clock for FLL source */
    MCG->C1 = (MCG->C1 & ~(MCG_C1_CLKS_MASK | MCG_C1_IREFS_MASK)) | (MCG_C1_CLKS(2));

    /* Wait until ERC is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2)) {}

    kinetis_mcg_disable_pll();
    current_mode = KINETIS_MCG_MODE_FBE;
}

/**
 * @brief Initialize the FLL Bypassed Low Power Internal Mode.
 *
 * MCGOUTCLK is derived from IRC.
 * FLL and PLL are disabled.
 * Previous and next allowed mode is FBI.
 */
static void kinetis_mcg_set_blpi(void)
{
    bit_set8(&MCG->C2, MCG_C2_LP_SHIFT);
    kinetis_mcg_disable_pll();
    current_mode = KINETIS_MCG_MODE_BLPI;
}

/**
 * @brief Initialize the FLL Bypassed Low Power External Mode.
 *
 * MCGOUTCLK is derived from ERC.
 * FLL and PLL are disabled.
 * Previous and next allowed mode: FBE or PBE.
 */
static void kinetis_mcg_set_blpe(void)
{
    bit_set8(&MCG->C2, MCG_C2_LP_SHIFT);
    kinetis_mcg_disable_pll();
    current_mode = KINETIS_MCG_MODE_BLPE;
}

#if KINETIS_HAVE_PLL
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
    /* PLL is enabled, but put in bypass mode */
    bit_clear8(&MCG->C2, MCG_C2_LP_SHIFT);

    /* select external reference clock instead of FLL/PLL */
    MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(2);

    /* Wait until ERC is selected */
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2)) {}

    /* select PLL */
    bit_set8(&MCG->C6, MCG_C6_PLLS_SHIFT);

    /* Wait until the source of the PLLS clock is PLL */
    while ((MCG->S & MCG_S_PLLST_MASK) == 0) {}

    /* Wait until PLL locked */
    while ((MCG->S & MCG_S_LOCK0_MASK) == 0) {}

    current_mode = KINETIS_MCG_MODE_PBE;
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
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3)) {}

    current_mode = KINETIS_MCG_MODE_PEE;
}
#endif /* KINETIS_HAVE_PLL */

int kinetis_mcg_set_mode(kinetis_mcg_mode_t mode)
{
    if (mode >= KINETIS_MCG_MODE_NUMOF) {
        return -1;
    }

    /* Using `do` because if we already are on the desired mode we still want to
     * update the hardware settings, e.g. when using FEI mode (since FEI is the
     * hardware reset default) */
    do {
        switch(mcg_mode_routing[current_mode][mode]) {
            case KINETIS_MCG_MODE_FEI:
                kinetis_mcg_set_fei();
                break;
            case KINETIS_MCG_MODE_FEE:
                kinetis_mcg_set_fee();
                break;
            case KINETIS_MCG_MODE_FBI:
                kinetis_mcg_set_fbi();
                break;
            case KINETIS_MCG_MODE_FBE:
                kinetis_mcg_set_fbe();
                break;
            case KINETIS_MCG_MODE_BLPI:
                kinetis_mcg_set_blpi();
                break;
            case KINETIS_MCG_MODE_BLPE:
                kinetis_mcg_set_blpe();
                break;
#if KINETIS_HAVE_PLL
            case KINETIS_MCG_MODE_PEE:
                kinetis_mcg_set_pee();
                break;
            case KINETIS_MCG_MODE_PBE:
                kinetis_mcg_set_pbe();
                break;
#endif /* KINETIS_HAVE_PLL */
            default:
                return -1;
        }
    } while (current_mode != mode);

    return 0;
}

/**
 * @brief Go to a safe clocking mode that should work for all boards regardless
 * of external components
 *
 * If the board is warm rebooting, for example when starting RIOT from a boot loader,
 * the MCG may be in a different state than what we expect. We need to carefully
 * step back to a mode which is clocked by the internal reference clock before
 * trying to modify the clock settings.
 */
static void kinetis_mcg_set_safe_mode(void)
{
    if (MCG->C2 & MCG_C2_LP_MASK) {
        /* We are currently in BLPE or BLPI */
        /* Leave LP mode */
        /* Moving to either of FBI, FBE, PBE */
        bit_clear8(&MCG->C2, MCG_C2_LP_SHIFT);
    }
#if KINETIS_HAVE_PLL
    /* See if the PLL is engaged */
    if (MCG->C6 & MCG_C6_PLLS_MASK) {
        if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0) {
            /* we are currently in PEE mode, we need to step back to PBE */
            /* switch MCGOUTCLK from PLL output to ERC */
            MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(2);

            /* Wait until ERC is selected */
            while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2)) {}
        }
        bit_clear8(&MCG->C6, MCG_C6_PLLS_SHIFT);
        /* Wait until the source of the PLLS clock is FLL */
        while ((MCG->S & MCG_S_PLLST_MASK) != 0) {}
    }
#endif
    /* when we reach this line we are in one of the FLL modes: FEI, FBI, FEE, FBE */
    /* Move to FEI mode with minimum multiplier factor */
    /* Select the correct FLL multiplier for the target frequency */
    kinetis_mcg_set_fll_factor(KINETIS_MCG_FLL_FACTOR_640);
    /* select slow internal reference clock for FLL source and use FLL output clock */
    MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | (MCG_C1_CLKS(0) | MCG_C1_IREFS_MASK);

    /* source of the FLL reference clock shall be internal reference clock */
    while ((MCG->S & MCG_S_IREFST_MASK) == 0) {}

    /* Wait until output of the FLL is selected */
    while (MCG->S & (MCG_S_CLKST_MASK)) {}

    current_mode = KINETIS_MCG_MODE_FEI;
    /* We avoid messing with the settings of the internal reference clock here
     * because it may be driving the LPTMR */
    /* At this point the core will be clocked from the FLL running off the slow
     * internal reference clock (32768 Hz) with a 640 multiplier which yields
     * 32.768 kHz x 640 = 20.971520 MHz. This should be safe regardless of the
     * SIM_CLKDIV1 settings used, for all supported Kinetis CPUs */
}

void kinetis_mcg_init(void)
{
    unsigned mask = irq_disable();
    /* Go to FBI mode for safety */
    kinetis_mcg_set_safe_mode();
    /* at this point the core should be running from the internal reference
     * clock, slow or fast, which is 32.768 kHz up to 4 MHz, depending on
     * previous settings */

    /* Set module clock dividers */
    SIM->CLKDIV1 = clock_config.clkdiv1;

    /* Select external reference clock source for the FLL */
    MCG->C7 = clock_config.oscsel;

    /* Set external reference clock divider for the FLL */
    MCG->C1 = (MCG->C1 & ~MCG_C1_FRDIV_MASK) | clock_config.fll_frdiv;

#if KINETIS_HAVE_PLL
    /* set ERC divider for the PLL */
    MCG->C5 = clock_config.pll_prdiv;

    /* set PLL VCO divider */
    MCG->C6 = clock_config.pll_vdiv;
#endif /* KINETIS_HAVE_PLL */

    kinetis_mcg_init_mcgirclk();

    kinetis_mcg_init_erclk32k();

    /* Switch to the selected MCG mode */
    kinetis_mcg_set_mode(clock_config.default_mode);
    irq_restore(mask);
}

/** @} */
