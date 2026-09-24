/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

 /**
 * @ingroup     pkg_bplib_nc
 * @{
 *
 * @file
 * @brief       A wrapper for the NC operations.
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
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
        BPLIB_MAX_BUNDLE_LEN,           /* PARAM_BUNDLE_SIZE_NO_FRAGMENT */
        CONFIG_BPLIB_MAX_SEQ_NUM,       /* PARAM_SET_MAX_SEQUENCE NUM */
        BPLIB_MAX_PAYLOAD_SIZE,         /* PARAM_SET_MAX_PAYLOAD_LENGTH */
        BPLIB_MAX_BUNDLE_LEN,           /* PARAM_SET_MAX_BUNDLE_LENGTH */
        CONFIG_BPLIB_SUPPORT_CUSTODY,   /* PARAM_SUPPORT_CUSTODY */
        BPLIB_MAX_LIFETIME_ALLOWED      /* PARAM_SET_MAX_LIFETIME */
    }
};

void bplib_riot_nc_init(BPLib_NC_ConfigPtrs_t* ConfigPtrs)
{
    if (ConfigPtrs == NULL) {
        return;
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
}

static BPLib_Status_t _channel_ok_and_stopped(uint32_t channel)
{
    if (channel >= BPLIB_MAX_NUM_CHANNELS) {
        return BPLIB_INVALID_CHAN_ID_ERR;
    }

    if (BPLib_NC_GetAppState(channel) != BPLIB_NC_APP_STATE_REMOVED) {
        return BPLIB_APP_STATE_ERR;
    }

    return BPLIB_SUCCESS;
}

static BPLib_Status_t _contact_ok_and_stopped(uint32_t contact)
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

BPLib_Status_t bplib_channel_set_hop_limit(uint32_t channel, uint8_t limit)
{
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].HopLimit = limit;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_crc_type(uint32_t channel, BPLib_CRC_Type_t type)
{
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
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
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].LocalServiceNumber = service_no;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_bundle_flags(uint32_t channel, uint64_t flags)
{
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].BundleProcFlags = flags;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_lifetime(uint32_t channel, uint64_t lifetime)
{
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        ChanTbl.Configs[channel].Lifetime = lifetime;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_dest_eid(uint32_t channel, BPLib_EID_t dest_eid)
{
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
    if (status == BPLIB_SUCCESS) {
        BPLib_EID_CopyEids(&ChanTbl.Configs[channel].DestEID, dest_eid);
    }
    return status;
}

BPLib_Status_t bplib_channel_set_report_to_eid(uint32_t channel, BPLib_EID_t report_eid)
{
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
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
    case BPLIB_CUSTODY_TRANSFER_BLOCK:
        return &ChanTbl.Configs[channel].CustodyTransferBlkConfig;
    default:
        return NULL;
    }
}

BPLib_Status_t bplib_channel_set_block_include(uint32_t channel,
            bplib_nc_canonical_block_t block, bool include)
{
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
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
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
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
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
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
    BPLib_Status_t status = _channel_ok_and_stopped(channel);
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

BPLib_Status_t bplib_contact_set_destinations(uint32_t contact,
            unsigned index, BPLib_EID_Pattern_t eid_pat)
{
    BPLib_Status_t status = _contact_ok_and_stopped(contact);
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
    BPLib_Status_t status = _contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        strncpy(ContactsTbl.ContactSet[contact].ClaOutAddr, addr, BPLIB_MAX_IP_LENGTH);
        ContactsTbl.ContactSet[contact].ClaOutPort = port;
    }
    return status;
}

BPLib_Status_t bplib_contact_set_in_addr(uint32_t contact,
            const char* addr, uint16_t port)
{
    BPLib_Status_t status = _contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        strncpy(ContactsTbl.ContactSet[contact].ClaInAddr, addr, BPLIB_MAX_IP_LENGTH);
        ContactsTbl.ContactSet[contact].ClaInPort = port;
    }
    return status;
}

BPLib_Status_t bplib_contact_set_cs_time_trigger(uint32_t contact, uint32_t millis)
{
    BPLib_Status_t status = _contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        if ((millis > BPLIB_MAX_CS_TIME_TRIGGER_ALLOWED) ||
            (millis < BPLIB_MIN_CS_TIME_TRIGGER_ALLOWED)) {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        ContactsTbl.ContactSet[contact].CSTimeTrigger = millis;
    }
    return status;
}

