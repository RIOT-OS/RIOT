/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "vendor/hw_nvic.h"
#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void pm_set(unsigned mode)
{
    bool deep = false;
    bool switch_osc = false;

    switch (mode) {
        case 0:
            /* lowest 16k RAM are lost here, wake by GPIO */
            SYS_CTRL_PMCTL = 0x3;
            deep = true;
            break;
        case 1:
            /* lowest 16k RAM are lost here, wake by GPIO & RTT */
            SYS_CTRL_PMCTL = 0x2;
            deep = true;
            break;
        case 2:
            /* all memory retained, wake by GPIO, RTT & USB */
            SYS_CTRL_PMCTL = 0x1;
            deep = true;
            break;
        case 3:
            /* all memory retained, wake by any interrupt source */
            deep = true;
            SYS_CTRL_PMCTL = 0x0;
            break;
    }

    if (deep) {
        *(cc2538_reg_t*) NVIC_SYS_CTRL |= NVIC_SYS_CTRL_SLEEPDEEP;

        /* If we used the 32 MHz clock, we have to switch to 16 MHz for deep sleep */
        switch_osc = !SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC;
        /* set to 0 in order to ensure that the 32 MHz XOSC is started as quick
           as possible after power mode */
        SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC_PD = 0;
    }

    /* switch to 16 MHz clock */
    if (switch_osc) {
        SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC = 1;
        while (!SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.OSC) {}
    }

    cortexm_sleep(deep);

    /* switch back to 32 MHz clock */
    if (switch_osc) {
        SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC = 0;
        /* Power down the oscillator not selected by OSC bit */
        SYS_CTRL->cc2538_sys_ctrl_clk_ctrl.CLOCK_CTRLbits.OSC_PD = 1;
        while (SYS_CTRL->cc2538_sys_ctrl_clk_sta.CLOCK_STAbits.OSC) {}
    }
}
