/*
 * SPDX-FileCopyrightText: 2026 Simon Grund
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the vfs based storage backend of bplib
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "bplib.h"

#include "bplib_init.h"
#include "bplib_riot_nc.h"

#include "thread_flags.h"
#include "fmt.h"
#include "shell.h"
#include "test_utils/interactive_sync.h"

#define TEST_PAYLOAD        "Hello World!"

#define MOCK_CLA_EGRESS_TIMEOUT     10000
#define MOCK_CLA_RETRANSMIT_TIME    5000

static int _cmd_mock_egress(int argc, char **argv)
{
    BPLib_Status_t rv;
    char bundle_buf[256];
    size_t out_size;
    int timeout;
    int c;

    if (argc < 3) {
        puts("Usage: mock_egress contact timeout");
        return 1;
    }

    c = atoi(argv[1]);
    timeout = atoi(argv[2]);

    rv = BPLib_CLA_Egress(&bplib_instance_data.BPLibInst, c, bundle_buf,
                          &out_size, sizeof(bundle_buf), timeout);
    if (rv != BPLIB_SUCCESS) {
        printf("bplib egress ERROR: %"PRIi32"\n", rv);
    }
    else {
        printf("bplib egress SUCCESS: ");
        print_bytes_hex(bundle_buf, out_size);
        printf("\n");
    }
    return 0;
}

static int _cmd_mock_ingress(int argc, char **argv)
{
    BPLib_Status_t rv;
    char bundle_buf[256];
    int c;
    ssize_t bytes;

    if (argc < 3) {
        puts("Usage: mock_ingress contact hex_bundle");
        return 1;
    }

    c = atoi(argv[1]);
    bytes = scn_buf_hex(bundle_buf, sizeof(bundle_buf), argv[2], strlen(argv[2]));

    if (bytes <= 0) {
        printf("bplib ingress ERROR: %zi\n", bytes);
        return 1;
    }

    rv = BPLib_CLA_Ingress(&bplib_instance_data.BPLibInst, c,
                                          bundle_buf, bytes, 0);
    if (rv != BPLIB_SUCCESS) {
        printf("bplib ingress ERROR: %"PRIi32"\n", rv);
    }
    else {
        printf("bplib ingress SUCCESS\n");
    }
    return 0;
}

static int _cmd_restart_bplib(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    /* This function could terminate and restart bplib but this is bugged.
     * So just reset the required values. */
    bplib_instance_data.BPLibInst.Ct.LastSeqCounterId = 0;

    return 0;
}

SHELL_COMMAND(mock_egress, "Egress bundles into stdout", _cmd_mock_egress);
SHELL_COMMAND(mock_ingress, "Ingress bundles from stdin", _cmd_mock_ingress);
SHELL_COMMAND(restart_bplib, "Restart bplib", _cmd_restart_bplib);

int main(void)
{
    /* Remove everything in the storage dir before bplib is started */
    char cmd[] = "vfs rm -r /nvm0/bp";
    shell_handle_input_line(NULL, cmd);

    BPLib_Status_t rv = bplib_init();
    if (rv != BPLIB_SUCCESS) {
        printf("bplib init ERROR: %"PRIi32"\n", rv);
        return 1;
    }

    /* As long as contacts are not configurable through the shell, have one
     * contact that can reach everything */
    BPLib_EID_Pattern_t reachable_eids = {
        .Scheme       = BPLIB_EID_SCHEME_IPN,
        .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
        .MaxAllocator = 0,
        .MinAllocator = 0,
        .MaxNode      = BPLIB_EID_PATTERN_MAX_NODE,
        .MinNode      = 0,
        .MaxService   = BPLIB_EID_PATTERN_MAX_SERVICE,
        .MinService   = 0
    };
    bplib_contact_set_destinations(0, 0, reachable_eids);
    bplib_contact_set_cs_retransmit_time(0, MOCK_CLA_RETRANSMIT_TIME);

    puts("bplib init SUCCESS");

    char line_buf[256];
    shell_run(NULL, line_buf, sizeof(line_buf));

    return 0;
}
