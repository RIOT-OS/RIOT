/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */
#include "bplib_riot_fwp.h"
#include "bplib.h"

#if MODULE_BPLIB_STOR_VFS
#include "bplib_stor_vfs.h"
#endif

#if MODULE_BPLIB_CLA_UDP
#include "bplib_cla_udp.h"
#endif

#include "ztimer64.h"
#include "walltime.h"

// TODO check includes
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>

static int64_t BPA_TIMEP_GetMonotonicTime(void)
{
    return ztimer64_now(ZTIMER64_MSEC);
}

static int64_t BPA_TIMEP_GetHostTime(void)
{
    uint16_t msec;
    uint32_t sec = walltime_get_riot(&msec);

    return (int64_t)(sec) * 1000 + (int64_t)(msec);
}

static void BPA_TIMEP_GetHostEpoch(BPLib_TIME_Epoch_t *Epoch)
{
    if (Epoch != NULL)
    {
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
    return BPLIB_TIME_CLOCK_VALID;
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

static BPLib_Status_t BPA_EVP_SendEvent(uint16_t EventID, BPLib_EM_EventType_t EventType, char const* Spec)
{
    printf("Event Type: %d, Event ID: %d, Event Text: %s\n", EventID, EventType, Spec);
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibConfigPkt(BPLib_NodeMibConfigHkTlm_Payload_t* NodeMIBConfigTlmPayload)
{
    (void) NodeMIBConfigTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendPerSourceMibConfigPkt(BPLib_SourceMibConfigHkTlm_Payload_t* SrcMIBConfigTlmPayload)
{
    (void) SrcMIBConfigTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibReportsPkt(BPLib_NodeMibReportsHkTlm_Payload_t* NodeMIBReportsTlmPayload)
{
    (void) NodeMIBReportsTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendNodeMibCounterPkt(BPLib_NodeMibCountersHkTlm_Payload_t* NodeMIBCounterTlmPayload)
{
    (void) NodeMIBCounterTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendPerSourceMibCounterPkt(BPLib_SourceMibCountersHkTlm_Payload_t* SrcMIBCounterTlmPayload)
{
    (void) SrcMIBCounterTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendChannelContactPkt(BPLib_ChannelContactStatHkTlm_Payload_t* ChannelContactTlmPayload)
{
    (void) ChannelContactTlmPayload;
    return BPLIB_SUCCESS;
}

static BPLib_Status_t BPA_TLMP_SendStoragePkt(BPLib_StorageHkTlm_Payload_t* StorTlmPayload)
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
