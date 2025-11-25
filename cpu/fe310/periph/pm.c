/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        pm.c
 * @brief       Implementation of the CPU power management for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdint.h>
#include "periph/pm.h"
#include "vendor/platform.h"

void pm_set_lowest(void)
{
    __asm__ volatile ("wfi");
}

void pm_reboot(void)
{
    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCMP) = 0;
    //wdogconfig: : wdogrsten | enablealways | reset to 0 | max scale
    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) |= (AON_WDOGCFG_RSTEN | AON_WDOGCFG_ENALWAYS | \
                             AON_WDOGCFG_ZEROCMP | AON_WDOGCFG_SCALE);
    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGFEED) = AON_WDOGFEED_VALUE;
    while (1) {}
}
