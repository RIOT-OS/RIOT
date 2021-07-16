/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_rp2040
 * @{
 *
 * @file cpu.c
 * @brief Implementation of CPU initialisation
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "stdio_base.h"
#include "periph_cpu.h"
#include "stdio_uart.h"
#include "periph_conf.h"

static void clock_init(void);

static void clock_init(void) {
    assert(CLK_XTAL_HZ <= 15000000);

    xosc_hw->startup = ((CLK_XTAL_HZ / 1000) + 128) / 256;

    xosc_hw->ctrl = XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ;

    xosc_hw->ctrl |= (XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB);

    // Wait till the clock is stable.
    while (!(xosc_hw->status & XOSC_STATUS_STABLE_BITS)) {}

    clocks_hw->clk[clk_ref].ctrl |= CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC;

    // Wait till clock source is activated.
    while (
        !(clocks_hw->clk[clk_ref].selected & (1 << CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC))
    ) {}

    // Enable peripheral clock.
    clocks_hw->clk[clk_peri].ctrl |= CLOCKS_CLK_PERI_CTRL_ENABLE_BITS;

    // Configure watchdog tick and enable.
    watchdog_hw->tick = (CLK_XTAL_HZ / 1000000) | WATCHDOG_TICK_ENABLE_BITS;
}

uint32_t get_clk_khz(unsigned int clk_src_idx) {
    assert(
        (clk_src_idx >= CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY) &&
        (clk_src_idx <= CLOCKS_FC0_SRC_VALUE_CLK_RTC)
    );

    uint32_t clk_khz = 0;

    clocks_hw->fc0.interval = 15;
    clocks_hw->fc0.ref_khz = CLK_XTAL_HZ / 1000;

    // Wait for any previous count to finish.
    while (!(clocks_hw->fc0.status & CLOCKS_FC0_STATUS_DONE_BITS)) {}

    // Initiate count.
    clocks_hw->fc0.src = clk_src_idx;

    // Wait till the count has finished.
    while (!(clocks_hw->fc0.status & CLOCKS_FC0_STATUS_DONE_BITS)) {}

    clk_khz = (clocks_hw->fc0.result >> CLOCKS_FC0_RESULT_KHZ_LSB);

    clocks_hw->fc0.src = CLOCKS_FC0_SRC_VALUE_NULL;
    clocks_hw->fc0.ref_khz = 0;

    return clk_khz;
}

/**
 * @brief Initialise the CPU
 */
void cpu_init(void) {
    cortexm_init();

    clock_init();

    stdio_init();
}
