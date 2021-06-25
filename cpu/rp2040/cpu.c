/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rp2040
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"

static uint32_t c;

static void cpu_clock_init(void);
static uint32_t fc_clk(uint32_t clk);

static uint32_t fc_clk(uint32_t clk) {
    /*
    /// \tag::frequency_count_khz[]
    uint32_t frequency_count_khz(uint src) {
        fc_hw_t *fc = &clocks_hw->fc0;

        // If frequency counter is running need to wait for it. It runs even if the source is NULL
        while(fc->status & CLOCKS_FC0_STATUS_RUNNING_BITS) {
            tight_loop_contents();
        }

        // Set reference freq
        fc->ref_khz = clock_get_hz(clk_ref) / 1000;

        // FIXME: Don't pick random interval. Use best interval
        fc->interval = 10;

        // No min or max
        fc->min_khz = 0;
        fc->max_khz = 0xffffffff;

        // Set SRC which automatically starts the measurement
        fc->src = src;

        while(!(fc->status & CLOCKS_FC0_STATUS_DONE_BITS)) {
            tight_loop_contents();
        }

        // Return the result
        return fc->result >> CLOCKS_FC0_RESULT_KHZ_LSB;
    }
    /// \end::frequency_count_khz[]
    */

    while(clocks_hw->fc0.status & CLOCKS_FC0_STATUS_RUNNING_BITS) {}

    clocks_hw->fc0.ref_khz = 12000;

    clocks_hw->fc0.interval = 10;

    clocks_hw->fc0.min_khz = 0;
    clocks_hw->fc0.max_khz = 0xffffffff;

    clocks_hw->fc0.src = (clk << CLOCKS_FC0_SRC_LSB);

    while(!(clocks_hw->fc0.status & CLOCKS_FC0_STATUS_DONE_BITS)) {}

    c = (clocks_hw->fc0.result >> CLOCKS_FC0_RESULT_KHZ_LSB);

    return c;
}
/**
 * @brief Configure the clock system
 */

static void cpu_clock_init(void) {
    #if CLK_REF_XTAL
        assert(CLK_XTAL_FREQ <= 15000000);

        xosc_hw->startup = ((CLK_XTAL_FREQ / 10000) + 128) / 256;

        xosc_hw->ctrl |=
            ((XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB) | XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ);

        // Wait till the clock is stable.
        while(!(xosc_hw->status & XOSC_STATUS_STABLE_BITS)) {}

        // Set clk_ref source to XTAL.
        clocks_hw->clk[clk_ref].ctrl |= CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC;

        clocks_hw->clk[clk_ref].div = (3 << CLOCKS_CLK_REF_DIV_INT_LSB);

        // Wait till clock source is activated.
        while (!(clocks_hw->clk[clk_ref].selected & (1 << CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC))) {}

        clocks_hw->clk[clk_peri].ctrl |= CLOCKS_CLK_PERI_CTRL_ENABLE_BITS;

        fc_clk(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    #endif
}

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void) {
    /* initialize the Cortex-M core */
    cortexm_init();

    /* initialize the clock system */
    cpu_clock_init();
}
