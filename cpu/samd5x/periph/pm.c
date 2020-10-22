/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
        default: /* Falls through */
        case 3:
            DEBUG_PUTS("pm_set(): setting IDLE2 mode.");
            _mode = PM_SLEEPCFG_SLEEPMODE_IDLE2;
            break;
    }

    /* write sleep configuration */
    PM->SLEEPCFG.bit.SLEEPMODE = _mode;
    /* make sure value has been set */
    while (PM->SLEEPCFG.bit.SLEEPMODE != _mode) {}

    sam0_cortexm_sleep(deep);
}
