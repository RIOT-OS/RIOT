/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on bplib's example implementation in [bplib]/app/src/bpcat_fwp.c
 */
#include "bplib_riot_fwp.h"
#include "bplib.h"

/* Signal the vfs based storage when a contact / channel terminates to flush the caches */
#if MODULE_BPLIB_STOR_VFS
#  include "bplib_stor_vfs.h"
#endif

/* If walltime is provided to be set to an accurate value. */
#if MODULE_BPLIB_WALLTIME_AVAILABLE
#  include "walltime.h"
#endif

#include "ztimer64.h"
#include <stdint.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static int64_t BPA_TIMEP_GetMonotonicTime(void)
{
    return ztimer64_now(ZTIMER64_MSEC);
}

static int64_t BPA_TIMEP_GetHostTime(void)
{
#if MODULE_BPLIB_WALLTIME_AVAILABLE
        uint16_t msec;
        uint32_t sec = walltime_get_riot(&msec);
        return (int64_t)(sec) * 1000 + (int64_t)(msec);
#else
        return 0;
#endif
}

static void BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch)
{
    if (Epoch != NULL)
    {
        /* RIOT Epoch: 2020/01/01, 00:00:00 UTC */
        Epoch->Year   = 2020;
        Epoch->Day    = 1;
        Epoch->Hour   = 1;
        Epoch->Minute = 0;
        Epoch->Second = 0;
        Epoch->Msec   = 0;
    }
}

static BPLib_TIME_ClockState_t BPA_TIMEP_GetHostClockState(void)
{
    /* Assume valid if walltime is there */
    if (IS_USED(MODULE_BPLIB_WALLTIME_AVAILABLE)) {
        return BPLIB_TIME_CLOCK_VALID;
    }
    else {
        return BPLIB_TIME_CLOCK_INVALID;
    }
}

static void BPA_PERFLOGP_Entry(uint32_t PerfLogID)
{
    (void) PerfLogID;
    return;
}

static void BPA_PERFLOGP_Exit(uint32_t PerfLogID)
{
    (void) PerfLogID;
    return;
}

static BPLib_Status_t BPA_TABLEP_TableUpdate(uint8_t TableType, void** TblPtr)
{
    (void) TableType;
    (void) TblPtr;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_EVP_Init(void)
{
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_EVP_SendEvent(uint16_t EventID,
    BPLib_EM_EventType_t EventType, char const* Spec)
{
    DEBUG("Event Type: %d, Event ID: %d, Event Text: %s\n", EventID, EventType, Spec);
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibConfigPkt(
    BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload)
{
    (void) NodeMIBConfigTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendPerSourceMibConfigPkt(
    BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload)
{
    (void) SrcMIBConfigTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibReportsPkt(
    BPLib_NodeMibReportsHkTlm_Payload_t* NodeMIBReportsTlmPayload)
{
    (void) NodeMIBReportsTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibCounterPkt(
    BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload)
{
    (void) NodeMIBCounterTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendPerSourceMibCounterPkt(
    BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload)
{
    (void) SrcMIBCounterTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendChannelContactPkt(
    BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload)
{
    (void) ChannelContactTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendStoragePkt(
    BPLib_StorageHkTlm_Payload_t* StorTlmPayload)
{
    (void) StorTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_AddApplication(uint32_t ChanId)
{
    (void) ChanId;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_StartApplication(uint32_t ChanId)
{
    (void) ChanId;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_StopApplication(uint32_t ChanId)
{
    (void) ChanId;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_ADUP_RemoveApplication(uint32_t ChanId)
{
    if (IS_USED(MODULE_BPLIB_STOR_VFS)) {
        bplib_stor_vfs_channel_changed(ChanId);
    }
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_CLAP_ContactSetup(uint32_t ContactId, BPLib_CLA_ContactsSet_t ContactInfo)
{
    (void) ContactId;
    (void) ContactInfo;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_CLAP_ContactStart(uint32_t ContactId)
{
    (void) ContactId;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_CLAP_ContactStop(uint32_t ContactId)
{
    (void) ContactId;
    return BPLIB_SUCCESS;
}

static void BPA_CLAP_ContactTeardown(uint32_t ContactId)
{
    if (IS_USED(MODULE_BPLIB_STOR_VFS)) {
        bplib_stor_vfs_contact_changed(ContactId);
    }
    return;
}

static BPLib_FWP_ProxyCallbacks_t Callbacks = {
    /* Time Proxy */
    .BPA_TIMEP_GetMonotonicTime = BPA_TIMEP_GetMonotonicTime,
    .BPA_TIMEP_GetHostEpoch = BPA_TIMEP_GetHostEpoch,
    .BPA_TIMEP_GetHostClockState = BPA_TIMEP_GetHostClockState,
    .BPA_TIMEP_GetHostTime = BPA_TIMEP_GetHostTime,

    /* Perf Log Proxy */
    .BPA_PERFLOGP_Entry = BPA_PERFLOGP_Entry,
    .BPA_PERFLOGP_Exit = BPA_PERFLOGP_Exit,

    /* Table Proxy */
    .BPA_TABLEP_TableUpdate = BPA_TABLEP_TableUpdate,

    /* Event Proxy */
    .BPA_EVP_Init = BPA_EVP_Init,
    .BPA_EVP_SendEvent = BPA_EVP_SendEvent,

    /* ADU Proxy */
    .BPA_ADUP_AddApplication = BPA_ADUP_AddApplication,
    .BPA_ADUP_StartApplication = BPA_ADUP_StartApplication,
    .BPA_ADUP_StopApplication = BPA_ADUP_StopApplication,
    .BPA_ADUP_RemoveApplication = BPA_ADUP_RemoveApplication,

    /* Telemetry Proxy */
    .BPA_TLMP_SendNodeMibConfigPkt = BPA_TLMP_SendNodeMibConfigPkt,
    .BPA_TLMP_SendPerSourceMibConfigPkt = BPA_TLMP_SendPerSourceMibConfigPkt,
    .BPA_TLMP_SendNodeMibReportsPkt = BPA_TLMP_SendNodeMibReportsPkt,
    .BPA_TLMP_SendNodeMibCounterPkt = BPA_TLMP_SendNodeMibCounterPkt,
    .BPA_TLMP_SendPerSourceMibCounterPkt = BPA_TLMP_SendPerSourceMibCounterPkt,
    .BPA_TLMP_SendChannelContactPkt = BPA_TLMP_SendChannelContactPkt,
    .BPA_TLMP_SendStoragePkt = BPA_TLMP_SendStoragePkt,

    /* CLA Proxy */
    .BPA_CLAP_ContactSetup = BPA_CLAP_ContactSetup,
    .BPA_CLAP_ContactStart = BPA_CLAP_ContactStart,
    .BPA_CLAP_ContactStop = BPA_CLAP_ContactStop,
    .BPA_CLAP_ContactTeardown = BPA_CLAP_ContactTeardown,
};

BPLib_Status_t bplib_riot_fwp_init(void)
{
    return BPLib_FWP_Init(&Callbacks);
}
