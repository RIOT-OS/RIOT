/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#include "bplib_init.h"
#include "bplib_cla_ble.h"
#include "bplib.h"
#include "bplib_riot_nc.h"

#include "shell.h"

#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>

#ifndef BPLIB_EXAMPLE_ROLE_CLIENT
#  define BPLIB_EXAMPLE_ROLE_CLIENT         0
#endif

#ifndef BPLIB_EXAMPLE_REMOTE
#  define BPLIB_EXAMPLE_REMOTE              ""
#endif

#ifndef BPLIB_EXAMPLE_REMOTE_NODE_NO
#  define BPLIB_EXAMPLE_REMOTE_NODE_NO      200
#endif

/* Note: Not changed through makefile, here both devices have the same service number,
 * but it could be changed like the node number. It is the analog to an IP port. */
#define BPLIB_EXAMPLE_REMOTE_SERVICE_NO     123

static char stack_egress[THREAD_STACKSIZE_MEDIUM];
static char recv_buffer[CONFIG_BPLIB_CLA_BLE_MTU];

static void* _poll_bp(void* arg)
{
    (void) arg;
    BPLib_Status_t rv;
    size_t size;

    while (1) {
        rv = BPLib_PI_Egress(&bplib_instance_data.BPLibInst, 0, recv_buffer, &size,
            sizeof(recv_buffer), 1000);
        if (rv == 0) {
            recv_buffer[size] = '\0';
            printf("Received Bundle of size %zd with content: %s\n", size, recv_buffer);
        }
        else if (rv != BPLIB_PI_TIMEOUT) {
            printf("Egress error: %"PRIi32"\n", rv);
        }
    }

    return NULL;
}

static void _config_nc(void)
{
    /* Configure the channel */
    BPLib_EID_t dest = {
       .Scheme = BPLIB_EID_SCHEME_IPN,
       .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
       .Allocator = 0,
       .Node = BPLIB_EXAMPLE_REMOTE_NODE_NO,
       .Service = BPLIB_EXAMPLE_REMOTE_SERVICE_NO
    };

    bplib_channel_set_crc_type(0, BPLib_CRC_Type_CRC16);
    bplib_channel_set_service_no(0, BPLIB_EXAMPLE_REMOTE_SERVICE_NO);
    bplib_channel_set_bundle_flags(0, 0);
    bplib_channel_set_lifetime(0, 3600000);
    bplib_channel_set_dest_eid(0, dest);
    bplib_channel_set_report_to_eid(0, BPLIB_EID_DTN_NONE);

    bplib_channel_set_block_crc_type(0, BPLIB_PAYLOAD_BLOCK, BPLib_CRC_Type_CRC32C);

    /* Note: Make sure every canonical block has a unique block number, the payload
     * has 1 statically and the primary block has 0 implicitly */
    bplib_channel_set_block_num(0, BPLIB_BUNDLE_AGE_BLOCK, 2);
    bplib_channel_set_block_crc_type(0, BPLIB_BUNDLE_AGE_BLOCK, BPLib_CRC_Type_CRC16);

    /* Uncomment these to generate the hop count block with the given limit
    bplib_channel_set_hop_limit(0, 10);
    bplib_channel_set_block_include(0, BPLIB_HOP_COUNT_BLOCK, true);
    bplib_channel_set_block_num(0, BPLIB_HOP_COUNT_BLOCK, 3);
    bplib_channel_set_block_crc_type(0, BPLIB_HOP_COUNT_BLOCK, BPLib_CRC_Type_None); */

    /* Uncomment these to generate the previous node block
    bplib_channel_set_block_include(0, BPLIB_PREVIOUS_NODE_BLOCK, true);
    bplib_channel_set_block_num(0, BPLIB_PREVIOUS_NODE_BLOCK, 4);
    bplib_channel_set_block_crc_type(0, BPLIB_PREVIOUS_NODE_BLOCK, BPLib_CRC_Type_None); */

    /* Configure the contact. The MAC address needs to be adjusted to your target server.
     * For the server this value is currently ignored, it accepts any connection. */
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
    bplib_contact_set_out_addr(0, BPLIB_EXAMPLE_REMOTE, 0);
}

static int _bp(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: bp <channel> <message>\n");
        return 1;
    }

    BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, atoi(argv[1]), argv[2], strlen(argv[2]));

    return 0;
}
SHELL_COMMAND(bp, "Send BP Test Bundles", _bp);

int main(void)
{
    int rv = bplib_init();
    if (rv != 0) {
        printf("Error Initializing bplib %i\n", rv);
        return 1;
    }

    _config_nc();

    rv = bplib_cla_ble_start(0, BPLIB_EXAMPLE_ROLE_CLIENT);
    if (rv != 0) {
        printf("BLE CLA failed to initialize, error %i\n", rv);
        if (rv == -EINVAL) {
            printf("Is your MAC address formatted correctly?\n");
        }
    }

    /* Add and start the application level I/O socket */
    BPLib_PI_AddApplication(0);
    BPLib_PI_StartApplication(0);

    /* Let bplib know the contact started */
    BPLib_CLA_ContactSetup(0);
    BPLib_CLA_ContactStart(0);

    thread_create(stack_egress, sizeof(stack_egress),
        THREAD_PRIORITY_MAIN - 1, 0, _poll_bp,
        NULL, "bplib APP IN");

    char buffer[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

    /* Note: Make sure to call BPLib_CLA_ContactTeardown and BPLib_PI_RemoveApplication
     * in production, since this includes some measures to push bundles not yet sent, but queued,
     * back into storage. This is not reachable here due to the shell. */
    bplib_cla_ble_stop();
    BPLib_CLA_ContactStop(0);
    BPLib_CLA_ContactTeardown(&bplib_instance_data.BPLibInst, 0);

    BPLib_PI_StopApplication(0);
    BPLib_PI_RemoveApplication(&bplib_instance_data.BPLibInst, 0);

    bplib_terminate();
}
