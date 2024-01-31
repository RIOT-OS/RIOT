/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32f3x0
 * @{
 *
 * @brief       Implementation of the CPU power management for GD32F3x0
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 * @}
 */

#include <stdint.h>
#include "periph/pm.h"
#include "periph/wdt.h"

// void pm_set_lowest(void)
// {
//     __asm__ volatile ("wfi");
// }

void pm_set(unsigned mode)
{
    bool csr_deepsleep = false; /* RISC-V sleep mode */

    switch (mode) {
        case GD32_PM_STANDBY:
            csr_deepsleep = true;
            /* set the STANDBY mode flag */
            PMU->CTL |= PMU_CTL_WURST_Msk;
            /* reset the wake up flag */
            PMU->CTL |= PMU_CTL_STBMOD_Msk;
            break;
        case GD32_PM_DEEPSLEEP:
            csr_deepsleep = true;
            /* reset the STANDBY mode flag */
            PMU->CTL &= ~PMU_CTL_STBMOD_Msk;
            /* use LDO low powered in deep sleep mode */
            PMU->CTL |= PMU_CTL_LDOLP_Msk;
            break;
        case GD32_PM_IDLE:
            csr_deepsleep = false;
            break;
        default:
            break;
    }

    if (csr_deepsleep) {
    //     /* Enable WKUP pin if used */
    //     PMU->CS &= ~PMU_CS_WUPEN0_Msk;
    //     PMU->CS |= (CONFIG_PM_EWUP_USED) ? PMU_CS_WUPEN0_Msk : 0;
    //     /* set CSR_SLEEPVALUE bit in RISC-V system control register */
    //     __set_wfi_sleepmode(WFI_DEEP_SLEEP);
    // }
    // else {
    //     /* clear CSR_SLEEPVALUE bit in RISC-V system control register */
    //     __set_wfi_sleepmode(WFI_SHALLOW_SLEEP);
    }

    /* trigger sleeping, TODO wait for wake up event (__WFE) implementation */
    __WFI();

    if (csr_deepsleep) {
    //     /* clear CSR_SLEEPVALUE bit in RISC-V system control register */
    //     __set_wfi_sleepmode(WFI_SHALLOW_SLEEP);
    //     /* after deep sleep, the IRC8M is used as clock so that a clock
    //      * reinitialization is required */
    //     gd32f3x0_clock_init();
    }
}

// void pm_reboot(void)
// {
//     wdt_setup_reboot(0, 1);
//     wdt_start();
//     wdt_kick();
//     while (1) {}
// }
