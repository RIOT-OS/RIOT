/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/init.h"

#define BIT(n)          ( 1UL << (n) )

/* Select CLOCK_CTRL register bit masks: */
#define OSC32K          BIT(24) /**< 32-kHz clock oscillator selection */
#define OSC_PD          BIT(17) /**< Oscillator power-down */
#define OSC             BIT(16) /**< System clock oscillator selection */

/* CLOCK_CTRL register field offsets: */
#define IO_DIV_SHIFT    8
#define SYS_DIV_SHIFT   0

#define CLOCK_STA_MASK ( OSC32K | OSC )

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* Enable the CC2538's more compact alternate interrupt mapping */
    SYS_CTRL->I_MAP = 1;
    /* initialize the clock system */
    cpu_clock_init();
    /* trigger static peripheral initialization */
    periph_init();
}

/**
 * @brief Configure the controllers clock system
 */
static void cpu_clock_init(void)
{
    const uint32_t CLOCK_CTRL_VALUE =
        OSC_PD                  /**< Power down the oscillator not selected by OSC bit (hardware-controlled when selected). */
        | (1 << IO_DIV_SHIFT)   /**< 16 MHz IO_DIV */
        | (1 << SYS_DIV_SHIFT)  /**< 16 MHz SYS_DIV */
#if !SYS_CTRL_OSC32K_USE_XTAL
        | OSC32K                /**< Use internal RC oscillator. */
#endif
    ;

#if SYS_CTRL_OSC32K_USE_XTAL
    /* Set the XOSC32K_Q pads to analog for the external crystal: */
    gpio_software_control(GPIO_PD6);
    gpio_dir_input(GPIO_PD6);
    IOC_PXX_OVER[GPIO_PD6] = IOC_OVERRIDE_ANA;

    gpio_software_control(GPIO_PD7);
    gpio_dir_input(GPIO_PD7);
    IOC_PXX_OVER[GPIO_PD7] = IOC_OVERRIDE_ANA;
#endif

    /* Configure the clock settings: */
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRL = CLOCK_CTRL_VALUE;

    /* Wait for the new clock settings to take effect: */
    while ((SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STA ^ CLOCK_CTRL_VALUE) & CLOCK_STA_MASK);

#if SYS_CTRL_OSC32K_USE_XTAL
    /* Wait for the 32-kHz crystal oscillator to stabilize: */
    while ( SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.SYNC_32K);
    while (!SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.SYNC_32K);
#endif
}
