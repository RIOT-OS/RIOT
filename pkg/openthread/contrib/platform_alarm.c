/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Jose Ignacio Alamos <jialamos@uc.cl>
 */

#include <stdint.h>

#include "msg.h"
#include "openthread/platform/alarm.h"
#include "ot.h"
#include "thread.h"
#include "xtimer.h"
#include "timex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static xtimer_t ot_timer;
static msg_t ot_alarm_msg;

/* OpenThread will call this for starting an aDt millisecs alarm when current time is aT0 millisecs */
void otPlatAlarmStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    DEBUG("openthread: otPlatAlarmStartAt: aT0: %i, aDT: %i\n", (int) aT0, (int) aDt);
    ot_alarm_msg.type = OPENTHREAD_XTIMER_MSG_TYPE_EVENT;

    int dt;
    if (aDt == 0) {
        msg_send(&ot_alarm_msg, thread_getpid());
    }
    else {
        dt = aDt * US_PER_MS;
        xtimer_set_msg(&ot_timer, dt, &ot_alarm_msg, thread_getpid());
    }
}

/* OpenThread will call this to stop alarms */
void otPlatAlarmStop(otInstance *aInstance)
{
    DEBUG("openthread: otPlatAlarmStop\n");
    xtimer_remove(&ot_timer);
}

/* OpenThread will call this for getting running time in millisecs */
uint32_t otPlatAlarmGetNow(void)
{
    uint32_t now = xtimer_now_usec() / US_PER_MS;

    DEBUG("openthread: otPlatAlarmGetNow: %i\n", (int) now);
    return now;
}
/** @} */
