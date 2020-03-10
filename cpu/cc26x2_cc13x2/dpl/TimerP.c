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
 *  ======== TimerPCC26XX_nortos.c ========
 */

#include <stdlib.h>

#include <driverlib/aon_rtc.h>
#include <driverlib/aon_event.h>
#include <driverlib/interrupt.h>

#include <ti/drivers/dpl/HwiP.h>
#include "TimerP.h"

#define COMPARE_MARGIN 4

#define MAX_SKIP  (0x7E9000000000)    /* 32400 seconds (9 hours) */
#define TIMER_FREQ 65536

typedef struct _TimerP_Obj {
    TimerP_StartMode        startMode;
    uint32_t                period;
    uintptr_t               arg;
    HwiP_Fxn                tickFxn;
    TimerP_FreqHz           frequency;
    uint64_t                period64;
    uint64_t                savedCurrCount;
    uint64_t                prevThreshold;
    uint64_t                nextThreshold;
} TimerP_Obj;

static bool TimerP_initialized = false;
static TimerP_Obj *TimerP_handle;
static HwiP_Struct TimerP_hwiStruct;
//static unsigned int TimerP_availMask = 0x7;

static TimerP_Params TimerP_defaultParams = {
    .startMode = TimerP_StartMode_AUTO,
    .arg = 0,
    .period = 10,
};


/*
 *  ======== TimerP_Params_init ========
 */
void TimerP_Params_init(TimerP_Params *params)
{
    /* structure copy */
    *params = TimerP_defaultParams;
}

/*
 *  ======== TimerP_construct ========
 */
TimerP_Handle TimerP_construct(TimerP_Struct *handle, TimerP_Fxn timerFxn, TimerP_Params *params)
{
    TimerP_Obj *obj = (TimerP_Obj *)handle;
    uintptr_t hwiKey;

    if (handle != NULL) {
        hwiKey = HwiP_disable();

        if (!TimerP_initialized) {
            HwiP_Params hwiParams;

            HwiP_Params_init(&hwiParams);
            hwiParams.priority = INT_PRI_LEVEL4;
            HwiP_construct(&TimerP_hwiStruct, INT_AON_RTC_COMB,
                           TimerP_dynamicStub, &hwiParams);

            TimerP_handle = (TimerP_Obj *)handle;

            TimerP_initDevice(TimerP_handle);

            TimerP_initialized = true;
        }

        HwiP_restore(hwiKey);

        if (params == NULL) {
            params = (TimerP_Params *)&TimerP_defaultParams;
        }

        obj->startMode = params->startMode;
        obj->arg = params->arg;
        obj->tickFxn = timerFxn;
        obj->frequency.lo = TIMER_FREQ;
        obj->frequency.hi = 0;
        obj->period = (0x100000000UL * params->period) / 1000000U;
        obj->period64 = obj->period;
        obj->savedCurrCount = 0;
        obj->prevThreshold = obj->period;
        obj->nextThreshold = obj->period;

        TimerP_startup();
    }

    return ((TimerP_Handle)handle);
}

/*
 *  ======== TimerP_create ========
 */
TimerP_Handle TimerP_create(TimerP_Fxn timerFxn, TimerP_Params *params)
{
    TimerP_Handle handle;

    handle = (TimerP_Handle)malloc(sizeof(TimerP_Obj));

    /* TimerP_construct will check handle for NULL, no need here */
    handle = TimerP_construct((TimerP_Struct *)handle, timerFxn, params);

    return (handle);
}

/*
 *  ======== TimerP_getMaxTicks ========
 */
uint32_t TimerP_getMaxTicks(TimerP_Handle handle)
{
    TimerP_Obj *obj = (TimerP_Obj *)handle;
    uint32_t ticks;
    uint64_t temp;

    temp = (uint64_t)(MAX_SKIP) / obj->period64;

    /* clip value to Clock tick count limit of 32-bits */
    if (temp > 0xFFFFFFFF) {
        ticks = 0xFFFFFFFF;
    }
    else {
        ticks = (uint32_t)temp;
    }

    return (ticks);
}

/*
 *  ======== TimerP_setThreshold ========
 */
void TimerP_setThreshold(TimerP_Handle handle, uint32_t next, bool wrap)
{
    uint32_t now;

    /* get the current RTC count corresponding to compare window */
    now = AONRTCCurrentCompareValueGet();

    /* else if next is too soon, set at least one RTC tick in future */
    /* assume next never be more than half the maximum 32 bit count value */
    if ((next - now) > (uint32_t)0x80000000) {
        /* now is past next */
        next = now + COMPARE_MARGIN;
    }
    else if ((now + COMPARE_MARGIN - next) < (uint32_t)0x80000000) {
        if (next < now + COMPARE_MARGIN) {
            next = now + COMPARE_MARGIN;
        }
    }

    /* set next compare threshold in RTC */
    AONRTCCompareValueSet(AON_RTC_CH0, next);
}

/*
 *  ======== TimerP_setNextTick ========
 */
