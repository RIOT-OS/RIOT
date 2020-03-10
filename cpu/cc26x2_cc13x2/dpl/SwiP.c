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
 *  ======== SwiP_nortos.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SwiP.h>

#include "QueueP.h"

/* Lowest priority interrupt */
#define INT_PRI_LEVEL7 0xe0

#define NUMPRI 4

typedef enum {
    SwiP_State_Idle = 0,
    SwiP_State_Posted,
    SwiP_State_Running,
    SwiP_State_Interrupted
} SwiP_State;

typedef uint32_t SwiP_LockState;

enum {
    SwiP_LockState_Locked,
    SwiP_LockState_Unlocked
};

typedef struct SwiP_Obj {
    QueueP_Elem    elem;
    QueueP_Handle  readyList;
    SwiP_Params    params;
    SwiP_Fxn       fxn;
    uint32_t       trigger;
    uint32_t       state;
} SwiP_Obj;


extern bool HwiP_inSwi(void);

static void SwiP_handleHwi();

static const SwiP_Params SwiP_defaultParams = {
    .arg0 = (uintptr_t) NULL,
    .arg1 = (uintptr_t) NULL,
    .priority = ~0,  /* max priority */
    .trigger = 0,
};

static volatile int SwiP_readyMask;
static volatile SwiP_LockState SwiP_lockState;
static volatile uint32_t SwiP_currentTrigger;
static volatile bool SwiP_schedulerRunning;
static volatile bool SwiP_initialized = false;
static HwiP_Struct SwiP_hwiStruct;
static QueueP_Obj SwiP_readyList[NUMPRI];

/* don't call with n == 0 */
static int maxbit(int n)
{
    int mask = 1 << (NUMPRI - 1);
    int max = NUMPRI - 1;

    while (mask) {
        if (n & mask) {
            return max;
        }
        max--;
        mask >>= 1;
    }

    return 0;
}

/*
 *  ======== SwiP_Params_init ========
 */
void SwiP_Params_init(SwiP_Params *params)
{
    /* structure copy */
    *params = SwiP_defaultParams;
}

/*
 *  ======== SwiP_construct ========
 */
SwiP_Handle SwiP_construct(SwiP_Struct *handle, SwiP_Fxn swiFxn,
    SwiP_Params *params)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;
    HwiP_Params hwiParams;
    uintptr_t hwiKey;
    uint32_t priority;
    int i;

    if (handle != NULL) {
        hwiKey = HwiP_disable();

        if (SwiP_initialized == false) {
            for (i = 0; i < NUMPRI; i++) {
                QueueP_init(&SwiP_readyList[i]);
            }
            SwiP_readyMask = 0;
            SwiP_currentTrigger = 0;
            SwiP_lockState = SwiP_LockState_Unlocked;
            SwiP_schedulerRunning = false;

            HwiP_Params_init(&hwiParams);
            hwiParams.priority = INT_PRI_LEVEL7;  // use the lowest priority
            HwiP_construct(&SwiP_hwiStruct, HwiP_swiPIntNum, SwiP_handleHwi,
                           &hwiParams);

            SwiP_initialized = true;
        }

        HwiP_restore(hwiKey);

        if (params == NULL) {
            params = (SwiP_Params *)&SwiP_defaultParams;
        }

        if (params->priority == (~0)) {
            priority = NUMPRI - 1;
        }
        else {
            priority = params->priority;
        }

        if (priority >= NUMPRI) {
            return NULL;
        }
        else {
            QueueP_init((QueueP_Obj *)&swi->elem);
            swi->params = *params;
            swi->params.priority = priority;
            swi->fxn = swiFxn;
            swi->trigger = swi->params.trigger;
            swi->state = SwiP_State_Idle;
            swi->readyList = &SwiP_readyList[priority];
        }
    }

    return ((SwiP_Handle)swi);
}

/*
 *  ======== SwiP_create ========
 */
SwiP_Handle SwiP_create(SwiP_Fxn swiFxn, SwiP_Params *params)
{
    SwiP_Handle handle;
    SwiP_Handle retHandle;

    handle = (SwiP_Handle)malloc(sizeof(SwiP_Obj));

    /*
     * Even though SwiP_construct will check handle for NULL and not do
     * anything, we should check it here so that we can know afterwards
     * that construct failed with non-NULL pointer and that we need to
     * free the handle.
     */
    if (handle != NULL) {
        retHandle = SwiP_construct((SwiP_Struct *)handle, swiFxn, params);
        if (retHandle == NULL) {
            free(handle);
            handle = NULL;
        }
    }

    return (handle);
}

/*
 *  ======== SwiP_destruct ========
 */
void SwiP_destruct(SwiP_Struct *handle)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;
    uintptr_t hwiKey = HwiP_disable();

    /* if on SwiP_readyList, remove it */
    QueueP_remove(&swi->elem);
    if (QueueP_empty(swi->readyList)) {
        SwiP_readyMask &= ~(1 << swi->params.priority);
    }

    HwiP_restore(hwiKey);
}

/*
 *  ======== SwiP_delete ========
 */
void SwiP_delete(SwiP_Handle handle)
{
    SwiP_destruct((SwiP_Struct *)handle);

    free(handle);
}

/*
 *  ======== SwiP_disable ========
 */
uintptr_t SwiP_disable(void)
{
    uintptr_t previousHwiState = HwiP_disable();

    SwiP_LockState previousLockState = SwiP_lockState;
    SwiP_lockState = SwiP_LockState_Locked;

    HwiP_restore(previousHwiState);

    return previousLockState;
}

