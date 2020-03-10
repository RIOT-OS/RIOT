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
 *  ======== TimerP.h ========
 */

typedef void *TimerP_Handle;
typedef void (*TimerP_Fxn)(uintptr_t arg);

#define TimerP_STRUCT_SIZE 60

typedef enum {
    TimerP_Status_INUSE,
    TimerP_Status_FREE,
} TimerP_Status;

typedef enum {
    TimerP_StartMode_AUTO,
    TimerP_StartMode_USER,
} TimerP_StartMode;

typedef struct _TimerP_FreqHz {
    uint32_t hi;
    uint32_t lo;
} TimerP_FreqHz;

typedef struct _TimerP_Params {
    TimerP_StartMode startMode;
    uintptr_t arg;
    uint32_t period;    /* in microseconds */
} TimerP_Params;

typedef union _TimerP_Struct {
    uint64_t dummy;
    char data[TimerP_STRUCT_SIZE];
} TimerP_Struct;

void TimerP_Params_init(TimerP_Params *params);
TimerP_Handle TimerP_construct(TimerP_Struct *handle, TimerP_Fxn timerFxn, TimerP_Params *params);
TimerP_Handle TimerP_create(TimerP_Fxn timerFxn, TimerP_Params *params);
void TimerP_getFreq(TimerP_Handle handle, TimerP_FreqHz *freq);
uint32_t TimerP_getMaxTicks(TimerP_Handle handle);
void TimerP_setThreshold(TimerP_Handle handle, uint32_t next, bool wrap);
void TimerP_setNextTick(TimerP_Handle handle, uint32_t ticks);
void TimerP_startup();
void TimerP_start(TimerP_Handle handle);
void TimerP_stop(TimerP_Handle handle);
void TimerP_setPeriod(TimerP_Handle handle, uint32_t period);
uint64_t TimerP_getCount64(TimerP_Handle handle);
void TimerP_dynamicStub(uintptr_t arg);
uint32_t TimerP_getCurrentTick(TimerP_Handle handle, bool saveFlag);
void TimerP_initDevice(TimerP_Handle handle);
