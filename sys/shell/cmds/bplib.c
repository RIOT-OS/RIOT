/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for the bplib package
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "bplib.h"
#include "bplib_init.h"
#include "bplib_eid_util.h"
#include "bplib_riot_nc.h"

#include "fmt.h"
#include "shell.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

/* Strings used for contact and channel config */
#define STR_INVALID_CHANNEL_CONTACT "Invalid channel/contact '%s'\n"
#define STR_INVALID_STATE           "Invalid state '%s'\n"
#define STR_INVALID_VALUE           "Invalid value '%s'\n"
#define STR_CURRENT_STATE           "Current state: %s\n"
#define STR_ONLY_IN_OFF_STATE       "Only possible in 'torndown'/'removed'\n"
#define STR_OTHER_ERROR             "Other error: %" PRIi32 "\n"

static void __print_help_bp(void)
{
    puts("Usage: bplib <subcommand>\n"
         " info    - Print general information\n"
         " config  - Configure bplib-wide settings\n"
         " channel - Configure per-channel settings\n"
         " contact - Configure per-contact settings\n"
         " send    - Ingress bundles");
}

static void __print_help_bp_config(void)
{
    puts("Usage: bplib config <subcommand>\n"
         " node <node_no> - Set local node number");
}

static void __print_help_bp_channel(void)
{
    puts("Usage: bplib channel <channel_id> <subcommand>\n"
         " service <service_no>\n"
         " dest <eid>\n"
         " report_to <eid>\n"
         " block <pn / ba / hc / pl / ct>\n"
         "   include <bool>\n"
         "   crc <none / CRC16 / CRC32>\n"
         "   num <block_num>\n"
         "   flags <block_flags_hex>\n"
         " max_hops <uint8_t>\n"
         " flags <bundle_flags_hex>\n"
         " crc <CRC16 / CRC32C>\n"
         " lifetime <lifetime_ms>\n"
         " state <add / start / stop / remove>");
}

static void __print_help_bp_contact(void)
{
    puts("Usage: bplib contact <contact_id> <subcommand>\n"
         " state <setup / start / stop / teardown>");
}

static void __print_help_bp_send(void)
{
    puts("Usage: bplib send <channel_id> <payload>");
}

static bool __validate_channel(char* chan_str, int* chan)
{
    if (isdigit((int)*chan_str)) {
        *chan = atoi(chan_str);
        if ((*chan >= 0) && (*chan < BPLIB_MAX_NUM_CHANNELS)) {
            return true;
        }
    }

    printf(STR_INVALID_CHANNEL_CONTACT, chan_str);
    return false;
}

static bool __validate_contact(char* cont_str, int* cont)
{
    if (isdigit((int)*cont_str)) {
        *cont = atoi(cont_str);
        if ((*cont >= 0) && (*cont < BPLIB_MAX_NUM_CONTACTS)) {
            return true;
        }
    }

    printf(STR_INVALID_CHANNEL_CONTACT, cont_str);
    return false;
}

static void __print_contact_channel_feedback(BPLib_Status_t status)
{
    if (status == BPLIB_SUCCESS) {
        puts("Success");
    }
    else if ((status == BPLIB_CLA_INCORRECT_STATE) || (status == BPLIB_APP_STATE_ERR)) {
        puts("Current state cannot go to requested state directly");
    }
    else {
        printf("Action failed with error %"PRIx32"\n", status);
    }
}

static void __set_contact(int c, char* arg)
{
    BPLib_Status_t status;

    if (strcmp(arg, "setup") == 0) {
        status = BPLib_CLA_ContactSetup(c);
    }
    else if (strcmp(arg, "start") == 0) {
        status = BPLib_CLA_ContactStart(c);
    }
    else if (strcmp(arg, "stop") == 0) {
        status = BPLib_CLA_ContactStop(c);
    }
    else if (strcmp(arg, "teardown") == 0) {
        status = BPLib_CLA_ContactTeardown(&bplib_instance_data.BPLibInst, c);
    }
    else {
        printf(STR_INVALID_STATE, arg);
        return;
    }

    __print_contact_channel_feedback(status);
}

