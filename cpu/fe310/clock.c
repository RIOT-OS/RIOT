/*
 * Copyright (C) 2017, 2019 Ken Rabold, JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the clock initialization for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"

#include "vendor/prci_driver.h"

#if IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC) || IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC_PLL)
static uint32_t _cpu_frequency = 0;
#endif

void clock_init(void)
{
    /* Ensure that we aren't running off the PLL before we mess with it. */
    if (PRCI_REG(PRCI_PLLCFG) & PLL_SEL(1)) {
        /* Make sure the HFROSC is running at its default setting */
        /* It is OK to change this even if we are running off of it.*/
        PRCI_REG(PRCI_HFROSCCFG) = (ROSC_DIV(4) | ROSC_TRIM(16) | ROSC_EN(1));

        /* Wait for HFROSC to be ready */
        while ((PRCI_REG(PRCI_HFROSCCFG) & ROSC_RDY(1)) == 0) {}

        /* Don't use PLL clock source */
        PRCI_REG(PRCI_PLLCFG) &= ~PLL_SEL(PLL_SEL_PLL);
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HFXOSC) ||
        IS_ACTIVE(CONFIG_USE_CLOCK_HFXOSC_PLL)) {
        /* Ensure HFXOSC is enabled */
        PRCI_REG(PRCI_HFXOSCCFG) = XOSC_EN(1);

        /* Wait for HFXOSC to become ready */
        while ((PRCI_REG(PRCI_HFXOSCCFG) & XOSC_RDY(1)) == 0) {}

        /* Select HFXOSC as reference frequency and bypass PLL */
        PRCI_REG(PRCI_PLLCFG) = PLL_REFSEL(PLL_REFSEL_HFXOSC) | PLL_BYPASS(1);

        if (IS_ACTIVE(CONFIG_USE_CLOCK_HFXOSC_PLL)) {
            /* Divide final output frequency by 1 */
            PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV_BY_1(1) | PLL_FINAL_DIV(0));

            /* Configure PLL */
            PRCI_REG(PRCI_PLLCFG) |= PLL_R(CONFIG_CLOCK_PLL_R) | PLL_F(
                CONFIG_CLOCK_PLL_F) | PLL_Q(CONFIG_CLOCK_PLL_Q);

            /* Disable PLL Bypass */
            PRCI_REG(PRCI_PLLCFG) &= ~PLL_BYPASS(1);

            /* Now it is safe to check for PLL Lock */
            while ((PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1)) == 0) {}
        }

        /* Switch over to PLL Clock source */
        PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(PLL_SEL_PLL);

        /* Turn off the HFROSC */
        PRCI_REG(PRCI_HFROSCCFG) &= ~ROSC_EN(1);
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC_PLL)) {
        PRCI_set_hfrosctrim_for_f_cpu(CONFIG_CLOCK_DESIRED_FREQUENCY,
                                      PRCI_FREQ_UNDERSHOOT);
    }
    else { /* Clock HFROSC */
        /* Disable Bypass */
        PRCI_REG(PRCI_PLLCFG) &= ~PLL_BYPASS(1);

        /* Configure trim and divider values of HFROSC */
        PRCI_REG(PRCI_HFROSCCFG) =
            (ROSC_DIV(CONFIG_CLOCK_HFROSC_DIV) |
             ROSC_TRIM(CONFIG_CLOCK_HFROSC_TRIM) | ROSC_EN(1));

        /* Wait for HFROSC to be ready */
        while ((PRCI_REG(PRCI_HFROSCCFG) & ROSC_RDY(1)) == 0) {}

        /* Don't use PLL clock source */
        PRCI_REG(PRCI_PLLCFG) &= ~PLL_SEL(PLL_SEL_PLL);
    }
}

uint32_t cpu_freq(void)
{
#if IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC) || IS_ACTIVE(CONFIG_USE_CLOCK_HFROSC_PLL)
    /* Clock frequency with HFROSC cannot be determined precisely from
       settings */
    /* If not done already, estimate the CPU frequency */
    if (_cpu_frequency == 0) {
        /* Ignore the first run (for icache reasons) */
        _cpu_frequency = PRCI_measure_mcycle_freq(3000, RTC_FREQ);
        _cpu_frequency = PRCI_measure_mcycle_freq(3000, RTC_FREQ);
    }
    return _cpu_frequency;
#else
    return CLOCK_CORECLOCK;
#endif
}
