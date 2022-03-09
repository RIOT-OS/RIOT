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
    (void) event;
    otInstance *instance = openthread_get_instance();
    while (otTaskletsArePending(instance)) {
        otTaskletsProcess(instance);
    }
}

static event_t ev_tasklet = {
    .handler = _ev_tasklets_handler
};

/* OpenThread will call this when switching state from empty tasklet to non-empty tasklet. */
void otTaskletsSignalPending(otInstance *aInstance) {
    (void) aInstance;
    event_post(openthread_get_evq(), &ev_tasklet);
}

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
