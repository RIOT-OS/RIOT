/*
 * Copyright (C) 2019 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @note        Handling of wake-up from POWERDOWN & SLEEP is not implemented
 *              yet, so those states are disabled.
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void pm_set(unsigned mode)
{
    switch (mode) {
        case 0:
            /* Everything except battery backup is powered down */
            DEBUG_PUTS("pm_set(): setting Deep Power Down mode.");
            PCON |= PM_DEEP_POWERDOWN;

            /* CPU will reset on wake-up */

            break;
        case 1:
            /* PLL & Flash are powered down */
            DEBUG_PUTS("pm_set(): setting Power Down mode.");
            PCON |= PM_POWERDOWN;

            cpu_init_pll();
            cpu_init_mam();

            break;
        case 2:
            /* PLL is powered down */
            DEBUG_PUTS("pm_set(): setting Sleep mode.");
            PCON |= PM_SLEEP;

            cpu_init_pll();

            break;
        default: /* Falls through */
        case 3:
            DEBUG_PUTS("pm_set(): setting Idle mode.");
            PCON |= PM_IDLE;
            break;
    }
}
