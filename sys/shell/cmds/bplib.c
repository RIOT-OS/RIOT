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

#include "shell.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

/* Strings used for contact and channel config */
#define STR_INVALID_CHANNEL_CONTACT "Invalid channel/contact: %s\n"
#define STR_INVALID_STATE_KW        "Invalid state keyword '%s'\n"
#define STR_CURRENT_STATE           "Current state: %s\n"
#define STR_ONLY_IN_OFF_STATE       "Only possible in 'torndown'/'removed'\n"

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
         " state [<add / start / stop / remove>]");
}

static void __print_help_bp_contact(void)
{
    puts("Usage: bplib contact <contact_id> <subcommand>\n"
         " state [<setup / start / stop / teardown>]");
}

static void __print_help_bp_send(void)
{
    puts("Usage: bplib send <channel_id> <payload>");
}

static bool __validate_channel(char* chan_str, int* chan)
{
    if (isdigit(*chan_str)) {
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
    if (isdigit(*cont_str)) {
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
        printf(STR_INVALID_STATE_KW, arg);
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
        printf(STR_INVALID_STATE_KW, arg);
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

    return 0;
}

static int _bp_config(int argc, char **argv)
{
    if (argc < 3) {
        __print_help_bp_config();
        return 1;
    }

    if (strcmp(argv[2], "node") == 0) {
        // TODO parse dtn:...
        // bplib_config_set_local_eid
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

    /* Things to configure:                     command                                 scope       status
     *
     * Local Node No                            bp config node                          1
     *
     * Local Service No per channel             bp channel service                      1
     * Dest per channel                         bp channel dest                         1
     * (Report to per channel)                  bp channel report_to                    1
     * Current state set / get                  bp channel state                        1           done
     *
     * Local addr and port                      bp contact bind_addr / bind_port        2
     * Remote addr and port                     bp contact remote_add / remote_port     2
     * CLA restart                          on: bp contact state teardown               2
     * -> whole other topic,                by: FWP callbacks
     *    move CLA to common struct
     * Set, unset routes                        bp contact routes <i>                   2
     *
     * Fast Forward contact / channel states                                            2
     * -> only torndown -> setup -> start & start -> stop -> towndown
     *    teardown for reconfig should be explicit
     *
     * TODO delete this later
     * */

    return 0;
}
SHELL_COMMAND(bplib, "Configure and interact with bplib", _bp);
