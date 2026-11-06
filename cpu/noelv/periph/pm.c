/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_noelv
 * @{
 *
 * @file
 * @brief       Power management for the Gaisler NOEL-V
 *
 * @author      Matvii Ivashchenko
 * @}
 */

#include "irq.h"
#include "periph/pm.h"

void pm_set_lowest(void)
{
    __asm__ volatile ("wfi");
}

void pm_reboot(void)
{
    /* The GRLIB reference design has no system reset controller: restart
     * the firmware by jumping to the reset entry point instead. Peripherals
     * are re-initialized by their drivers on startup. */
    extern void _start(void);

    irq_disable();
    _start();

    while (1) {}
}
