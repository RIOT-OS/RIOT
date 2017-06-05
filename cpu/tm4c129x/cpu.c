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
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "kernel.h"
#include "kernel_internal.h"
#include "sched.h"
#include "thread.h"
#include "arch/thread_arch.h"
#include "arch/irq_arch.h"
#include "hw/hw_nvic.h"
#include "hw/hw_types.h"
#include "cpu_conf.h"
#include "periph_conf.h"

uint32_t sysClkValue = 0;

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initializes the Cortex-M core */
    cortexm_init();

    HWREG(NVIC_CPAC) = ((HWREG(NVIC_CPAC) &
                         ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) |
                        NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL);
    /* initialize the clock system */
    cpu_clock_init(CLOCK_SOURCE);

    /* Call to initialize FPU core TODO: This could be moved to the cortexm-init */
    setup_fpu();
}
void setup_fpu(void)
{
    /* initializes the Cortex-M FPU Core */
    ROM_FPUEnable();

    /* TODO: Check to see if I need this call and explain it*/
    ROM_FPULazyStackingEnable();
}

void cpu_clock_init(int clk)
{
//    setup_fpu();/* TODO: see if I want to move this */

    switch(clk){
        case CLK120:
            sysClkValue = ROM_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
            break;
            case CLK80:

                break;
            case CLK50:
            //    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
                break;
            case CLK40:
            //    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
                break;
            case CLK16:
            //    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
                break;
            case CLK1:
            //    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_1MHZ | SYSCTL_OSC_MAIN);
                break;
            default:
            //    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
                break;
    }
}

uint32_t cpu_getClock(void) {
    return sysClkValue;
}
