/*
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== ClockPTimer_nortos.c ========
 */

#include <stdlib.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

#include "QueueP.h"
#include "TimerP.h"

#define CPU_CLOCK_HZ            ((uint32_t)48000000)
#define SYSTICK_FREQ            1000

typedef struct _ClockP_Module_State {
    QueueP_Obj clockQ;
    volatile uint32_t ticks;
    SwiP_Struct swiStruct;
    SwiP_Handle swi;
    TimerP_Struct timerStruct;
    TimerP_Handle timer;
    volatile unsigned int numTickSkip;
    uint32_t nextScheduledTick;
    uint32_t maxSkippable;
    bool inWorkFunc;
    bool startDuringWorkFunc;
    bool ticking;
} ClockP_Module_State;

typedef struct _ClockP_Obj {
    QueueP_Elem elem;
    uint32_t timeout;
    uint32_t currTimeout;
    uint32_t period;
    volatile bool active;
    ClockP_Fxn fxn;
    uintptr_t arg;
} ClockP_Obj;

static ClockP_Module_State ClockP_module;
static bool ClockP_initialized = false;
static ClockP_Params ClockP_defaultParams = {
    .startFlag = false,
    .period = 0,
    .arg = 0,
};


/*
 *  Set the default clock period to 10 microseconds for CC13XX and
 *  CC26XX devices, 1 msec for all other devices.
 */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0) || \
    (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2)
uint32_t ClockP_tickPeriod = 10;
#else
uint32_t ClockP_tickPeriod = 1000;
#endif

void ClockP_workFuncDynamic(uintptr_t arg0, uintptr_t arg1);
void ClockP_doTick();

static void sleepTicks(uint32_t ticks);

static TimerP_FreqHz timerFreq;
static uint32_t usecsPerTimerTick;

/*
 *  ======== ClockP_Params_init ========
 */
void ClockP_Params_init(ClockP_Params *params)
{
    /* structure copy */
    *params = ClockP_defaultParams;
}

/*
 *  ======== ClockP_startup ========
 */
void ClockP_startup(void)
{
    SwiP_Params swiParams;
    TimerP_Params timerParams;

    if (!ClockP_initialized) {
        QueueP_init(&ClockP_module.clockQ);
        ClockP_module.ticks = 0;
        ClockP_module.numTickSkip = 1;
        ClockP_module.nextScheduledTick = 1;
        ClockP_module.maxSkippable = 0;
        ClockP_module.inWorkFunc = false;
        ClockP_module.startDuringWorkFunc = false;
        ClockP_module.ticking = false;

        SwiP_Params_init(&swiParams);
        swiParams.priority = ~0;  /* max priority */
        ClockP_module.swi = SwiP_construct(&ClockP_module.swiStruct,
                                           (SwiP_Fxn)ClockP_workFuncDynamic,
                                           &swiParams);

        TimerP_Params_init(&timerParams);
        timerParams.period = ClockP_tickPeriod;
        ClockP_module.timer = TimerP_construct(&ClockP_module.timerStruct,
                                               (TimerP_Fxn)ClockP_doTick,
                                               &timerParams);

        /* get the max ticks that can be skipped by the timer */
        ClockP_module.maxSkippable = TimerP_getMaxTicks(ClockP_module.timer);

        /* Used for ClockP_usleep() */
        TimerP_getFreq(ClockP_module.timer, &timerFreq);
        usecsPerTimerTick = 1000000 / timerFreq.lo;

        ClockP_initialized = true;
    }
}

/*
 *  ======== ClockP_getTicks  ========
 */
uint32_t ClockP_getTicks(void)
{
    uint32_t ticks;
    uintptr_t hwiKey;

    hwiKey = HwiP_disable();

    /* do not ask Timer to save NOW */
    ticks = TimerP_getCurrentTick(ClockP_module.timer, false);

    HwiP_restore(hwiKey);

    return (ticks);
}

/*
 *  ======== ClockP_getTicksUntilInterrupt  ========
 */
uint32_t ClockP_getTicksUntilInterrupt(void)
{
    uint32_t ticks;
    uint32_t current;
    uintptr_t key;

    key = HwiP_disable();

    /* do not ask Timer to save NOW */
    current = TimerP_getCurrentTick(ClockP_module.timer, false);

    ticks = ClockP_module.nextScheduledTick - current;

    /* clamp value to zero if nextScheduledTick is less than current */
    if (ticks > ClockP_module.maxSkippable) {
        ticks = 0;
    }

    HwiP_restore(key);

    return (ticks);
}

