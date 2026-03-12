/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
/**
 * @ingroup     pkg_bplib_storage
 * 
 * @{
 *
 * @file
 * @brief       Void / no storage.
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "bplib.h"

BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* Inst)
{
    (void) Inst;

    return BPLIB_SUCCESS;
}

void BPLib_STOR_Destroy(BPLib_Instance_t* Inst)
{
    (void) Inst;
    return;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle)
{
    BPLib_MEM_BundleFree(&Inst->pool, Bundle);
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
    BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);
    BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_INFORMATION,
        "Bunde Storage failed.");
    return BPLIB_OS_ERROR;
}

BPLib_Status_t BPLib_STOR_EgressForID(BPLib_Instance_t* Inst, uint32_t EgressID,
    bool LocalDelivery, size_t* NumEgressed)
{
    (void) Inst;
    (void) EgressID,
    (void) LocalDelivery;
    *NumEgressed = 0;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_FlushPending(BPLib_Instance_t* Inst)
{
    (void) Inst;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t* Inst)
{
    (void) Inst;
    return BPLIB_SUCCESS;
}

void BPLib_STOR_UpdateHkPkt(BPLib_Instance_t* Inst)
{
    (void) Inst;
    return;
}

BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *TblData)
{
    (void) TblData;
    return BPLIB_SUCCESS;
}