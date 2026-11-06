/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_samd5x
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

#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void pm_set(unsigned mode)
{
    bool deep = 0;
    uint32_t _mode;

    switch (mode) {
        case 0:
            DEBUG_PUTS("pm_set(): setting BACKUP mode.");
            _mode = PM_SLEEPCFG_SLEEPMODE_BACKUP;
            deep = 1;
            break;
        case 1:
            DEBUG_PUTS("pm_set(): setting HIBERNATE mode.");
            _mode = PM_SLEEPCFG_SLEEPMODE_HIBERNATE;
            deep = 1;
            break;
        case 2:
            DEBUG_PUTS("pm_set(): setting STANDBY mode.");
            _mode = PM_SLEEPCFG_SLEEPMODE_STANDBY;
            deep = 1;
            break;
        case 3:
            DEBUG_PUTS("pm_set(): setting IDLE2 mode.");
            _mode = PM_SLEEPCFG_SLEEPMODE_IDLE2;
            break;
        default:
            /* no sleep */
            return;
    }

    /* write sleep configuration */
    PM->SLEEPCFG.reg = _mode;
    /* make sure value has been set */
    while ((PM->SLEEPCFG.reg & PM_SLEEPCFG_SLEEPMODE_Msk) != _mode) {}

    sam0_cortexm_sleep(deep);
}
