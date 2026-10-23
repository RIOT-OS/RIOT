/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rp2350
 * @{
 *
 * @file
 * @brief       Power management for the RISC-V cores of the RP2350
 *
 * @author      Christopher Buechse
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#include "periph_cpu.h"
#include "regs/psm.h"
#include "regs/watchdog.h"

void pm_set_lowest(void)
{
    __asm__ volatile ("wfi");
}

void pm_reboot(void)
{
    irq_disable();

    /* make sure the boot ROM does a regular boot instead of jumping to the
     * entry point stored in the watchdog scratch registers */
    WATCHDOG->SCRATCH4 = 0;
    /* reset everything except the oscillators, as the Pico SDK does */
    PSM->WDSEL = PSM_WDSEL_BITS & ~(PSM_WDSEL_ROSC_BITS | PSM_WDSEL_XOSC_BITS);
    /* trigger a watchdog reset immediately */
    WATCHDOG->CTRL = WATCHDOG_CTRL_TRIGGER_BITS;

    while (1) {}
}
