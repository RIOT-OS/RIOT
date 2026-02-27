/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#include "bplib_riot_nc.h"
#include "bplib.h"

static BPLib_PI_ChannelTable_t       ChanTbl = { 0 };
static BPLib_CLA_ContactsTable_t ContactsTbl = { 0 };
static BPLib_NC_MibPerNodeConfig_t  MibPnTbl = {
    .InstanceEID = {
        .Scheme       = BPLIB_LOCAL_EID_SCHEME,
        .IpnSspFormat = BPLIB_LOCAL_EID_IPN_SSP_FORMAT,
        .Allocator    = BPLIB_LOCAL_EID_ALLOCATOR,
        .Node         = BPLIB_LOCAL_EID_NODE_NUM,
        .Service      = BPLIB_LOCAL_EID_SERVICE_NUM
    },

    .Configs = {
        /* PARAM_BUNDLE_SIZE_NO_FRAGMENT      */ BPLIB_MAX_BUNDLE_LEN,
        /* PARAM_SET_MAX_SEQUENCE NUM         */ BPLIB_MAX_SEQ_NUM,
        /* PARAM_SET_MAX_PAYLOAD_LENGTH       */ BPLIB_MAX_PAYLOAD_SIZE,
        /* PARAM_SET_MAX_BUNDLE_LENGTH        */ BPLIB_MAX_BUNDLE_LEN,
        /* PARAM_SET_NODE_DTN_TIME            */ 0,
        /* PARAM_SET_BEHAVIOR_EVENT_REPORTING */ 10,
        /* PARAM_SET_MAX_LIFETIME             */ BPLIB_MAX_LIFETIME_ALLOWED
    }
};

BPLib_Status_t bplib_riot_nc_init(BPLib_NC_ConfigPtrs_t* ConfigPtrs)
{
    if (ConfigPtrs == NULL) {
        return BPLIB_NULL_PTR_ERROR;
    }

    ConfigPtrs->ChanConfigPtr      = &ChanTbl;
    ConfigPtrs->ContactsConfigPtr  = &ContactsTbl;
    ConfigPtrs->CrsConfigPtr       = NULL;
    ConfigPtrs->CustodianConfigPtr = NULL;
    ConfigPtrs->CustodyConfigPtr   = NULL;
    ConfigPtrs->MibPnConfigPtr     = &MibPnTbl;
    ConfigPtrs->MibPsConfigPtr     = NULL;
    ConfigPtrs->ReportConfigPtr    = NULL;
    ConfigPtrs->AuthConfigPtr      = NULL;
    ConfigPtrs->LatConfigPtr       = NULL;
    ConfigPtrs->StorConfigPtr      = NULL;

    return BPLib_NC_Init(ConfigPtrs);
}

static BPLib_Status_t channel_ok_and_stopped(uint32_t channel)
{
    if (channel >= BPLIB_MAX_NUM_CHANNELS) {
        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    if (BPLib_NC_GetAppState(channel) != BPLIB_NC_APP_STATE_REMOVED) {
        return BPLIB_APP_STATE_ERR;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t bplib_channel_set_hop_limit(uint32_t channel, uint8_t limit)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].HopLimit = limit;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_crc_type(uint32_t channel, BPLib_CRC_Type_t type)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        if (type != BPLib_CRC_Type_CRC16 &&
            type != BPLib_CRC_Type_CRC32C)
        {
            return BPLIB_INVALID_CRC_ERROR;
        }

        ChanTbl.Configs[channel].CrcType = type;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_service_no(uint32_t channel, uint64_t service_no)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].LocalServiceNumber = service_no;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_bundle_flags(uint32_t channel, uint64_t flags)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].BundleProcFlags = flags;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_lifetime(uint32_t channel, uint64_t lifetime)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].Lifetime = lifetime;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_dest_eid(uint32_t channel, BPLib_EID_t dest_eid)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        BPLib_EID_CopyEids(&ChanTbl.Configs[channel].DestEID, dest_eid);
    }
    return status;
}

BPLib_Status_t bplib_channel_set_report_to_eid(uint32_t channel, BPLib_EID_t report_eid)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        BPLib_EID_CopyEids(&ChanTbl.Configs[channel].ReportToEID, report_eid);
    }
    return status;
}

