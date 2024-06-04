/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml1x
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void pm_set(unsigned mode)
{
    int deep = 0;
    uint32_t _mode;

    switch (mode) {
        case 0:
            DEBUG_PUTS("pm_set(): setting STANDBY mode.");
            _mode = PM_SLEEPCFG_SLEEPMODE_STANDBY;
            deep = 1;
            break;
        default: /* Falls through */
        case 1:
            DEBUG_PUTS("pm_set(): setting IDLE mode.");
            _mode = PM_SLEEPCFG_SLEEPMODE_IDLE;
            break;
    }

    /* write sleep configuration */
    PM->SLEEPCFG.reg = _mode;
    /* make sure value has been set */
    while ((PM->SLEEPCFG.reg & PM_SLEEPCFG_SLEEPMODE_Msk) != _mode) {}

    sam0_cortexm_sleep(deep);
}