BPLib_Status_t bplib_contact_set_cs_size_trigger(uint32_t contact, uint32_t size)
{
    BPLib_Status_t status = _contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        if ((size > BPLIB_MAX_CS_SIZE_TRIGGER_ALLOWED) ||
            (size < BPLIB_MIN_CS_SIZE_TRIGGER_ALLOWED)) {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        ContactsTbl.ContactSet[contact].CSSizeTrigger = size;
    }
    return status;
}

BPLib_Status_t bplib_contact_set_cs_retransmit_time(uint32_t contact, uint32_t millis)
{
    BPLib_Status_t status = _contact_ok_and_stopped(contact);
    if (status == BPLIB_SUCCESS) {
        if ((millis > BPLIB_MAX_RETRANSMIT_ALLOWED) ||
            (millis < BPLIB_MIN_RETRANSMIT_ALLOWED)) {
            return BPLIB_INVALID_CONFIG_ERR;
        }
        ContactsTbl.ContactSet[contact].RetransmitTimeout = millis;
    }
    return status;
}

BPLib_Status_t bplib_channel_set_state(uint32_t channel, BPLib_NC_ApplicationState_t state)
{
    /* BPLIB_INVALID_CHAN_ID_ERR and BPLIB_APP_STATE_ERR are checked by the bplib PI functions */

    BPLib_Status_t status = BPLIB_SUCCESS;
    BPLib_NC_ApplicationState_t curr_state = BPLib_NC_GetAppState(channel);

    /* For fast forward, try to call this function recursively once to change this state */
    if (curr_state == BPLIB_NC_APP_STATE_REMOVED && state == BPLIB_NC_APP_STATE_STARTED) {
        status = bplib_channel_set_state(channel, BPLIB_NC_APP_STATE_ADDED);
    }
    else if (curr_state == BPLIB_NC_APP_STATE_STARTED && state == BPLIB_NC_APP_STATE_REMOVED) {
        status = bplib_channel_set_state(channel, BPLIB_NC_APP_STATE_STOPPED);
    }

    if (status != BPLIB_SUCCESS) {
        /* Fast forward failed */
        return status;
    }

    switch (state) {
    case BPLIB_NC_APP_STATE_ADDED:
        status = BPLib_PI_AddApplication(&bplib_instance_data.BPLibInst, channel);
        break;
    case BPLIB_NC_APP_STATE_STARTED:
        status = BPLib_PI_StartApplication(channel);
        break;
    case BPLIB_NC_APP_STATE_STOPPED:
        status = BPLib_PI_StopApplication(channel);
        break;
    case BPLIB_NC_APP_STATE_REMOVED:
        status = BPLib_PI_RemoveApplication(&bplib_instance_data.BPLibInst, channel);
        break;
    default:
        return BPLIB_INVALID_CONFIG_ERR;
    }

    return status;
}

BPLib_Status_t bplib_contact_set_state(uint32_t contact, BPLib_CLA_ContactRunState_t state)
{
    /* BPLIB_INVALID_CONT_ID_ERR and BPLIB_CLA_INCORRECT_STATE are checked
     * by the bplib CLA functions */

    BPLib_Status_t status = BPLIB_SUCCESS;
    BPLib_CLA_ContactRunState_t curr_state;
    status = BPLib_CLA_GetContactRunState(contact, &curr_state);
    if (status != BPLIB_SUCCESS) {
        return status;
    }

    /* For fast forward, try to call this function recursively once to change this state */
    if (curr_state == BPLIB_CLA_TORNDOWN && state == BPLIB_CLA_STARTED) {
        status = bplib_contact_set_state(contact, BPLIB_CLA_SETUP);
    }
    else if (curr_state == BPLIB_CLA_STARTED && state == BPLIB_CLA_TORNDOWN) {
        status = bplib_contact_set_state(contact, BPLIB_CLA_STOPPED);
    }

    if (status != BPLIB_SUCCESS) {
        /* Fast forward failed */
        return status;
    }

    switch (state) {
    case BPLIB_CLA_SETUP:
        status = BPLib_CLA_ContactSetup(&bplib_instance_data.BPLibInst, contact);
        break;
    case BPLIB_CLA_STARTED:
        status = BPLib_CLA_ContactStart(&bplib_instance_data.BPLibInst, contact);
        break;
    case BPLIB_CLA_STOPPED:
        status = BPLib_CLA_ContactStop(&bplib_instance_data.BPLibInst, contact);
        break;
    case BPLIB_CLA_TORNDOWN:
        status = BPLib_CLA_ContactTeardown(&bplib_instance_data.BPLibInst, contact);
        break;
    default:
        return BPLIB_INVALID_CONFIG_ERR;
    }

    return status;
}
