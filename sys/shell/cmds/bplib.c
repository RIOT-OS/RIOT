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

#include "shell.h"

#include <stdint.h>
#include <stdlib.h>

/* Strings used for contact and channel config */
#define STR_INVALID_CHANNEL_CONTACT "Invalid channel/contact: %i\n"
#define STR_INVALID_STATE_KW        "Invalid state keyword '%s'\n"
#define STR_CURRENT_STATE           "Current state: %s\n"

static void __bp_print_help(void)
{
    puts("Usage: bplib <subcommand>\n"
         " send    <channel_id> \"PAYLOAD\"\n"
         " contact <contact_id> [<setup / start / stop / teardown>]\n"
         " channel <channel_id> [<add / start / stop / remove>]");
}

static bool __validate_channel(int chan)
{
    if ((chan >= 0) && (chan < BPLIB_MAX_NUM_CHANNELS)) {
        return true;
    }
    else {
        printf(STR_INVALID_CHANNEL_CONTACT, chan);
        return false;
    }
}

static bool __validate_contact(int cont)
{
    if ((cont >= 0) && (cont < BPLIB_MAX_NUM_CONTACTS)) {
        return true;
    }
    else {
        printf(STR_INVALID_CHANNEL_CONTACT, cont);
        return false;
    }
}

static void __bp_print_contact_channel_feedback(BPLib_Status_t status)
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

static void __bp_contact(int c, char* arg)
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

    __bp_print_contact_channel_feedback(status);
}

static void __bp_channel(int c, char* arg)
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

    __bp_print_contact_channel_feedback(status);
}

static const char* __bp_resolve_contact_state(BPLib_CLA_ContactRunState_t state) {
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

static const char* __bp_resolve_channel_state(BPLib_NC_ApplicationState_t state) {
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

static int _bp(int argc, char **argv)
{
    if (argc < 3) {
        __bp_print_help();
        return 1;
    }

    int c = atoi(argv[2]);

    if (strcmp(argv[1], "send") == 0) {
        if (argc < 4) {
            __bp_print_help();
            return 1;
        }

        if (!__validate_channel(c)) {
            return 1;
        }

        BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, c, argv[3], strlen(argv[3]));
    }
    else if (strcmp(argv[1], "contact") == 0) {
        if (!__validate_contact(c)) {
            return 1;
        }

        /* Return current state */
        if (argc == 3) {
            BPLib_CLA_ContactRunState_t state;
            BPLib_CLA_GetContactRunState(c, &state);

            printf(STR_CURRENT_STATE, __bp_resolve_contact_state(state));
        }
        else {
            __bp_contact(c, argv[3]);
        }
    }
    else if (strcmp(argv[1], "channel") == 0) {
        if (!__validate_channel(c)) {
            return 1;
        }

        /* Return current state */
        if (argc == 3) {
            BPLib_NC_ApplicationState_t state = BPLib_NC_GetAppState(c);

            printf(STR_CURRENT_STATE, __bp_resolve_channel_state(state));
        }
        else {
            __bp_channel(c, argv[3]);
        }
    }
    else {
        __bp_print_help();
        return 1;
    }

    return 0;
}
SHELL_COMMAND(bplib, "Configure and interact with bplib", _bp);
