/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file        cpu.c
 * @brief       Implementation of the CPU initialization
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#include "cpu.h"
#include "irq.h"
#include "kernel.h"
#include "kernel_internal.h"
#include "sched.h"
#include "thread.h"
#include "arch/thread_arch.h"
#include "arch/irq_arch.h"

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initializes the Cortex-M core */
    cortexm_init();

    /* initialize the clock system */
    cpu_clock_init(CLK40);
}

void setup_fpu(void)
{
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();
}

void cpu_clock_init(int clk)
{
    setup_fpu();
    switch(clk){
        case CLK80:
            ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
            break;
        case CLK50:
            ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
            break;
        case CLK40:
            ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
            break;
        case CLK16:
            ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
            break;
        case CLK1:
            ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_1MHZ | SYSCTL_OSC_MAIN);
            break;
        default:
            ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
            break;
    }
}
/** @} */
