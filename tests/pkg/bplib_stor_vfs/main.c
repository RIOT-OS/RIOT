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

#include "bplib.h"

#include "bplib_init.h"
#include "bplib_riot_nc.h"

#include "thread_flags.h"
#include "fmt.h"
#include "shell.h"

#define TEST_PAYLOAD        "Hello World!"
#define TEST_PAYLOAD_LEN    (sizeof(TEST_PAYLOAD) - 1)

#define THREAD_FLAG_CONTINUE        0x01

#define MOCK_CLA_EGRESS_TIMEOUT     20000

char _shell_thread_stack[THREAD_STACKSIZE_MAIN];

static thread_t* _main_thread;

static void _print_bundle_storage_info(void)
{
    printf("bundles in storage: %"PRIi32", bytes used: %zu, limit: %zu\n",
        bplib_instance_data.BPLibInst.BundleStorage.BundleCountStored,
        bplib_instance_data.BPLibInst.BundleStorage.BytesStorageInUse,
        (size_t) BPLIB_MAX_STORED_BUNDLE_BYTES);
}

/**
 * Source: This node
 * Destination: Other node
 * Custody Transfer: no
 */
static void _test_this_node_to_other_node(void)
{
    BPLib_Status_t rv;
    char bundle_buf[256];
    size_t out_size;

    /* TODO all of this might be doable directly in the shell once it is merged */

    /* Note: The success of the NC helper functions is not tested here, but their
     * failure likely leads to a failure in this test */

    bplib_channel_set_state(0, BPLIB_NC_APP_STATE_REMOVED);
    bplib_contact_set_state(0, BPLIB_CLA_TORNDOWN);

    /* TODO set this node no to something else, currently a compile time macro */
    bplib_channel_set_service_no(0, 100);

    BPLib_EID_t dest = {
       .Scheme = BPLIB_EID_SCHEME_IPN,
       .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
       .Allocator = 0,
       .Node = 400,
       .Service = 1234
    };
    bplib_channel_set_dest_eid(0, dest);
    bplib_channel_set_block_include(0, BPLIB_CUSTODY_TRANSFER_BLOCK, false);
    bplib_channel_set_lifetime(0, 1000 * 30);

    /* The contact can reach everything but is not started, so everything should
     * go to storage until it is started */
    BPLib_EID_Pattern_t reachable_eids = {
        .Scheme       = BPLIB_EID_SCHEME_IPN,
        .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
        .MaxAllocator = 0,
        .MinAllocator = 0,
        .MaxNode      = 0xffffffffffffffff,
        .MinNode      = 0,
        .MaxService   = 0xffffffffffffffff,
        .MinService   = 0
    };
    bplib_contact_set_destinations(0, 0, reachable_eids);

    bplib_channel_set_state(0, BPLIB_NC_APP_STATE_STARTED);

    rv = BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, 0, TEST_PAYLOAD, TEST_PAYLOAD_LEN);
    printf("bplib ingress %s\n", rv == BPLIB_SUCCESS ? "SUCCESS" : "ERROR");

    /* Now the bundle is in storage. Check if the metadata increased as expected */
    _print_bundle_storage_info();

    /* Tell bplib the contact is now ready, it should now egress the bundle into
     * this "CLA" which just prints it to stdout */
    bplib_contact_set_state(0, BPLIB_CLA_STARTED);
    rv = BPLib_CLA_Egress(&bplib_instance_data.BPLibInst, 0, bundle_buf,
                          &out_size, sizeof(bundle_buf), MOCK_CLA_EGRESS_TIMEOUT);

    if (rv != BPLIB_SUCCESS) {
        printf("bplib egress ERROR: %"PRIi32"\n", rv);
    }
    else {
        printf("bplib egress SUCCESS: ");
        print_bytes_hex(bundle_buf, out_size);
        printf("\n");
    }

    /* The bundle should have now been deleted from storage */
    _print_bundle_storage_info();
}

void *_shell_thread(void *arg)
{
    (void) arg;

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return NULL;
}

int main(void)
{
    _main_thread = thread_get_active();

    thread_create(_shell_thread_stack, sizeof(_shell_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, 0, _shell_thread, NULL,
                  "shell");

    /* Remove everything in the storage dir before bplib is started */
    char cmd[] = "vfs rm -r /nvm0/bp";
    shell_handle_input_line(NULL, cmd);

    BPLib_Status_t rv = bplib_init();
    if (rv != BPLIB_SUCCESS) {
        printf("bplib init ERROR: %"PRIi32"\n", rv);
        return 1;
    }

    puts("bplib init SUCCESS");

    /* Test case 1: Bundle from this node to external target is stored, and later egressed */
    _test_this_node_to_other_node();

    return 0;
}