/*
 * This is a non-preemptive fixed-priority scheduler implementation.
 * It runs with interrupts disabled, but enables them for each swi.
 */
void SwiP_dispatch(uintptr_t hwiKey)
{
    SwiP_Obj *swi;
    int maxpri;

    while (SwiP_readyMask && (SwiP_lockState == SwiP_LockState_Unlocked)) {
        maxpri = maxbit(SwiP_readyMask);
        swi = (SwiP_Obj *)QueueP_get(&SwiP_readyList[maxpri]);

        if (QueueP_empty(&SwiP_readyList[maxpri])) {
            SwiP_readyMask &= ~(1 << maxpri);
        }

        /*
         * Prepare the swi for execution.  The trigger has to be saved
         * because the swi might re-post itself with another trigger value.
         */
        swi->state = SwiP_State_Running;
        SwiP_currentTrigger = swi->trigger;
        swi->trigger = swi->params.trigger;

        /* run the swi with interrupts enabled */
        HwiP_restore(hwiKey);
        swi->fxn(swi->params.arg0, swi->params.arg1);
        hwiKey = HwiP_disable();

        /* If the swi didn't get re-posted, set it to idle now */
        if (swi->state == SwiP_State_Running) {
            swi->state = SwiP_State_Idle;
        }
    }

    /* Scheduler was set to running immediately after posting the interrupt */
    SwiP_schedulerRunning = false;
}

static void SwiP_handleHwi()
{
    uintptr_t hwiKey = HwiP_disable();

    SwiP_dispatch(hwiKey);

    HwiP_restore(hwiKey);
}

/*
 *  ======== SwiP_getTrigger ========
 */
uint32_t SwiP_getTrigger()
{
    return (SwiP_currentTrigger);
}

/*
 *  ======== SwiP_andn ========
 */
void SwiP_andn(SwiP_Handle handle, uint32_t mask)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;
    uintptr_t hwiKey = HwiP_disable();

    if (swi->trigger != 0) {
        swi->trigger &= ~mask;
        if (swi->trigger == 0) {
            HwiP_restore(hwiKey);
            SwiP_post(swi);

            return;
        }
    }

    HwiP_restore(hwiKey);
}

/*
 *  ======== SwiP_dec ========
 */
void SwiP_dec(SwiP_Handle handle)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;
    uintptr_t hwiKey = HwiP_disable();

    if (swi->trigger != 0) {
        swi->trigger -= 1;
        if (swi->trigger == 0) {
            HwiP_restore(hwiKey);
            SwiP_post(swi);

            return;
        }
    }

    HwiP_restore(hwiKey);
}

/*
 *  ======== SwiP_inc ========
 */
void SwiP_inc(SwiP_Handle handle)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;
    uintptr_t hwiKey = HwiP_disable();

    swi->trigger += 1;

    HwiP_restore(hwiKey);
    SwiP_post(swi);
}


/*
 *  ======== SwiP_inISR ========
 */
bool SwiP_inISR(void)
{
    return (HwiP_inSwi());
}

/*
 *  ======== SwiP_or ========
 */
void SwiP_or(SwiP_Handle handle, uint32_t mask)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;
    uintptr_t hwiKey = HwiP_disable();

    swi->trigger |= mask;

    HwiP_restore(hwiKey);
    SwiP_post(swi);
}

/*
 *  ======== SwiP_post ========
 */
void SwiP_post(SwiP_Handle handle)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;
    uintptr_t hwiKey = HwiP_disable();

    /* (Re-)post an swi only once */
    if (swi->state != SwiP_State_Posted) {
        swi->state = SwiP_State_Posted;

        QueueP_put(&SwiP_readyList[swi->params.priority],
                   (QueueP_Elem *)&swi->elem);
        SwiP_readyMask |= 1 << swi->params.priority;
    }

    /* Activate the scheduler when not already running */
    if ((SwiP_schedulerRunning == false) &&
        (SwiP_lockState == SwiP_LockState_Unlocked)) {
        HwiP_post(HwiP_swiPIntNum);
        /* Set the scheduler into running state to avoid double posts */
        SwiP_schedulerRunning = true;
    }

    HwiP_restore(hwiKey);
}

/*
 *  ======== SwiP_restore ========
 */
void SwiP_restore(uintptr_t key)
{
    uintptr_t hwiKey = HwiP_disable();

    SwiP_lockState = key;

    /* Determine whether the scheduler needs to run */
    if (SwiP_readyMask && (key == SwiP_LockState_Unlocked) &&
        (SwiP_schedulerRunning == false)) {
        HwiP_post(HwiP_swiPIntNum);
        /* Set the scheduler into running state to avoid double posts */
        SwiP_schedulerRunning = true;
    }

    HwiP_restore(hwiKey);
}

/*
 *  ======== SwiP_setPriority ========
 */
void SwiP_setPriority(SwiP_Handle handle, uint32_t priority)
{
    SwiP_Obj *swi = (SwiP_Obj *)handle;

    if (priority == (~0)) {
        priority = NUMPRI - 1;    /* map ~0 to maxpri */
    }

    if (priority >= NUMPRI) {
        return;    /* noop, don't change the priority to bogus value */
    }

    swi->params.priority = priority;

    swi->readyList = &SwiP_readyList[priority];
}

/*
 *  ======== SwiP_staticObjectSize ========
 */
size_t SwiP_staticObjectSize(void)
{
    return (sizeof(SwiP_Obj));
}