/*
 *  ======== ClockP_scheduleNextTick  ========
 *  Must be called with global interrupts disabled!
 */
void ClockP_scheduleNextTick(uint32_t deltaTicks, uint32_t absTick)
{
    /* now reprogram the timer for the new period and next interrupt */
    TimerP_setNextTick(ClockP_module.timer, deltaTicks);

    /* remember this */
    ClockP_module.numTickSkip = deltaTicks;
    ClockP_module.nextScheduledTick = absTick;
}

/*
 *  ======== ClockP_walkQueueDynamic ========
 *  Walk the Clock Queue for TickMode_DYNAMIC, optionally servicing a
 *  specific tick
 */
uint32_t ClockP_walkQueueDynamic(bool service, uint32_t thisTick)
{
    uint32_t distance = ~0;
    QueueP_Handle clockQ;
    QueueP_Elem *elem;
    ClockP_Obj *obj;
    uint32_t delta;

    /* Traverse clock queue */
    clockQ = &ClockP_module.clockQ;
    elem = (QueueP_Elem *)QueueP_head(clockQ);

    while (elem != (QueueP_Elem *)(clockQ)) {

        obj = (ClockP_Obj *)elem;
        elem = (QueueP_Elem *)QueueP_next(elem);

        /* if  the object is active ... */
        if (obj->active == true) {

            /* optionally service if tick matches timeout */
            if (service == true) {

                /* if this object is timing out update its state */
                if (obj->currTimeout == thisTick) {

                    if (obj->period == 0) { /* oneshot? */
                        /* mark object idle */
                        obj->active = false;
                    }
                    else {                      /* periodic */
                        /* refresh timeout */
                        obj->currTimeout += obj->period;
                    }

                    /* call handler */
                    obj->fxn(obj->arg);
                }
            }

            /* if object still active update distance to soonest tick */
            if (obj->active == true) {

                delta = obj->currTimeout - thisTick;

                /* if this is the soonest tick update distance to soonest */
                if (delta < distance) {
                    distance = delta;
                }

            }
        }
    }

    return (distance);
}

/*
 *  ======== ClockP_workFuncDynamic ========
 *  Service Clock Queue for TickMode_DYNAMIC
 */
void ClockP_workFuncDynamic(uintptr_t arg0, uintptr_t arg1)
{
    uint32_t distance;
    uint32_t serviceTick, serviceDelta;
    uint32_t ticksToService;
    unsigned int skippable;
    uint32_t nowTick, nowDelta, nextTick;
    uintptr_t hwiKey;

    hwiKey = HwiP_disable();

    /* get current tick count, signal Timer to save corresponding NOW info */
    nowTick = TimerP_getCurrentTick(ClockP_module.timer, true);

    /* set flags while actively servicing queue */
    ClockP_module.inWorkFunc = true;
    ClockP_module.startDuringWorkFunc = false;

    /* determine first tick expiration to service (the anticipated next tick) */
    serviceTick = ClockP_module.nextScheduledTick;
    ticksToService = nowTick - serviceTick;

    /*
     * if now hasn't caught up to nextScheduledTick,
     * a spurious interrupt has probably occurred.
     * ignore for now...
     */

    serviceDelta = serviceTick - ClockP_module.ticks;
    nowDelta = nowTick - ClockP_module.ticks;
    if (serviceDelta > nowDelta) {
        ClockP_module.inWorkFunc = false;
        HwiP_restore(hwiKey);
        return;
    }

    HwiP_restore(hwiKey);

    distance = 0;

    /* walk queue until catch up to current tick count */
    while (ticksToService >= distance) {
        serviceTick = serviceTick + distance;
        ticksToService -= distance;
        distance = ClockP_walkQueueDynamic(true, serviceTick);
    }

    /* now determine next needed tick and setup timer for that tick ... */
    hwiKey = HwiP_disable();

    /* if ClockP_start() during processing of Q, re-walk to update distance */
    if (ClockP_module.startDuringWorkFunc == true) {
        distance = ClockP_walkQueueDynamic(false, serviceTick);
    }

    /* if no active timeouts then skip the maximum supported by the timer */
    if (distance == ~0) {
        skippable = ClockP_module.maxSkippable;
        nextTick = serviceTick + skippable;
    }
    /* else, finalize how many ticks can skip */
    else {
        skippable = distance - ticksToService;
        if (skippable > ClockP_module.maxSkippable) {
            skippable = ClockP_module.maxSkippable;
        }
        nextTick = serviceTick + skippable;
    }

    /* reprogram timer for next expected tick */
    ClockP_scheduleNextTick(skippable, nextTick);

    ClockP_module.ticking = true;
    ClockP_module.inWorkFunc = false;
    ClockP_module.ticks = serviceTick;

    HwiP_restore(hwiKey);
}

