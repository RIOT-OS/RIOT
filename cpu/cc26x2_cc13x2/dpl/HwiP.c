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
 *  ======== HwiPCC26XX_nortos.c ========
 */

#include <stdio.h>
#include <stdlib.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/DebugP.h>

/* driverlib header files */
#include <inc/hw_types.h>
#include <inc/hw_cpu_scs.h>
#include <inc/hw_ints.h>
#include <driverlib/interrupt.h>
#include <driverlib/rom.h>

typedef struct _HwiP_Obj {
    uint32_t intNum;
    HwiP_Fxn fxn;
    uintptr_t arg;
} HwiP_Obj;

typedef struct _Hwi_NVIC {
    uint32_t RES_00;
    uint32_t ICTR;
    uint32_t RES_08;
    uint32_t RES_0C;
    uint32_t STCSR;
    uint32_t STRVR;
    uint32_t STCVR;
    uint32_t STCALIB;
    uint32_t RES_20[56];
    uint32_t ISER[8];
    uint32_t RES_120[24];
    uint32_t ICER[8];
    uint32_t RES_1A0[24];
    uint32_t ISPR[8];
    uint32_t RES_220[24];
    uint32_t ICPR[8];
    uint32_t RES_2A0[24];
    uint32_t IABR[8];
    uint32_t RES_320[56];
    uint8_t IPR[240];
    uint32_t RES_4F0[516];
    uint32_t CPUIDBR;
    uint32_t ICSR;
    uint32_t VTOR;
    uint32_t AIRCR;
    uint32_t SCR;
    uint32_t CCR;
    uint8_t SHPR[12];
    uint32_t SHCSR;
    uint8_t MMFSR;
    uint8_t BFSR;
    uint16_t UFSR;
    uint32_t HFSR;
    uint32_t DFSR;
    uint32_t MMAR;
    uint32_t BFAR;
    uint32_t AFSR;
    uint32_t PFR0;
    uint32_t PFR1;
    uint32_t DFR0;
    uint32_t AFR0;
    uint32_t MMFR0;
    uint32_t MMFR1;
    uint32_t MMFR2;
    uint32_t MMFR3;
    uint32_t ISAR0;
    uint32_t ISAR1;
    uint32_t ISAR2;
    uint32_t ISAR3;
    uint32_t ISAR4;
    uint32_t RES_D74[5];
    uint32_t CPACR;
    uint32_t RES_D8C[93];
    uint32_t STI;
    uint32_t RES_F04[12];
    uint32_t FPCCR;
    uint32_t FPCAR;
    uint32_t FPDSCR;
    uint32_t MVFR0;
    uint32_t MVFR1;
    uint32_t RES_F48[34];
    uint32_t PID4;
    uint32_t PID5;
    uint32_t PID6;
    uint32_t PID7;
    uint32_t PID0;
    uint32_t PID1;
    uint32_t PID2;
    uint32_t PID3;
    uint32_t CID0;
    uint32_t CID1;
    uint32_t CID2;
    uint32_t CID3;
} Hwi_NVIC;

static Hwi_NVIC *Hwi_nvic = (Hwi_NVIC *)0xE000E000;

static HwiP_Obj* HwiP_dispatchTable[NUM_INTERRUPTS] = {
    0
};

int HwiP_swiPIntNum = INT_PENDSV;


/*
 *  ======== HwiP_enable ========
 */
void HwiP_enable(void)
{
    IntMasterEnable();
}

/*
 *  ======== HwiP_disable ========
 */
uintptr_t HwiP_disable(void)
{
    return (IntMasterDisable());
}

/*
 *  ======== HwiP_restore ========
 */
void HwiP_restore(uintptr_t alreadyDisabled)
{
    if (!alreadyDisabled) {
        IntMasterEnable();
    }
}

/*
 *  ======== HwiP_clearInterrupt ========
 */
void HwiP_clearInterrupt(int interruptNum)
{
    IntPendClear((uint32_t)interruptNum);
}

/*
 *  ======== HwiP_destruct ========
 */
void HwiP_destruct(HwiP_Struct *handle)
{
    HwiP_Obj *obj = (HwiP_Obj *)handle;

    IntDisable(obj->intNum);
    IntUnregister(obj->intNum);
}

/*
 *  ======== HwiP_delete ========
 */
void HwiP_delete(HwiP_Handle handle)
{
    HwiP_destruct((HwiP_Struct *)handle);

    free(handle);
}

/*
 *  ======== HwiP_disableInterrupt ========
 */
void HwiP_disableInterrupt(int interruptNum)
{
    IntDisable((uint32_t)interruptNum);
}

/*
 *  ======== HwiP_dispatch ========
 */
