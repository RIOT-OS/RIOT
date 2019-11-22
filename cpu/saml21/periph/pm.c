/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_saml21
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

#define ENABLE_DEBUG (0)
#include "debug.h"

void pm_set(unsigned mode)
{
    if (mode < PM_NUM_MODES) {
        uint32_t _mode;

        switch (mode) {
            case 0:
                DEBUG_PUTS("pm_set(): setting BACKUP mode.");
                _mode = PM_SLEEPCFG_SLEEPMODE_BACKUP;
                break;
            case 1:
                DEBUG_PUTS("pm_set(): setting STANDBY mode.");
                _mode = PM_SLEEPCFG_SLEEPMODE_STANDBY;
                break;
            default: /* Falls through */
            case 2:
                DEBUG_PUTS("pm_set(): setting IDLE mode.");
#if defined(CPU_MODEL_SAMR30G18A) || defined(CPU_MODEL_SAMR34J18B)
                _mode = PM_SLEEPCFG_SLEEPMODE_IDLE;
#else
                _mode = PM_SLEEPCFG_SLEEPMODE_IDLE2;
#endif
                break;
        }

        /* write sleep configuration */
        PM->SLEEPCFG.bit.SLEEPMODE = _mode;
        /* make sure value has been set */
        while (PM->SLEEPCFG.bit.SLEEPMODE != _mode) {}
    }

    cortexm_sleep(0);
}
