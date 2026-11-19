/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of pm_off()
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "periph/pm.h"

#ifdef PM_SLEEPCFG_SLEEPMODE_OFF
void pm_off(void)
{
    irq_disable();

    PM->SLEEPCFG.bit.SLEEPMODE = PM_SLEEPCFG_SLEEPMODE_OFF;
    while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_OFF) {}

    cortexm_sleep(1);
}
#endif
