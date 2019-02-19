/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     pkg_semtech-loramac
 * @file
 * @brief       Compatibility functions for controlling the LoRaMAC timers
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Francisco Molina <francisco.molina@inria.cl>
 * @}
 */

#include "xtimer.h"
#include "thread.h"
#include "semtech-loramac/timer.h"

extern kernel_pid_t semtech_loramac_pid;

void TimerInit(TimerEvent_t *obj, void (*cb)(void))
{
    obj->dev.target = 0;
    obj->running = 0;
    obj->cb = cb;
}

void TimerReset(TimerEvent_t *obj)
{
    TimerStop(obj);
    TimerStart(obj);
}

void TimerStart(TimerEvent_t *obj)
{
    obj->running = 1;
    xtimer_t *timer = &(obj->dev);
    msg_t *msg = &(obj->msg);
    msg->type = MSG_TYPE_MAC_TIMEOUT;
    msg->content.ptr = obj->cb;
    xtimer_set_msg(timer, obj->timeout, msg, semtech_loramac_pid);
}

void TimerStop(TimerEvent_t *obj)
{
    obj->running = 0;
    xtimer_remove(&(obj->dev));
}

void TimerSetValue(TimerEvent_t *obj, uint32_t value)
{
    if (obj->running) {
        xtimer_remove(&(obj->dev));
    }

    obj->timeout = value * US_PER_MS;
}

TimerTime_t TimerGetCurrentTime(void)
{
    uint64_t CurrentTime = xtimer_now_usec64() / US_PER_MS;
    return (TimerTime_t)CurrentTime;
}

TimerTime_t TimerGetElapsedTime(TimerTime_t savedTime)
{
    uint64_t CurrentTime = xtimer_now_usec64() / US_PER_MS;
    return (TimerTime_t)(CurrentTime - savedTime);
}

TimerTime_t TimerGetFutureTime(TimerTime_t eventInFuture)
{
    uint64_t CurrentTime = xtimer_now_usec64() / US_PER_MS;
    return (TimerTime_t)(CurrentTime + eventInFuture);
}

void TimerLowPowerHandler( void )
{

}