void TimerP_setNextTick(TimerP_Handle handle, uint32_t ticks)
{
    TimerP_Obj *obj = (TimerP_Obj *)handle;
    bool wrap = false;
    uint32_t next;
    uint64_t newThreshold;

    /* calculate new 64-bit RTC count for next interrupt */
    newThreshold = obj->savedCurrCount + (uint64_t)ticks * obj->period64;

    /* isolate the new 32-bit compare value to write to RTC */
    next = (uint32_t)(newThreshold >> 16);

    /* set the compare threshold at the RTC */
    TimerP_setThreshold(obj, next, wrap);

    /* save the threshold for next interrupt */
    obj->prevThreshold = newThreshold;
}

/*
 *  ======== TimerP_startup ========
 *  Here after call to main().  Called from BIOS_start().
 */
void TimerP_startup()
{
    TimerP_Obj *obj;

    obj = TimerP_handle;
    /* if timer was statically created/constructed */
    if (obj != NULL) {
        if (obj->startMode == TimerP_StartMode_AUTO) {
            TimerP_start(obj);
        }
    }
}

/*
 *  ======== TimerP_start ========
 *
 * 1. HwiP_disable()
 * 2. Reset the RTC
 * 3. Clear any RTC events
 * 4. Set first compare threshold (per configured period)
 * 5. Enable the compare channel
 * 6. Enable the RTC to start it ticking
 * 7. HwiP_restore()
 *
 */
void TimerP_start(TimerP_Handle handle)
{
    TimerP_Obj *obj = (TimerP_Obj *)handle;
    uint32_t compare;
    uintptr_t key;

    key = HwiP_disable();

    /* reset timer */
    AONRTCReset();
    AONRTCEventClear(AON_RTC_CH0);
    IntPendClear(INT_AON_RTC_COMB);

    /*
     * set the compare register to the counter start value plus one period.
     * For a very small period round up to interrupt upon 4th RTC tick
     */
    if (obj->period < 0x40000) {
        compare = 0x4;    /* 4 * 15.5us ~= 62us */
    }
    /* else, interrupt on first period expiration */
    else {
        compare = obj->period >> 16;
    }

    /* set the compare value at the RTC */
    AONRTCCompareValueSet(AON_RTC_CH0, compare);

    /* enable compare channel 0 */
    AONEventMcuWakeUpSet(AON_EVENT_MCU_WU0, AON_EVENT_RTC0);
    AONRTCChannelEnable(AON_RTC_CH0);
    AONRTCCombinedEventConfig(AON_RTC_CH0);

    /* start timer */
    AONRTCEnable();

    HwiP_restore(key);
}

/*
 *  ======== TimerP_stop ========
 */
void TimerP_stop(TimerP_Handle handle)
{
    TimerP_Obj *obj = (TimerP_Obj *)handle;
    /* not implemented for this timer */
    obj->savedCurrCount = 0;
}

/*
 *  ======== TimerP_setPeriod ========
 *
 * 1. Stop timer
 * 2. Set period value in timer obj
 *
 */
void TimerP_setPeriod(TimerP_Handle handle, uint32_t period)
{
    TimerP_Obj *obj = (TimerP_Obj *)handle;
    obj->period = period;
}

/*
 *  ======== TimerP_getCount64 ========
 */
uint64_t TimerP_getCount64(TimerP_Handle handle)
{
    return(AONRTCCurrent64BitValueGet());
}

/*
 *  ======== TimerP_getFreq ========
 */
void TimerP_getFreq(TimerP_Handle handle, TimerP_FreqHz *freq)
{
    freq->lo = TIMER_FREQ;
    freq->hi = 0;
}

/*
 *  ======== TimerP_dynamicStub ========
 */
void TimerP_dynamicStub(uintptr_t arg)
{
    TimerP_Obj *obj;

    obj = TimerP_handle;

    /* clear the RTC event */
    AONRTCEventClear(AON_RTC_CH0);

    /* call the tick function */
    obj->tickFxn(obj->arg);
}

/*
 *  ======== TimerP_getCurrentTick ========
 *  Used by the Clock module for TickMode_DYNAMIC to query the corresponding
 *  Clock tick, as derived from the current timer count.
 */
uint32_t TimerP_getCurrentTick(TimerP_Handle handle, bool saveFlag)
{
    TimerP_Obj *obj = (TimerP_Obj *)handle;
    uint64_t tick, currCount;

    currCount = (uint64_t)TimerP_getCount64(obj);

    tick = currCount / obj->period64;

    if (saveFlag != 0) {
        /*
         * to avoid accumulating drift, make currCount be an integer
         * multiple of timer periods
         */
        currCount = tick * obj->period64;
        obj->savedCurrCount = currCount;
    }

    return ((uint32_t)tick);
}

/* ======== TimerP_initDevice ========
 * Disable and reset the RTC.
 */
void TimerP_initDevice(TimerP_Handle handle)
{
    AONRTCDisable();
    AONRTCReset();

    HWREG(AON_RTC_BASE + AON_RTC_O_SYNC) = 1;
    /* read sync register to complete reset */
    HWREG(AON_RTC_BASE + AON_RTC_O_SYNC);

    AONRTCEventClear(AON_RTC_CH0);
    IntPendClear(INT_AON_RTC_COMB);

    HWREG(AON_RTC_BASE + AON_RTC_O_SYNC);
}
