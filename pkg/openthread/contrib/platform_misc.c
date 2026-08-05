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

#include "openthread/platform/misc.h"
#include "periph/pm.h"
#include "ot.h"
#include "openthread/tasklet.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _ev_tasklets_handler(event_t *event)
{
    (void)event;
    otInstance *instance = openthread_get_instance();
    while (otTaskletsArePending(instance)) {
        otTaskletsProcess(instance);
    }
}

static event_t ev_tasklet = {
    .handler = _ev_tasklets_handler
};

/* OpenThread will call this when switching state from empty tasklet to non-empty tasklet. */
void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
    event_post(openthread_get_evq(), &ev_tasklet);
}

void otPlatReset(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("reboot...\n");
    pm_reboot();
}

otError otPlatResetToBootloader(otInstance *aInstance)
{
    DEBUG("openthread: otPlatResetToBootloader is not implemented");
    (void)aInstance;
    return OT_ERROR_NOT_CAPABLE;
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{
    DEBUG("openthread: otPlatGetResetReason is not implemented");
    (void)aInstance;
    return OT_PLAT_RESET_REASON_UNKNOWN;
}

void otPlatAssertFail(const char *aFilename, int aLineNumber)
{
    DEBUG("openthread: otPlatAssertFail is not implemented");
    (void)aFilename;
    (void)aLineNumber;
}

void otPlatWakeHost(void)
{
    DEBUG("openthread: otPlatWakeHost is not implemented");
}

otError otPlatSetMcuPowerState(otInstance *aInstance, otPlatMcuPowerState aState)
{
    DEBUG("openthread: otPlatSetMcuPowerState is not implemented");
    (void)aInstance;
    (void)aState;
    return OT_ERROR_FAILED;
}

otPlatMcuPowerState otPlatGetMcuPowerState(otInstance *aInstance)
{
    DEBUG("openthread: otPlatGetMcuPowerState is not implemented");
    (void)aInstance;
    return OT_PLAT_MCU_POWER_STATE_OFF;
}

otError otPlatLogCrashDump(void)
{
    DEBUG("openthread: otPlatLogCrashDump is not implemented");
    return OT_ERROR_NOT_IMPLEMENTED;
}
