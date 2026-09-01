/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#include "bplib_init.h"
#include "bplib.h"
#include "bplib_riot_nc.h"

#include "bplib_cla_uwb.h"

#include "fmt.h"
#include "shell.h"

#include "dw3000.h"
#include "deca_device_api.h"

#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>

#ifndef BPLIB_EXAMPLE_REMOTE_NODE_NO
#  define BPLIB_EXAMPLE_REMOTE_NODE_NO      200
#endif

/* Note: Not changed through makefile, here both devices have the same service number,
 * but it could be changed like the node number. It is the analog to an IP port. */
#define BPLIB_EXAMPLE_REMOTE_SERVICE_NO     123

static char stack_egress[THREAD_STACKSIZE_MEDIUM];
static char recv_buffer[1024];

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

    bplib_channel_set_hop_limit(0, 10);
    bplib_channel_set_block_include(0, BPLIB_HOP_COUNT_BLOCK, true);
    bplib_channel_set_block_num(0, BPLIB_HOP_COUNT_BLOCK, 3);
    bplib_channel_set_block_crc_type(0, BPLIB_HOP_COUNT_BLOCK, BPLib_CRC_Type_CRC16);

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
    bplib_contact_set_out_addr(0, "1234", 0xdeca);
    bplib_contact_set_in_addr(0, "1234", 0xdeca);
}

static void _bp_print_help(void) {
    printf("Usage: bp <send / contact / app / bench>\n");
    printf(" send    <channel_id> \"PAYLOAD\"\n");
    printf(" contact <contact_id> <start / stop>\n");
    printf(" channel <channel_id> <start / stop>\n");
    printf(" bench   <channel_id> <payload_len> <count> [interval_ms]\n");
}
static int _bp(int argc, char **argv) {
    BPLib_Status_t status;

    if (argc < 4) {
        _bp_print_help();
        return 1;
    }

    int c = atoi(argv[2]);

    if (strcmp(argv[1], "send") == 0) {
        BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, c, argv[3], strlen(argv[3]));
    }
    else if (strcmp(argv[1], "contact") == 0) {
        if (strcmp(argv[3], "start") == 0) {
            status = BPLib_CLA_ContactStart(c);
            if (status == 0) {
                printf("Started contact\n");
            } else {
                printf("Contact start failed with: %"PRIx32"\n", status);
            }

        } else if (strcmp(argv[3], "stop") == 0) {
            status = BPLib_CLA_ContactStop(c);
            if (status == 0) {
                printf("Stopped contact\n");
            } else {
                printf("Contact stop failed with: %"PRIx32"\n", status);
            }
        }
    }
    else if (strcmp(argv[1], "bench") == 0) {
        if (argc < 5) {
            _bp_print_help();
            return 1;
        }

        int len = atoi(argv[3]);
        int cnt = atoi(argv[4]);
        int intvl = 0;
        if (argc >= 6) {
            intvl = atoi(argv[5]);
        }

        for (int i = 0; i < cnt; i++) {
            /* TODO buffer. In theory dont care, the values are just read */
            BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, c, argv[0], len);
            if (intvl) {
                ztimer_sleep(ZTIMER_MSEC, intvl);
            }
        }
    }

    return 0;
}
SHELL_COMMAND(bp, "Test bplib with different commands", _bp);

static dwt_config_t _config = {
    .chan = 9,
    .txPreambLength = DWT_PLEN_64,
    .rxPAC = DWT_PAC8,
    .txCode = 11,
    .rxCode = 11,
    .sfdType = DWT_SFD_IEEE_4Z,
    .dataRate = DWT_BR_6M8,
    .phrMode = DWT_PHRMODE_EXT,
    .phrRate = DWT_PHRRATE_STD,
    .sfdTO = (64 + 1 + 8 - 8),   /* (plen + 1 + SFD length - PAC size) */
    .stsMode = DWT_STS_MODE_OFF,
    .stsLength = DWT_STS_LEN_64,
    .pdoaMode = DWT_PDOA_M0,     /* off */
};

static int _init_deca(void)
{
    dw3000_hw_init();
    dw3000_hw_init_interrupt();
    dw3000_hw_reset();

    dwt_probe((struct dwt_probe_s *)&dw3000_probe_interf);
    uint32_t dev_id = dwt_readdevid();
    printf("[deca init] detected device id: %"PRIx32"\n", dev_id);

    /* The API guide says it is recommended to check for idle rc before
     * dwt_initialse(), while libdeca does the other order. */
    while (!dwt_checkidlerc()) {};
    puts("[deca init] DW3xxx reached IDLE_RC");

    if (dwt_initialise(DWT_DW_IDLE) != DWT_SUCCESS) {
        puts("[deca init] Error initializing device");
        return 1;
    }

    /* After idle rc the SPI speed can be increased */
    dw3000_spi_speed_fast();

    if (dwt_configure(&_config) != DWT_SUCCESS) {
        puts("[deca init] Error configuring device to given parameters");
        return 1;
    }
    puts("[deca init] Device initialized and configured");
    return 0;
}

int main(void)
{
    int rv = bplib_init();
    if (rv != 0) {
        printf("Error Initializing bplib %i\n", rv);
        return 1;
    }

    _config_nc();

    rv = _init_deca();
    if (rv != 0) {
        printf("Error Initializing decadriver %i\n", rv);
        return 1;
    }

    rv = bplib_cla_uwb_start(0);
    if (rv != 0) {
        printf("UWB CLA failed to initialize, error %i\n", rv);
        if (rv == -EINVAL) {
            printf("Is your addresses configures and formatted correctly?\n");
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

    bplib_cla_uwb_stop();
    BPLib_CLA_ContactStop(0);
    BPLib_CLA_ContactTeardown(&bplib_instance_data.BPLibInst, 0);

    BPLib_PI_StopApplication(0);
    BPLib_PI_RemoveApplication(&bplib_instance_data.BPLibInst, 0);

    bplib_terminate();
}
