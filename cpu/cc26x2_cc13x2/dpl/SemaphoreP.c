/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
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
 *  ======== SemaphoreP_nortos.c ========
 */

#include <stdio.h>
#include <stdlib.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/Power.h>

#define MAXCOUNT 0xffff


typedef struct _SemaphoreP_Obj {
    SemaphoreP_Params params;
    uint32_t maxCount;
    volatile uint32_t count;
} SemaphoreP_Obj;

static bool pendInProgress = false;
static bool doEnablePolicy = false;
static bool SemaphoreP_initialized = false;

SemaphoreP_Params SemaphoreP_defaultParams = {
    .mode = SemaphoreP_Mode_COUNTING,
    .callback = Power_idleFunc,
};

extern void ClockP_startup(void);
extern void ClockP_add(ClockP_Struct *handle, ClockP_Fxn fxn,
                       uint32_t timeout, uintptr_t arg);

/*
 * ClockP objects need a fxn, but we don't need to do anything in the fxn
 * since we just use the Clock_isActive status to determine if it's still
 * running or not.
 */
void clkFxn(uintptr_t arg)
{
}

/*
 *  ======== SemaphoreP_construct ========
 */
SemaphoreP_Handle SemaphoreP_construct(SemaphoreP_Struct *handle,
        unsigned int count, SemaphoreP_Params *params)
{
    SemaphoreP_Obj *pSemaphore = (SemaphoreP_Obj *)handle;

    if (!SemaphoreP_initialized) {
        ClockP_startup();

        SemaphoreP_initialized = true;
    }

    if (handle != NULL) {
        if (params == NULL) {
            params = &(pSemaphore->params);
            SemaphoreP_Params_init(params);
        }
        else {
            pSemaphore->params.mode = params->mode;
            pSemaphore->params.callback = params->callback;
        }

        /* check if the semaphore is binary or counting */
        if (params->mode == SemaphoreP_Mode_COUNTING) {
            pSemaphore->count = count;
            pSemaphore->maxCount = MAXCOUNT;
        }
        else {
            pSemaphore->maxCount = 1;
            if (count != 0) {
                pSemaphore->count = 1;
            }
            else {
                pSemaphore->count = 0;
            }
        }
    }

    return ((SemaphoreP_Handle)handle);
}

/*
 *  ======== SemaphoreP_constructBinary ========
 */
SemaphoreP_Handle SemaphoreP_constructBinary(SemaphoreP_Struct *handle,
        unsigned int count)
{
    SemaphoreP_Params params;

    SemaphoreP_Params_init(&params);
    params.mode = SemaphoreP_Mode_BINARY;

    return (SemaphoreP_construct(handle, count, &params));
}

/*
 *  ======== SemaphoreP_create ========
 */
SemaphoreP_Handle SemaphoreP_create(unsigned int count,
        SemaphoreP_Params *params)
{
    SemaphoreP_Handle handle;

    handle = (SemaphoreP_Handle)malloc(sizeof(SemaphoreP_Obj));

    /* SemaphoreP_construct will check handle for NULL, no need here */
    handle = SemaphoreP_construct((SemaphoreP_Struct *)handle, count, params);

    return (handle);
}

/*
 *  ======== SemaphoreP_createBinary ========
 */
SemaphoreP_Handle SemaphoreP_createBinary(unsigned int count)
{
    SemaphoreP_Params params;

    SemaphoreP_Params_init(&params);
    params.mode = SemaphoreP_Mode_BINARY;

    return (SemaphoreP_create(count, &params));
}

/*
 *  ======== SemaphoreP_createBinaryCallback ========
 */
SemaphoreP_Handle SemaphoreP_createBinaryCallback(unsigned int count,
        void (*callback)(void))
{
    SemaphoreP_Params params;

    SemaphoreP_Params_init(&params);
    params.mode = SemaphoreP_Mode_BINARY;
    params.callback = callback;

    return (SemaphoreP_create(count, &params));
}

/*
 *  ======== SemaphoreP_destruct ========
 */
void SemaphoreP_destruct(SemaphoreP_Struct *handle)
{
}

/*
 *  ======== SemaphoreP_delete ========
 */
void SemaphoreP_delete(SemaphoreP_Handle handle)
{
    SemaphoreP_destruct((SemaphoreP_Struct *)handle);

    free(handle);
}

/*
 *  ======== SemaphoreP_Params_init ========
 */
void SemaphoreP_Params_init(SemaphoreP_Params *params)
{
    *params = SemaphoreP_defaultParams;
}

/*
 *  ======== SemaphoreP_pend ========
 */
SemaphoreP_Status SemaphoreP_pend(SemaphoreP_Handle handle, uint32_t timeout)
{
    SemaphoreP_Obj    *pSemaphore = (SemaphoreP_Obj *)handle;
    SemaphoreP_Status  status;
    ClockP_Struct      clockStruct;
    uintptr_t          key;

    /*
     * Always add Clock (but don't start) so that ClockP_isActive() below
     * is valid.  It's OK to add a Clock even when timeout is 0 or forever
     * (but it is not OK to start it).
     */
    ClockP_add(&clockStruct, clkFxn, timeout, (uintptr_t)NULL);

    if ((timeout != 0) && (timeout != SemaphoreP_WAIT_FOREVER)) {
        ClockP_start((ClockP_Handle)&clockStruct);
    }

    key = HwiP_disable();

    pendInProgress = true;

    while ((pSemaphore->count == 0) &&
           ((timeout == SemaphoreP_WAIT_FOREVER) ||
            ClockP_isActive((ClockP_Handle)&clockStruct))) {

        HwiP_restore(key);

        /* Call the registered callback */
        if (pSemaphore->params.callback != NULL) {
            pSemaphore->params.callback();
        }

        if (doEnablePolicy) {
            Power_enablePolicy();
            doEnablePolicy = false;
        }

        key = HwiP_disable();
    }

    pendInProgress = false;

    if (pSemaphore->count > 0) {
        (pSemaphore->count)--;
        status = SemaphoreP_OK;
    }
    else {
        status = SemaphoreP_TIMEOUT;
    }

    HwiP_restore(key);

    ClockP_destruct(&clockStruct);

    return (status);
}

/*
 *  ======== SemaphoreP_post ========
 */
void SemaphoreP_post(SemaphoreP_Handle handle)
{
    SemaphoreP_Obj *pSemaphore = (SemaphoreP_Obj *)handle;
    uintptr_t       key;

    key = HwiP_disable();

    if (pSemaphore->count < pSemaphore->maxCount) {
        (pSemaphore->count)++;
    }

    if (HwiP_inISR() && pendInProgress && !doEnablePolicy) {
        /* short-circuit potential policy invocation */
        doEnablePolicy = Power_disablePolicy();
    }

    HwiP_restore(key);
}

/*
 *  ======== SemaphoreP_staticObjectSize ========
 *  Internal function used by DPL tests to check that SemaphoreP_Struct
 *  is large enough to hold OS SemaphoreP object.
 */
size_t SemaphoreP_staticObjectSize(void)
{
    return (sizeof(SemaphoreP_Obj));
}
