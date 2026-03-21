/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#include "bplib_init.h"
#include "bplib_cla_udp.h"
#include "bplib.h"
#include "bplib_riot_nc.h"

#include "shell.h"

#include <inttypes.h>
#include <stdlib.h>

static bplib_cla_udp_t cla_udp1;

static char stack_egress[THREAD_STACKSIZE_MEDIUM];
static char recv_buffer[CONFIG_BPLIB_CLA_UDP_BUFLEN];
static void* poll_bp(void* arg) {
    (void) arg;
    BPLib_Status_t rv;
    size_t size;

    while (1) {
        rv = BPLib_PI_Egress(&bplib_instance_data.BPLibInst, 0, recv_buffer, &size,
            sizeof(recv_buffer), 1000);
        if (rv == 0) {
            recv_buffer[size] = '\0';
            printf("Received Bundle of size %zd with content: %s\n", size, recv_buffer);
        } else if (rv != BPLIB_PI_TIMEOUT) {
            printf("Egress error: %"PRIi32"\n", rv);
        }
    }

    return NULL;
}

static void config_nc(void)
{
    /* Configure the channel */
    BPLib_EID_t dest = {
       .Scheme = BPLIB_EID_SCHEME_IPN,
       .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
       .Allocator = 0,
       .Node = 100,
       .Service = 60
    };
    bplib_channel_set_crc_type(0, BPLib_CRC_Type_CRC16);
    bplib_channel_set_service_no(0, 10);
    bplib_channel_set_bundle_flags(0, 0);
    bplib_channel_set_lifetime(0, 3600000);
    bplib_channel_set_dest_eid(0, dest);
    bplib_channel_set_report_to_eid(0, BPLIB_EID_DTN_NONE);

    bplib_channel_set_block_crc_type(0, BPLIB_PAYLOAD_BLOCK, BPLib_CRC_Type_CRC32C);

    bplib_channel_set_block_num(0, BPLIB_BUNDLE_AGE_BLOCK, 2);
    bplib_channel_set_block_crc_type(0, BPLIB_BUNDLE_AGE_BLOCK, BPLib_CRC_Type_CRC16);

    bplib_channel_set_block_include(0, BPLIB_HOP_COUNT_BLOCK, true);
    bplib_channel_set_hop_limit(0, 10);
    bplib_channel_set_block_num(0, BPLIB_HOP_COUNT_BLOCK, 3);
    bplib_channel_set_block_crc_type(0, BPLIB_HOP_COUNT_BLOCK, BPLib_CRC_Type_None);

    bplib_channel_set_block_include(0, BPLIB_PREVIOUS_NODE_BLOCK, true);
    bplib_channel_set_block_num(0, BPLIB_PREVIOUS_NODE_BLOCK, 4);
    bplib_channel_set_block_crc_type(0, BPLIB_PREVIOUS_NODE_BLOCK, BPLib_CRC_Type_None);

    BPLib_EID_Pattern_t reachable_eids = {
        .Scheme       = BPLIB_EID_SCHEME_IPN,
        .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
        .MaxAllocator = 0,
        .MinAllocator = 0,
        .MaxNode      = 10000,
        .MinNode      = 1,
        .MaxService   = 10000,
        .MinService   = 1
    };
    bplib_contact_set_destinations(0, 0, reachable_eids);
    bplib_contact_set_out_addr(0, "fd00::9191:13", 4556);
    bplib_contact_set_in_addr(0, "fd00::9191:11", 4556);
}

/******************************************************************************
 * Main
 ******************************************************************************/
int main(void)
{
    /*char cmd[] = "ifconfig 5 add fd00::9191:11/64";
    shell_handle_input_line(NULL, cmd);*/

    int rv = bplib_init();
    if (rv != 0) {
        printf("Error Initializing bplib: %i\n", rv);
        return 1;
    }

    config_nc();

    /* Start the implementation of the CLA */
    bplib_cla_udp_start(&cla_udp1, 0);

    /* Add and start the application level I/O */
    BPLib_PI_AddApplication(0);
    BPLib_PI_StartApplication(0);

    /* Let bplib know the contact started */
    BPLib_CLA_ContactSetup(0);
    BPLib_CLA_ContactStart(0);

    /* Consume the incoming bundles */
    thread_create(stack_egress, sizeof(stack_egress),
        THREAD_PRIORITY_MAIN - 1, 0, poll_bp,
        NULL, "bplib APP IN");

    char buffer[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

    /* Cleanup (although not reachable), gives an idea of how this would be done if
     * bplib would be stopped in between. */
    BPLib_CLA_ContactStop(0);
    BPLib_CLA_ContactTeardown(&bplib_instance_data.BPLibInst, 0);
    bplib_cla_udp_stop(&cla_udp1);

    BPLib_PI_StopApplication(0);
    BPLib_PI_RemoveApplication(&bplib_instance_data.BPLibInst, 0);

    bplib_terminate();
}

static int _bp(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: bp <channel> <message>\n");
        return 1;
    }

    BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, atoi(argv[1]), argv[2], strlen(argv[2]));

    return 0;
}
SHELL_COMMAND(bp, "Send BP Test Bundles", _bp);