/*
 *  ======== ClockP_doTick ========
 */
void ClockP_doTick()
{
    SwiP_post(ClockP_module.swi);
}

/*
 *  ======== ClockP_construct ========
 */
ClockP_Handle ClockP_construct(ClockP_Struct *handle, ClockP_Fxn fxn,
                               uint32_t timeout, ClockP_Params *params)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    if (handle == NULL) {
        return NULL;
    }

    ClockP_startup();

    if (params == NULL) {
        params = &ClockP_defaultParams;
    }

    obj->period = params->period;
    obj->timeout = timeout;
    obj->fxn = fxn;
    obj->arg = params->arg;
    obj->active = false;

    /*
     * Clock object is always placed on Clock work Q
     */
    QueueP_put(&ClockP_module.clockQ, &obj->elem);

    if (params->startFlag) {
        ClockP_start(obj);
    }

    return ((ClockP_Handle)handle);
}

/*
 *  ======== ClockP_add ========
 */
void ClockP_add(ClockP_Struct *handle, ClockP_Fxn fxn,
                uint32_t timeout, uintptr_t arg)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    obj->period = 0;
    obj->timeout = timeout;
    obj->fxn = fxn;
    obj->arg = arg;
    obj->active = false;

    /*
     * Clock object is always placed on Clock work Q
     */
    QueueP_put(&ClockP_module.clockQ, &obj->elem);
}

/*
 *  ======== ClockP_create ========
 */
ClockP_Handle ClockP_create(ClockP_Fxn clkFxn, uint32_t timeout,
                            ClockP_Params *params)
{
    ClockP_Handle handle;

    handle = (ClockP_Handle)malloc(sizeof(ClockP_Obj));

    /* ClockP_construct will check handle for NULL, no need here */
    handle = ClockP_construct((ClockP_Struct *)handle, clkFxn, timeout,
                              params);

    return (handle);
}

/*
 *  ======== ClockP_destruct ========
 */
void ClockP_destruct(ClockP_Struct *clk)
{
    ClockP_Obj *obj = (ClockP_Obj *)clk;
    uintptr_t key;

    key = HwiP_disable();
    QueueP_remove(&obj->elem);
    HwiP_restore(key);
}

/*
 *  ======== ClockP_delete ========
 */
void ClockP_delete(ClockP_Handle handle)
{
    ClockP_destruct((ClockP_Struct *)handle);

    free(handle);
}

/*
 *  ======== ClockP_start ========
 *  Set the Clock object's currTimeout value and set its active flag
 *  to true.
 */
void ClockP_start(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;
    uintptr_t key = HwiP_disable();

    uint32_t nowTick, nowDelta;
    uint32_t scheduledTick, scheduledDelta;
    uint32_t remainingTicks;
    bool objectServiced = false;

    /* now see if need this new timeout before next scheduled tick ... */
    /* wait till after first tick */
    if ((ClockP_module.ticking == true) &&
        /* if Clock is NOT currently processing its Q */
        (ClockP_module.inWorkFunc == false)) {

        /*
         * get virtual current tick count,
         * signal Timer to save corresponding NOW info
         */
        nowTick = TimerP_getCurrentTick(ClockP_module.timer, true);

        nowDelta = nowTick - ClockP_module.ticks;
        scheduledDelta = ClockP_module.nextScheduledTick - ClockP_module.ticks;

        if (nowDelta <= scheduledDelta) {
            objectServiced = true;

            /* start new Clock object */
            obj->currTimeout = nowTick + obj->timeout;
            obj->active = true;

            /* get the next scheduled tick */
            scheduledTick = ClockP_module.nextScheduledTick;

            /* how many ticks until scheduled tick? */
            remainingTicks = scheduledTick - nowTick;

            if (obj->timeout < remainingTicks) {
                ClockP_scheduleNextTick(obj->timeout, obj->currTimeout);
            }
        }
    }

    if (objectServiced == false) {
        /*
         * get virtual current tick count,
         * DO NOT (!) signal Timer to save corresponding NOW info
         */
        nowTick = ClockP_getTicks();

        /* start new Clock object */
        obj->currTimeout = nowTick + obj->timeout;
        obj->active = true;

        if (ClockP_module.inWorkFunc == true) {
            ClockP_module.startDuringWorkFunc = true;
        }
    }

    HwiP_restore(key);
}

