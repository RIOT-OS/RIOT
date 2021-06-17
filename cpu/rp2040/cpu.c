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

static void cpu_clock_init(void);

/**
 * @brief Configure the clock system
 */

static void cpu_clock_init(void)
{
#if CLK_REF_XTAL
uint32_t reg_v;

reg_v = xosc_hw->startup;

// Set startup delay.
#if (CLK_XTAL_FREQ == 12000000)
reg_v &= ~XOSC_STARTUP_DELAY_BITS;
reg_v |= (47 << XOSC_STARTUP_DELAY_LSB);
#endif

reg_v &= ~XOSC_CTRL_FREQ_RANGE_BITS;
reg_v |= (XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ << XOSC_CTRL_FREQ_RANGE_LSB);

reg_v &= ~XOSC_CTRL_ENABLE_BITS;
reg_v |= (XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB);

xosc_hw->startup = reg_v;

// Wait till the clock is stable.
while(!(xosc_hw->status & XOSC_STATUS_STABLE_BITS)) {
    ;
}

reg_v = clocks_hw->clk[clk_ref].ctrl;

// Set clk_ref source to XTAL.
reg_v &= ~CLOCKS_CLK_REF_CTRL_SRC_BITS;
reg_v |= (CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC << CLOCKS_CLK_REF_CTRL_SRC_LSB);

clocks_hw->clk[clk_ref].ctrl = reg_v;

// Wait till clock source is activated.
while (!(clocks_hw->clk[clk_ref].selected & (1 << CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC))) {
    ;
}
#endif
}

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* initialize the clock system */
    cpu_clock_init();
}
