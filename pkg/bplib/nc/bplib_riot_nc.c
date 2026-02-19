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

#include "bplib_riot_nc.h"
#include "bplib.h"

extern BPLib_PI_ChannelTable_t bplib_pi_channeltable;
extern BPLib_CLA_ContactsTable_t bplib_cla_contactstable;

static BPLib_ARP_CRSTable_t        CrsTbl;
static BPLib_PDB_CustodianTable_t  CustodianTbl;
static BPLib_PDB_CustodyTable_t    CustodyTbl;
static BPLib_NC_MibPerNodeConfig_t MibPnTbl = {
    .InstanceEID = {
        .Scheme       = BPLIB_LOCAL_EID_SCHEME,
        .IpnSspFormat = BPLIB_LOCAL_EID_IPN_SSP_FORMAT,
        .Allocator    = BPLIB_LOCAL_EID_ALLOCATOR,
        .Node         = BPLIB_LOCAL_EID_NODE_NUM,
        .Service      = BPLIB_LOCAL_EID_SERVICE_NUM
    },

    .Configs = {
        /* PARAM_BUNDLE_SIZE_NO_FRAGMENT      */ BPLIB_MAX_BUNDLE_LEN,
        /* PARAM_SET_MAX_SEQUENCE NUM         */ 100000,   
        /* PARAM_SET_MAX_PAYLOAD_LENGTH       */ BPLIB_MAX_PAYLOAD_SIZE,
        /* PARAM_SET_MAX_BUNDLE_LENGTH        */ BPLIB_MAX_BUNDLE_LEN,
        /* PARAM_SET_NODE_DTN_TIME            */ 0,
        /* PARAM_SET_BEHAVIOR_EVENT_REPORTING */ 10,
        /* PARAM_SET_MAX_LIFETIME             */ BPLIB_MAX_LIFETIME_ALLOWED
    }
};

static BPLib_NC_MIBConfigPSTable_t MibPsTbl;
static BPLib_PDB_ReportToTable_t   ReportTbl;
static BPLib_PDB_SrcAuthTable_t    AuthTbl;
static BPLib_PDB_SrcLatencyTable_t LatencyTbl;
static BPLib_STOR_StorageTable_t   StorTbl;

BPLib_Status_t bplib_riot_nc_init(BPLib_NC_ConfigPtrs_t* ConfigPtrs)
{
    if (ConfigPtrs == NULL)
    {
        return BPLIB_NULL_PTR_ERROR;
    }

    ConfigPtrs->ChanConfigPtr      = &bplib_pi_channeltable;
    ConfigPtrs->ContactsConfigPtr  = &bplib_cla_contactstable;
    ConfigPtrs->CrsConfigPtr       = &CrsTbl;
    ConfigPtrs->CustodianConfigPtr = &CustodianTbl;
    ConfigPtrs->CustodyConfigPtr   = &CustodyTbl;
    ConfigPtrs->MibPnConfigPtr     = &MibPnTbl;
    ConfigPtrs->MibPsConfigPtr     = &MibPsTbl;
    ConfigPtrs->ReportConfigPtr    = &ReportTbl;
    ConfigPtrs->AuthConfigPtr      = &AuthTbl;
    ConfigPtrs->LatConfigPtr       = &LatencyTbl;
    ConfigPtrs->StorConfigPtr      = &StorTbl;    

    return BPLib_NC_Init(ConfigPtrs);
}