static BPLib_PI_CanBlkConfig_t* map_block(uint32_t channel,
            bplib_nc_canonical_block_t block)
{
    switch (block) {
    case BPLIB_PREVIOUS_NODE_BLOCK:
        return &ChanTbl.Configs[channel].PrevNodeBlkConfig;
    case BPLIB_BUNDLE_AGE_BLOCK:
        return &ChanTbl.Configs[channel].AgeBlkConfig;
    case BPLIB_HOP_COUNT_BLOCK:
        return &ChanTbl.Configs[channel].HopCountBlkConfig;
    case BPLIB_PAYLOAD_BLOCK:
        return &ChanTbl.Configs[channel].PayloadBlkConfig;
    default:
        return NULL;
    }
}

BPLib_Status_t bplib_channel_set_block_include(uint32_t channel,
            bplib_nc_canonical_block_t block, bool include)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        BPLib_PI_CanBlkConfig_t* block_ptr = map_block(channel, block);
        if (block_ptr == NULL) {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        else {
            block_ptr->IncludeBlock = include;
        }
    }
    return status;
}

BPLib_Status_t bplib_channel_set_block_crc_type(uint32_t channel,
            bplib_nc_canonical_block_t block, BPLib_CRC_Type_t type)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        BPLib_PI_CanBlkConfig_t* block_ptr = map_block(channel, block);
        if (block_ptr == NULL) {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        if (type != BPLib_CRC_Type_None &&
            type != BPLib_CRC_Type_CRC16 &&
            type != BPLib_CRC_Type_CRC32C)
        {
            return BPLIB_INVALID_CRC_ERROR;
        }

        block_ptr->CrcType = type;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_block_num(uint32_t channel,
            bplib_nc_canonical_block_t block, uint32_t num)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        BPLib_PI_CanBlkConfig_t* block_ptr = map_block(channel, block);
        if (block_ptr == NULL) {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        else {
            block_ptr->BlockNum = num;
        }
    }
    return status;
}

BPLib_Status_t bplib_channel_set_block_flags(uint32_t channel,
            bplib_nc_canonical_block_t block, uint64_t flags)
{
    BPLib_Status_t status = channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        BPLib_PI_CanBlkConfig_t* block_ptr = map_block(channel, block);
        if (block_ptr == NULL) {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        else {
            block_ptr->BlockProcFlags = flags;
        }
    }
    return status;
}

static BPLib_Status_t contact_ok_and_stopped(uint32_t contact)
{
    if (contact >= BPLIB_MAX_NUM_CONTACTS) {
        return BPLIB_INVALID_CONT_ID_ERR;
    }

    BPLib_CLA_ContactRunState_t state;
    BPLib_CLA_GetContactRunState(contact, &state);
    if (state != BPLIB_CLA_TORNDOWN) {
        return BPLIB_CLA_INCORRECT_STATE;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t bplib_contact_set_destinations(uint32_t contact,
            unsigned index, BPLib_EID_Pattern_t eid_pat)
{
    BPLib_Status_t status = contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        if (index >= BPLIB_MAX_CONTACT_DEST_EIDS) {
            return BPLIB_ERROR;
        }

        BPLib_EID_CopyEidPatterns(&ContactsTbl.ContactSet[contact].DestEIDs[index], eid_pat);
    }
    return status;
}

BPLib_Status_t bplib_contact_set_out_addr(uint32_t contact,
            const char* addr, uint16_t port)
{
    BPLib_Status_t status = contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        strncpy(ContactsTbl.ContactSet[contact].ClaOutAddr, addr, BPLIB_MAX_IP_LENGTH);
        ContactsTbl.ContactSet[contact].ClaOutPort = port;
    }
    return status;
}

BPLib_Status_t bplib_contact_set_in_addr(uint32_t contact,
            const char* addr, uint16_t port)
{
    BPLib_Status_t status = contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        strncpy(ContactsTbl.ContactSet[contact].ClaInAddr, addr, BPLIB_MAX_IP_LENGTH);
        ContactsTbl.ContactSet[contact].ClaInPort = port;
    }
    return status;
}
