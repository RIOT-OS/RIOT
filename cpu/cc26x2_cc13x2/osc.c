/*
 * SPDX-FileCopyrightText: 2020 Locha Inc
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 Oscillator functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cpu.h"

static inline bool _hf_source_ready(void)
{
    if (DDI_0_OSC->STAT0 & DDI_0_OSC_STAT0_PENDINGSCLKHFSWITCHING) {
        return true;
    }
    return false;
}

static inline uint32_t _hf_source_get(void)
{
    return (DDI_0_OSC->STAT0 & DDI_0_OSC_STAT0_SCLK_HF_SRC_m) >>
           DDI_0_OSC_STAT0_SCLK_HF_SRC_s;
}

static inline void _hf_source_set(uint32_t osc)
{
    uint32_t mask = DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_m;
    uint32_t ctl = osc << DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_s;

    /* Use a 16-bit masked write, target bits are on the lower 16-bit
     * half */
    DDI_0_OSC_M16->CTL0.LOW = (mask << 16) | ctl;
}

void osc_hf_source_switch(uint32_t osc)
{
    if (_hf_source_get() == osc) {
        return;
    }

    /* Request oscillator change */
    _hf_source_set(osc);

    /* Wait for the oscillator to be ready */
    while (!_hf_source_ready()) {}

    /* If target clock source is RCOSC, change clock source for DCDC to RCOSC */
    if (osc == OSC_RCOSC_HF) {
        /* Force DCDC to use RCOSC before switching SCLK_HF to RCOSC */
        uint32_t mask = DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m;
        uint32_t data = DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m >> 16;
        DDI_0_OSC_M16->CTL0.HIGH = mask | data;

        /* Dummy read to ensure that the write has propagated */
        DDI_0_OSC->CTL0;
    }

    /* Switch the HF clock source */
    rom_hapi_hf_source_safe_switch();

    /* If target clock source is XOSC, change clock source for DCDC to "auto" */
    if (osc == OSC_XOSC_HF) {
        /* Set DCDC clock source back to "auto" after SCLK_HF was switched to
         * XOSC */
        uint32_t mask = DDI_0_OSC_CTL0_CLK_DCDC_SRC_SEL_m;
        uint32_t data = 0;

        DDI_0_OSC_M16->CTL0.HIGH = mask | data;
    }
}
