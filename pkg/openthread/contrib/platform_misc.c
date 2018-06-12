/*
 * Copyright (C) Baptiste Clenet
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread misc platform abstraction
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @}
 */


#include "openthread/types.h"
#include "openthread/platform/misc.h"
#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void otPlatReset(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("reboot...\n");
    pm_reboot();
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{
    (void)aInstance;
    /* TODO: Write me! */
    return OT_PLAT_RESET_REASON_POWER_ON;
}

void otPlatWakeHost(void)
{
    /* TODO: implement an operation to wake the host from sleep state. */
}
