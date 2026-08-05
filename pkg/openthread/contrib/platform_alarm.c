/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread alarm platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include <stdint.h>

#include "msg.h"
#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/alarm-micro.h"
#include "ot.h"
#include "thread.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct {
    event_t super;
    _Bool isUsec;
} timer_event_t;

static void _ev_timer_handler(event_t *event)
{
    timer_event_t *timer_event = container_of(event, timer_event_t, super);

    if (timer_event->isUsec) {
        otPlatAlarmMicroFired(openthread_get_instance());
    }
    else {
        otPlatAlarmMilliFired(openthread_get_instance());
    }
}

static timer_event_t ev_timer_milli = {
    .super.handler = _ev_timer_handler,
    .isUsec = false
};

static timer_event_t ev_timer_micro = {
    .super.handler = _ev_timer_handler,
    .isUsec = true
};

void _timeout_cb_milli(void *arg)
{
    (void)arg;
    event_post(openthread_get_evq(), &ev_timer_milli.super);
}

void _timeout_cb_micro(void *arg)
{
    (void)arg;
    event_post(openthread_get_evq(), &ev_timer_micro.super);
}

static ztimer_t ot_timer_milli = {
    .callback = _timeout_cb_milli,
};

static ztimer_t ot_timer_micro = {
    .callback = _timeout_cb_micro,
};

/**
 * Set the alarm to fire at @p aDt milliseconds after @p aT0.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aT0        The reference time.
 * @param[in] aDt        The time delay in milliseconds from @p aT0.
 */
void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void)aInstance;
    (void)aT0;

    DEBUG("openthread: otPlatAlarmStartAt: aT0: %" PRIu32 ", aDT: %" PRIu32 "\n", aT0, aDt);

    if (aDt == 0) {
        event_post(openthread_get_evq(), &ev_timer_milli.super);
    }
    else {
        ztimer_set(ZTIMER_MSEC, &ot_timer_milli, aDt);
    }
}

/* OpenThread will call this to stop alarms */
void otPlatAlarmMilliStop(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("openthread: otPlatAlarmStop\n");
    ztimer_remove(ZTIMER_MSEC, &ot_timer_milli);
}

/* OpenThread will call this for getting running time in millisecs */
uint32_t otPlatAlarmMilliGetNow(void)
{
    uint32_t now = ztimer_now(ZTIMER_MSEC);

    DEBUG("openthread: otPlatAlarmGetNow: %" PRIu32 "\n", now);
    return now;
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void)aInstance;
    (void)aT0;

    DEBUG("openthread: otPlatAlarmStartAt: aT0: %" PRIu32 ", aDT: %" PRIu32 "\n", aT0, aDt);

    if (aDt == 0) {
        event_post(openthread_get_evq(), &ev_timer_micro.super);
    }
    else {
        ztimer_set(ZTIMER_USEC, &ot_timer_micro, aDt);
    }
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("openthread: otPlatAlarmStop\n");
    ztimer_remove(ZTIMER_USEC, &ot_timer_micro);
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    uint32_t now = ztimer_now(ZTIMER_USEC);

    DEBUG("openthread: otPlatAlarmGetNow: %" PRIu32 "\n", now);
    return now;
}