static void __set_channel(int c, char* arg)
{
    BPLib_Status_t status;

    if (strcmp(arg, "add") == 0) {
        status = BPLib_PI_AddApplication(c);
    }
    else if (strcmp(arg, "start") == 0) {
        status = BPLib_PI_StartApplication(c);
    }
    else if (strcmp(arg, "stop") == 0) {
        status = BPLib_PI_StopApplication(c);
    }
    else if (strcmp(arg, "remove") == 0) {
        status = BPLib_PI_RemoveApplication(&bplib_instance_data.BPLibInst, c);
    }
    else {
        printf(STR_INVALID_STATE, arg);
        return;
    }

    __print_contact_channel_feedback(status);
}

static const char* __resolve_contact_state(BPLib_CLA_ContactRunState_t state) {
    switch (state) {
    case BPLIB_CLA_TORNDOWN:
        return "torndown";
    case BPLIB_CLA_SETUP:
        return "setup";
    case BPLIB_CLA_STARTED:
        return "started";
    case BPLIB_CLA_STOPPED:
        return "stopped";
    default:
        return "[invalid]";
    }
}

static const char* __resolve_channel_state(BPLib_NC_ApplicationState_t state) {
    switch (state) {
    case BPLIB_NC_APP_STATE_REMOVED:
        return "removed";
    case BPLIB_NC_APP_STATE_STOPPED:
        return "stopped";
    case BPLIB_NC_APP_STATE_ADDED:
        return "added";
    case BPLIB_NC_APP_STATE_STARTED:
        return "started";
    default:
        return "[invalid]";
    }
}

static bplib_nc_canonical_block_t __resolve_block_type(const char* str) {
    if (strcmp(str, "pn") == 0) {
        return BPLIB_PREVIOUS_NODE_BLOCK;
    }
    if (strcmp(str, "ba") == 0) {
        return BPLIB_BUNDLE_AGE_BLOCK;
    }
    if (strcmp(str, "hc") == 0) {
        return BPLIB_HOP_COUNT_BLOCK;
    }
    if (strcmp(str, "pl") == 0) {
        return BPLIB_PAYLOAD_BLOCK;
    }
    if (strcmp(str, "ct") == 0) {
        return BPLIB_CUSTODY_TRANSFER_BLOCK;
    }
    
    return BPLIB_INVALID_BLOCK;
}

static bool __resolve_crc_str(const char* str, BPLib_CRC_Type_t* crc) {
    if (strcmp(str, "CRC16") == 0) {
        *crc = BPLib_CRC_Type_CRC16;
    }
    else if (strcmp(str, "CRC32") == 0) {
        *crc = BPLib_CRC_Type_CRC32C;
    }
    else if (strcmp(str, "none") == 0) {
        *crc = BPLib_CRC_Type_None;
    }
    else {
        return false;
    }
    
    return true;
}

static const char* __resolve_crc_type(BPLib_CRC_Type_t crc) {
    switch (crc) {
    case BPLib_CRC_Type_None:
        return "none";
    case BPLib_CRC_Type_CRC16:
        return "CRC16";
    case BPLib_CRC_Type_CRC32C:
        return "CRC32";
    default:
        return "[invalid]";
    }
}

static int _bp_send(int argc, char **argv)
{
    if (argc < 4) {
        __print_help_bp_send();
        return 1;
    }

    int c = 0;

    if (!__validate_channel(argv[2], &c)) {
        return 1;
    }

    BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, c, argv[3], strlen(argv[3]));

    return 0;
}

static int _bp_contact(int argc, char **argv)
{
    if (argc < 4) {
        __print_help_bp_contact();
        return 1;
    }

    int c = 0;

    if (!__validate_contact(argv[2], &c)) {
        return 1;
    }

    if (strcmp(argv[3], "state") == 0) {
        /* Return current state */
        if (argc == 4) {
            BPLib_CLA_ContactRunState_t state;
            BPLib_CLA_GetContactRunState(c, &state);

            printf(STR_CURRENT_STATE, __resolve_contact_state(state));
        }
        else {
            __set_contact(c, argv[4]);
        }
    }
    else {
       __print_help_bp_contact();
        return 1; 
    }

    return 0;
}

