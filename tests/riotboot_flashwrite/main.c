/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       riotboot_flashwrite test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "net/nanocoap_sock.h"
#include "xtimer.h"
#include "riotboot/slot.h"

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* import "ifconfig" shell command, used for printing addresses */
extern int _gnrc_netif_config(int argc, char **argv);

int main(void)
{
    puts("riotboot_flashwrite test application");

    int current_slot = riotboot_slot_current();
    if (current_slot != -1) {
        /* Sometimes, udhcp output messes up the following printfs. That
         * confuses the test script. As a workaround, just disable interrupts
         * for a while.
         */
        irq_disable();
        printf("running from slot %d\n", current_slot);
        printf("slot start addr = %p\n", (void *)riotboot_slot_get_hdr(current_slot));
        riotboot_slot_print_hdr(current_slot);
        irq_enable();
    }
    else {
        printf("[FAILED] You're not running riotboot\n");
    }

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("");

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");
    _gnrc_netif_config(0, NULL);

    /* initialize nanocoap server instance */
    puts("Starting nanocoap server instance");
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));

    /* should be never reached */
    return 0;
}