static void HwiP_dispatch(void)
{
    /* Determine which interrupt has fired */
    uint32_t intNum = (Hwi_nvic->ICSR & 0x000000ff);
    HwiP_Obj* obj = HwiP_dispatchTable[intNum];
    if (obj) {
        (obj->fxn)(obj->arg);
    }
}

/*
 *  ======== HwiP_enableInterrupt ========
 */
void HwiP_enableInterrupt(int interruptNum)
{
    IntEnable((uint32_t)interruptNum);
}

/*
 *  ======== HwiP_construct ========
 */
HwiP_Handle HwiP_construct(HwiP_Struct *handle, int interruptNum,
                           HwiP_Fxn hwiFxn, HwiP_Params *params)
{
    HwiP_Params defaultParams;
    HwiP_Obj *obj = (HwiP_Obj *)handle;

    if (handle != NULL) {
        if (params == NULL) {
            params = &defaultParams;
            HwiP_Params_init(&defaultParams);
        }

        if ((params->priority & 0xFF) == 0xFF) {
            /* SwiP_nortos.c uses INT_PRI_LEVEL7 as its scheduler */
            params->priority = INT_PRI_LEVEL6;
        }

        if (interruptNum != HwiP_swiPIntNum &&
            params->priority == INT_PRI_LEVEL7) {
            DebugP_log0("HwiP_construct: can't use reserved INT_PRI_LEVEL7");

            handle = NULL;
        }
        else {
            HwiP_dispatchTable[interruptNum] = obj;
            obj->fxn = hwiFxn;
            obj->arg = params->arg;
            obj->intNum = (uint32_t)interruptNum;

            IntRegister((uint32_t)interruptNum, HwiP_dispatch);
            IntPrioritySet((uint32_t)interruptNum, (uint8_t)params->priority);

            if (params->enableInt) {
                IntEnable((uint32_t)interruptNum);
            }
        }
    }

    return ((HwiP_Handle)handle);
}

/*
 *  ======== HwiP_create ========
 */
HwiP_Handle HwiP_create(int interruptNum, HwiP_Fxn hwiFxn, HwiP_Params *params)
{
    HwiP_Handle handle;
    HwiP_Handle retHandle;

    handle = (HwiP_Handle)malloc(sizeof(HwiP_Obj));

    /*
     * Even though HwiP_construct will check handle for NULL and not do
     * anything, we should check it here so that we can know afterwards
     * that construct failed with non-NULL pointer and that we need to
     * free the handle.
     */
    if (handle != NULL) {
        retHandle = HwiP_construct((HwiP_Struct *)handle, interruptNum, hwiFxn,
                                   params);
        if (retHandle == NULL) {
            free(handle);
            handle = NULL;
        }
    }

    return (handle);
}

/*
 *  ======== HwiP_Params_init ========
 */
void HwiP_Params_init(HwiP_Params *params)
{
    if (params != NULL) {
        params->arg = 0;
        params->priority = (~0);
        params->enableInt = true;
    }
}

/*
 *  ======== HwiP_plug ========
 */
void HwiP_plug(int interruptNum, void *fxn)
{
    IntRegister((uint32_t)interruptNum, (void (*)(void))fxn);
}

/*
 *  ======== HwiP_setFunc ========
 */
void HwiP_setFunc(HwiP_Handle hwiP, HwiP_Fxn fxn, uintptr_t arg)
{
    HwiP_Obj *obj = (HwiP_Obj *)hwiP;

    uintptr_t key = HwiP_disable();

    obj->fxn = fxn;
    obj->arg = arg;

    HwiP_restore(key);
}

/*
 *  ======== HwiP_post ========
 */
void HwiP_post(int interruptNum)
{
    IntPendSet((uint32_t)interruptNum);
}

/*
 *  ======== HwiP_inISR ========
 */
bool HwiP_inISR(void)
{
    bool stat;

    if ((Hwi_nvic->ICSR & CPU_SCS_ICSR_VECTACTIVE_M) == 0) {
        stat = false;
    }
    else {
        stat = true;
    }

    return (stat);
}

/*
 *  ======== HwiP_inSwi ========
 */
bool HwiP_inSwi(void)
{
    uint32_t intNum  = Hwi_nvic->ICSR & 0x000000ff;
    if (intNum == HwiP_swiPIntNum) {
        /* Currently in a Swi */
        return (true);
    }

    return (false);
}

/*
 * ======== HwiP_setPri ========
 */
void HwiP_setPriority(int interruptNum, uint32_t priority)
{
    IntPrioritySet((uint32_t)interruptNum, (uint8_t)priority);
}

/*
 *  ======== HwiP_staticObjectSize ========
 */
size_t HwiP_staticObjectSize(void)
{
    return (sizeof(HwiP_Obj));
}