/* Print feedback for channel, config commands */
static void __print_feedback(BPLib_Status_t rv)
{
    if (rv == BPLIB_APP_STATE_ERR) {
        printf(STR_ONLY_IN_OFF_STATE);
    }
    else if (rv == BPLIB_INVALID_CRC_ERROR) {
        /* This is returned only when no CRC is trying to be set for the primary
         * block, which is not possible. */
        printf(STR_INVALID_VALUE, "none");
    }
    else if (rv != BPLIB_SUCCESS) {
        printf(STR_OTHER_ERROR, rv);
    }
}

static int _bp_channel_block(int argc, char **argv, int c)
{
    /* The calling function has already checked: channel validity
     * e.g. 'bplib channel 0 block ct include 1' */
    if (argc < 6) {
        __print_help_bp_channel();
        return 1;
    }

    /* This is a setting command if there are enough args */
    bool set = argc >= 7;

    bplib_nc_canonical_block_t block = __resolve_block_type(argv[4]);
    if (block == BPLIB_INVALID_BLOCK) {
        __print_help_bp_channel();
        return 1; 
    }

    BPLib_Status_t rv = BPLIB_SUCCESS;
    BPLib_PI_CanBlkConfig_t* block_cfg = bplib_channel_map_block(c, block);

    if (strcmp(argv[5], "include") == 0) {
        if (set) {
            int res = scn_bool_str(argv[6]);
            if (res == -EINVAL) {
                printf(STR_INVALID_VALUE, argv[6]);
                return 1;
            }
            rv = bplib_channel_set_block_include(c, block, res);
        }
        else {
            printf("Current value: %s\n", block_cfg->IncludeBlock ? "true" : "false");
        }
    }
    else if (strcmp(argv[5], "crc") == 0) {
        if (set) {
            BPLib_CRC_Type_t crc;
            if (!__resolve_crc_str(argv[6], &crc)) {
                printf(STR_INVALID_VALUE, argv[6]);
                return 1;
            }
                
            rv = bplib_channel_set_block_crc_type(c, block, crc);
        }
        else {
            printf("Current value: %s\n", __resolve_crc_type(block_cfg->CrcType));
        }
    }
    else if (strcmp(argv[5], "num") == 0) {
        if (set) {
            rv = bplib_channel_set_block_num(c, block, strtoul(argv[6], NULL, 10));
        }
        else {
            printf("Current value: %" PRIi32 "\n", block_cfg->BlockNum);
        }
    }
    else if (strcmp(argv[5], "flags") == 0) {
        if (set) {
            rv = bplib_channel_set_block_flags(c, block, strtoull(argv[6], NULL, 16));
        }
        else {
            printf("Current value: 0x%" PRIx64 "\n", block_cfg->BlockProcFlags);
        }
    }
    else {
        __print_help_bp_channel();
        return 1; 
    }

    /* A valid command was executed, but it might have failed */
    __print_feedback(rv);

    return 0;
}