/*
 *  ======== ClockP_stop ========
 *  remove and clear Clock object's queue elem from clockQ
 */
void ClockP_stop(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    obj->active = false;
}

/*
 *  ======== ClockP_setTimeout ========
 */
void ClockP_setTimeout(ClockP_Handle handle, uint32_t timeout)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    obj->timeout = timeout;
}

/*
 *  ======== ClockP_getTimeout ========
 */
uint32_t ClockP_getTimeout(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    if (obj->active == true) {
        return (obj->currTimeout - ClockP_getTicks());
    }
    else {
        return (obj->timeout);
    }
}

/*
 *  ======== ClockP_isActive ========
 */
bool ClockP_isActive(ClockP_Handle handle)
{
    ClockP_Obj *obj = (ClockP_Obj *)handle;

    return (obj->active);
}

/*
 *  ======== ClockP_getCpuFreq ========
 */
void ClockP_getCpuFreq(ClockP_FreqHz *freq)
{
    freq->lo = (uint32_t)CPU_CLOCK_HZ;
    freq->hi = 0;
}

/*
 *  ======== ClockP_getSystemTickPeriod ========
 */
uint32_t ClockP_getSystemTickPeriod(void)
{
    return (ClockP_tickPeriod);
}

/*
 *  ======== ClockP_getSystemTicks ========
 */
uint32_t ClockP_getSystemTicks(void)
{
    uint32_t  ticks;
    uintptr_t key;

    ClockP_startup();

    key = HwiP_disable();

    /*
     *  Needs to be called with interrupts disabled, at least for
     *  MSP432, where ticks is computed based on the number of timer
     *  counter rollovers and the value of the timer counter register.
     */
    ticks = TimerP_getCurrentTick(ClockP_module.timer, false);

    HwiP_restore(key);

    return (ticks);
}

/*
 *  ======== ClockP_sleep ========
 */
void ClockP_sleep(uint32_t sec)
{
    uint64_t ticksToSleep;

    ClockP_startup();

    ticksToSleep = ((uint64_t)sec * (uint64_t)1000000) / (uint64_t)ClockP_tickPeriod;
    sleepTicks((uint32_t)ticksToSleep);
}

/*
 *  ======== ClockP_usleep ========
 */
void ClockP_usleep(uint32_t usec)
{
    uint64_t curTick, endTick;
    uint32_t ticksToSleep;

    ClockP_startup();

    curTick = TimerP_getCount64(ClockP_module.timer);

    /* Make sure we sleep at least one tick if usec > 0 */
    endTick = curTick + (usec + usecsPerTimerTick - 1) / usecsPerTimerTick;

    /*
     *  If usec > ClockP_tickPeriod, sleep for the appropriate number
     *  of clock ticks.
     */
    if (usec >= ClockP_tickPeriod) {
        ticksToSleep = usec / ClockP_tickPeriod;
        sleepTicks(ticksToSleep);
    }

    curTick = TimerP_getCount64(ClockP_module.timer);
    while (curTick < endTick) {
        curTick = TimerP_getCount64(ClockP_module.timer);;
    }
}

/*
 *  ======== ClockP_staticObjectSize ========
 *  Internal function for testing that ClockP_Struct is large enough
 *  to hold ClockP object.
 */
size_t ClockP_staticObjectSize(void)
{
    return (sizeof(ClockP_Obj));
}

/*
 *  ======== sleepTicks ========
 *  Sleep for a given number of ClockP ticks.
 */
static void sleepTicks(uint32_t ticks)
{
    SemaphoreP_Struct semStruct;
    SemaphoreP_Handle sem;

    if (ticks > 0) {
        /*
         *  Construct a semaphore with 0 count that will never be posted.
         *  We will timeout pending on this semaphore.
         */
        sem = SemaphoreP_construct(&semStruct, 0, NULL);
        SemaphoreP_pend(sem, ticks);
        SemaphoreP_destruct(&semStruct);
    }
}
