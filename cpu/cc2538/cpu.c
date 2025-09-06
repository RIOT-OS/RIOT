/*
 * SPDX-FileCopyrightText: 2014 Loci Controls Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "stdio_base.h"

#include "cpu.h"
#include "kernel_init.h"
#include "periph/init.h"
#include "periph_conf.h"

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
    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();
    /* trigger static peripheral initialization */
    periph_init();
}

/**
 * @brief Configure the controllers clock system
 */
static void cpu_clock_init(void)
{
#if SYS_CTRL_OSC32K_USE_XTAL
    /* set the XOSC32K_Q pads to analog for the external crystal: */
    gpio_software_control(GPIO_PD6);
    gpio_dir_input(GPIO_PD6);
    IOC_PXX_OVER[GPIO_PD6] = IOC_OVERRIDE_ANA;

    gpio_software_control(GPIO_PD7);
    gpio_dir_input(GPIO_PD7);
    IOC_PXX_OVER[GPIO_PD7] = IOC_OVERRIDE_ANA;
#endif

    /* CLOCK_CTRL.OSC32K register bit can be written at any time, but only takes
       effect when 16MHz RCOSC is the active system clock source, 16MHz RCOSC is
       the active clock on reset but to be idempotent make sure it's the
       selected source clock */
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC = 1;
    while (SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.SOURCE_CHANGE) {}

#if SYS_CTRL_OSC32K_USE_XTAL
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC32K = 0;
    while (SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.OSC32K) {}
#else
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC32K = 1;
    while (!SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.OSC32K) {}
#endif

#if SYS_CTRL_OSC_USE_XTAL
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC = 0;
#else
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC = 1;
#endif

    /* power down the oscillator not selected by OSC bit */
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC_PD = 1;
    /* set desired system clock rate */
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.SYS_DIV = \
        __builtin_ctz(XOSC32M_FREQ / CLOCK_CORECLOCK);
    /* set desired I/O clock rate */
    SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.IO_DIV = \
        __builtin_ctz(XOSC32M_FREQ / CLOCK_IO);
    /* wait for new clock settings to take effect */
    while (SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.SOURCE_CHANGE) {}

#if SYS_CTRL_OSC32K_USE_XTAL
    /* 32-kHz crystal oscillator to stabilize after a positive transition */
    while (SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.SYNC_32K) {}
    while (!SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.SYNC_32K) {}
#endif
}
