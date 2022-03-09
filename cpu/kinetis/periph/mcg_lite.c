/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     cpu_kinetis_mcg
 * @{
 *
 * @file
 * @brief       Implementation of the Kinetis Multipurpose Clock Generator (Lite version)
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 */

#include <stdint.h>
#include "periph_conf.h"
#include "mcg.h"
#include "bit.h"

/* The CPU is in LIRC8M mode after hardware reset */
static kinetis_mcg_mode_t current_mode = KINETIS_MCG_MODE_LIRC8M;

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
    /* On MCG_Lite, LIRC is divided first by LIRC_DIV1, controlled by
     * MCG_SC[FCRDIV], then by LIRC_DIV2, controlled by MCG_MC[LIRC_DIV2] */
    MCG->SC = (MCG->SC & ~MCG_SC_FCRDIV_MASK) | clock_config.fcrdiv;
    MCG->MC = (MCG->MC & ~MCG_MC_LIRC_DIV2_MASK) | clock_config.lirc_div2;
    /* on MCG_Lite, we control the IRCS flag via the mode selection (LIRC2M vs LIRC8M) */

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
 * @brief   Initialize the MCG high speed peripheral clock (MCGPCLK)
 *
 * This clock signal can be used for directly clocking certain peripherals
 */
static void kinetis_mcg_init_mcgpclk(void)
{
    if (clock_config.clock_flags & KINETIS_CLOCK_MCGPCLK_EN) {
        bit_set8(&MCG->MC, MCG_MC_HIRCEN_SHIFT);
    }
    else {
        bit_clear8(&MCG->MC, MCG_MC_HIRCEN_SHIFT);
    }
}

/**
 * @brief Initialize HIRC (48 MHz) mode.
 */
static void kinetis_mcg_set_hirc(void)
{
    /* select HIRC mode */
    MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | (MCG_C1_CLKS(0));

    /* Wait until HIRC is selected */
    while ((MCG->S & (MCG_S_CLKST_MASK)) != 0) {}

    current_mode = KINETIS_MCG_MODE_HIRC;
}

/**
 * @brief Initialize EXT (external clock) mode.
 */
static void kinetis_mcg_set_ext(void)
{
    kinetis_mcg_enable_osc();

    /* select EXT mode */
    MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | (MCG_C1_CLKS(0));

    /* Wait until HIRC is selected */
    while ((MCG->S & (MCG_S_CLKST_MASK)) != 0) {}

    current_mode = KINETIS_MCG_MODE_HIRC;
}

/**
 * @brief Initialize LIRC mode.
 *
 * Use @p ircs to select between LIRC8M (8 MHz) and LIRC2M (2 MHz).
 *
 * @param[in]   lirc8m  set to 1 -> LIRC8M, 0 -> LIRC2M
 */
static void kinetis_mcg_set_lirc(unsigned lirc8m)
{
    uint32_t clkdiv = SIM->CLKDIV1;
    if ((lirc8m && (current_mode == KINETIS_MCG_MODE_LIRC8M)) ||
        (!lirc8m && (current_mode == KINETIS_MCG_MODE_LIRC2M))) {
        /* We can not switch directly between LIRC2M <-> LIRC8M, go via HIRC */
        /* Set safe clock dividers so we don't run out of specs while switching */
        SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(3); /* divide clock by 4 => 12 MHz */
        kinetis_mcg_set_hirc();
    }

    if (lirc8m) {
        /* Select 8 MHz mode */
        bit_set8(&MCG->C2, MCG_C2_IRCS_SHIFT);
        current_mode = KINETIS_MCG_MODE_LIRC8M;
    }
    else {
        /* Select 2 MHz mode */
        bit_clear8(&MCG->C2, MCG_C2_IRCS_SHIFT);
        current_mode = KINETIS_MCG_MODE_LIRC2M;
    }

    /* select LIRC as clock source */
    MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | (MCG_C1_CLKS(1));

    /* Wait until LIRC is selected */
    while ((MCG->S & (MCG_S_CLKST_MASK)) != MCG_S_CLKST(1)) {}

    /* Restore clock divider settings */
    SIM->CLKDIV1 = clkdiv;
}

int kinetis_mcg_set_mode(kinetis_mcg_mode_t mode)
{
    switch(mode) {
        case KINETIS_MCG_MODE_LIRC8M:
            kinetis_mcg_set_lirc(1);
            break;
        case KINETIS_MCG_MODE_HIRC:
            kinetis_mcg_set_hirc();
            break;
        case KINETIS_MCG_MODE_EXT:
            kinetis_mcg_set_ext();
            break;
        case KINETIS_MCG_MODE_LIRC2M:
            kinetis_mcg_set_lirc(0);
            break;
        default:
            return -1;
    }

    return 0;
}

void kinetis_mcg_init(void)
{
    unsigned mask = irq_disable();

    /* Set module clock dividers */
    SIM->CLKDIV1 = clock_config.clkdiv1;

    kinetis_mcg_init_mcgpclk();

    kinetis_mcg_init_mcgirclk();

    kinetis_mcg_init_erclk32k();

    /* Switch to the selected MCG mode */
    kinetis_mcg_set_mode(clock_config.default_mode);
    irq_restore(mask);
}

/** @} */
