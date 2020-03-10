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
 *  ======== MutexP_nortos.c ========
 */

#include <stdio.h>
#include <stdlib.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/MutexP.h>

typedef struct _MutexP_Config {
    uint32_t dummy;
} MutexP_Config;

/*
 *  ======== MutexP_construct ========
 */
MutexP_Handle MutexP_construct(MutexP_Struct *handle, MutexP_Params *params)
{
    return ((MutexP_Handle)handle);
}

/*
 *  ======== MutexP_create ========
 */
MutexP_Handle MutexP_create(MutexP_Params *params)
{
    MutexP_Config *pMutex;

    //dynamically allocating memory for the mutex struct
    pMutex = (MutexP_Config *)malloc(sizeof(MutexP_Config));

    pMutex = (MutexP_Config *)MutexP_construct((MutexP_Struct *)pMutex, params);

    return ((MutexP_Handle)pMutex);
}

/*
 *  ======== MutexP_destruct ========
 */
void MutexP_destruct(MutexP_Struct *handle)
{
}

/*
 *  ======== MutexP_delete ========
 */
void MutexP_delete(MutexP_Handle handle)
{
    if (handle != NULL) {
        MutexP_destruct((MutexP_Struct *)handle);
        free(handle);
    }
}

/*
 *  ======== MutexP_lock ========
 */
uintptr_t MutexP_lock(MutexP_Handle handle)
{
    return (0);
}

/*
 *  ======== MutexP_Params_init ========
 */
void MutexP_Params_init(MutexP_Params *params)
{
    return;
}

/*
 *  ======== MutexP_unlock ========
 */
void MutexP_unlock(MutexP_Handle handle, uintptr_t key)
{
    return;
}
