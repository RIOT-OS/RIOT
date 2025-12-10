/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_msp430
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       MSP430 common periph/pm functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"

void pm_reboot(void)
{
    /* force an hardware reboot ("Power-Up Clear"), by writing
       an illegal value to the watchdog control register */
    while (1) {
        WDTCTL = 0x0000;
    }
}