static int _bp_channel(int argc, char **argv)
{
    if (argc < 4) {
        __print_help_bp_channel();
        return 1;
    }

    int c = 0;

    if (!__validate_channel(argv[2], &c)) {
        return 1;
    }

    BPLib_PI_Config_t* chan_cfg = &bplib_instance_data.ConfigPtrs.
                                  ChanConfigPtr->Configs[c];
    BPLib_Status_t rv = BPLIB_SUCCESS;

    if (strcmp(argv[3], "state") == 0) {
        /* Return current state */
        if (argc == 4) {
            BPLib_NC_ApplicationState_t state = BPLib_NC_GetAppState(c);

            printf(STR_CURRENT_STATE, __resolve_channel_state(state));
        }
        else {
            __set_channel(c, argv[4]);
        }
    }
    else if (strcmp(argv[3], "block") == 0) {
        return _bp_channel_block(argc, argv, c);
    }
    else if (strcmp(argv[3], "service") == 0) {
        if (argc == 4) {
            printf("Current value: %" PRIu64 "\n", chan_cfg->LocalServiceNumber);
        }
        else {
            rv = bplib_channel_set_service_no(c, strtoull(argv[4], NULL, 10));
            __print_feedback(rv);
        }
    }
    else if (strcmp(argv[3], "max_hops") == 0) {
        if (argc == 4) {
            printf("Current value: %" PRIu8 "\n", chan_cfg->HopLimit);
        }
        else {
            int val = atoi(argv[4]);
            if ((val < 1) || (val > 255)) {
                printf(STR_INVALID_VALUE, argv[4]);
                return 1;
            }
            rv = bplib_channel_set_hop_limit(c, val);
            __print_feedback(rv);
        }
    }
    else if (strcmp(argv[3], "flags") == 0) {
        if (argc == 4) {
            printf("Current value: 0x%" PRIx64 "\n", chan_cfg->BundleProcFlags);
        }
        else {
            rv = bplib_channel_set_bundle_flags(c, strtoull(argv[4], NULL, 16));
            __print_feedback(rv);
        }
    }
    else if (strcmp(argv[3], "crc") == 0) {
        if (argc == 4) {
            printf("Current value: %s\n", __resolve_crc_type(chan_cfg->CrcType));
        }
        else {
            BPLib_CRC_Type_t crc;
            if (!__resolve_crc_str(argv[4], &crc)) {
                printf(STR_INVALID_VALUE, argv[4]);
                return 1;
            }
            rv = bplib_channel_set_crc_type(c, crc);
            __print_feedback(rv);
        }
    }
    else if (strcmp(argv[3], "lifetime") == 0) {
        if (argc == 4) {
            printf("Current value: %" PRIu64 "\n", chan_cfg->Lifetime);
        }
        else {
            rv = bplib_channel_set_lifetime(c, strtoull(argv[4], NULL, 10));
            __print_feedback(rv);
        }
    }
    else if (strcmp(argv[3], "dest") == 0) {
        if (argc == 4) {
            char buf[DTN_EID_IPN_MAX_SIZE];
            bplib_util_eid2str(&chan_cfg->DestEID, buf);
            printf("Current value: %s\n", buf);
        }
        else {
            BPLib_EID_t eid;
            if (!bplib_util_str2eid(argv[4], &eid)) {
                printf(STR_INVALID_VALUE, argv[4]);
                return 1;
            }
            rv = bplib_channel_set_dest_eid(c, eid);
            __print_feedback(rv);
        }
    }
    else if (strcmp(argv[3], "report_to") == 0) {
        if (argc == 4) {
            char buf[DTN_EID_IPN_MAX_SIZE];
            bplib_util_eid2str(&chan_cfg->ReportToEID, buf);
            printf("Current value: %s\n", buf);
        }
        else {
            BPLib_EID_t eid;
            if (!bplib_util_str2eid(argv[4], &eid)) {
                printf(STR_INVALID_VALUE, argv[4]);
                return 1;
            }
            rv = bplib_channel_set_report_to_eid(c, eid);
            __print_feedback(rv);
        }
    }
    else {
       __print_help_bp_channel();
        return 1; 
    }

    return 0;
}

static int _bp_info(void)
{
    char strbuf[DTN_EID_IPN_MAX_SIZE];

    bplib_util_eid2str(&BPLIB_EID_INSTANCE, strbuf);
    printf("Node: %s\n", strbuf);

    puts("Channels:");
    for (int i = 0; i < BPLIB_MAX_NUM_CHANNELS; i++) {
        BPLib_PI_Config_t* chan_cfg = &bplib_instance_data.ConfigPtrs.
                                        ChanConfigPtr->Configs[i];
        printf("%i: Service: %"PRIu64"\n", i, chan_cfg->LocalServiceNumber);
        bplib_util_eid2str(&chan_cfg->DestEID, strbuf);
        printf(" Dest: %s\n", strbuf);
    }

    puts("Contacts:");
    for (int i = 0; i < BPLIB_MAX_NUM_CONTACTS; i++) {
        BPLib_CLA_ContactsSet_t* cont_cfg = &bplib_instance_data.ConfigPtrs.
                                            ContactsConfigPtr->ContactSet[i];
        printf("%i: Out Addr: '%s' Port: %"PRIu16"\n",
                i, cont_cfg->ClaOutAddr, cont_cfg->ClaOutPort);
        printf(" Bind Addr: '%s' Port: %"PRIu16"\n",
                cont_cfg->ClaInAddr, cont_cfg->ClaInPort);

        /* List all routes */
        puts(" Routes:");
        for (int j = 0; j < BPLIB_MAX_CONTACT_DEST_EIDS; j++) {
            BPLib_EID_t eid1;
            BPLib_EID_t eid2;
            bplib_util_eids_from_pattern(&cont_cfg->DestEIDs[j], &eid1, &eid2);
            printf(" %i: ", j);
            if (cont_cfg->DestEIDs[j].Scheme == BPLIB_EID_SCHEME_RESERVED) {
                puts("not configured");
            }
            else {
                bplib_util_eid2str(&eid1, strbuf);
                printf("%s - ", strbuf);
                bplib_util_eid2str(&eid2, strbuf);
                printf("%s\n", strbuf);
            }
        }
    }

    // TODO include me when rebased bplib 7.0.5 
#if 0
    size_t used, free;
    used = BPLib_MEM_GetBytesInUse(&bplib_instance_data.BPLibInst.pool);
    free = BPLib_MEM_GetBytesFree(&bplib_instance_data.BPLibInst.pool);
    printf("MemPool usage [B]: %zu / %zu", used, used + free);
    used = BPLib_MEM_GetHighwaterMark(&bplib_instance_data.BPLibInst.pool);
    printf(", high-water: %zu\n", used);
#endif

    printf("Storage usage [B]: %zu / %zu",
        bplib_instance_data.BPLibInst.BundleStorage.BytesStorageInUse,
        (size_t) BPLIB_MAX_STORED_BUNDLE_BYTES);
    printf(", num stored: %"PRIi32"\n",
        bplib_instance_data.BPLibInst.BundleStorage.BundleCountStored);

    return 0;
}

static int _bp_config(int argc, char **argv)
{
    if (argc < 3) {
        __print_help_bp_config();
        return 1;
    }

    BPLib_Status_t rv = BPLIB_SUCCESS;

    if (strcmp(argv[2], "node") == 0) {
        if (argc == 3) {
            char buf[DTN_EID_IPN_MAX_SIZE];
            bplib_util_eid2str(&BPLIB_EID_INSTANCE, buf);
            printf("Current value: %s\n", buf);
        }
        else {
            BPLib_EID_t eid;
            if (!bplib_util_str2eid(argv[3], &eid) ||
                (eid.Allocator == 0 && eid.Node == 0) || (eid.Service != 0)) {
                /* The local node cannot be a null EID, but the local service
                 * number should be 0 */
                printf(STR_INVALID_VALUE, argv[3]);
                return 1;
            }
            rv = bplib_config_set_local_eid(&eid);
            __print_feedback(rv);
        }
    }
    else {
        __print_help_bp_config();
        return 1;
    }


    return 0;
}

static int _bp(int argc, char **argv)
{
    if (argc < 2) {
        __print_help_bp();
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        return _bp_send(argc, argv);
    }
    else if (strcmp(argv[1], "contact") == 0) {
        return _bp_contact(argc, argv);
    }
    else if (strcmp(argv[1], "channel") == 0) {
        return _bp_channel(argc, argv);
    }
    else if (strcmp(argv[1], "config") == 0) {
        return _bp_config(argc, argv);
    }
    else if (strcmp(argv[1], "info") == 0) {
        return _bp_info();
    }
    else {
        __print_help_bp();
        return 1;
    }

    return 0;
}
SHELL_COMMAND(bplib, "Configure and interact with bplib", _bp);
