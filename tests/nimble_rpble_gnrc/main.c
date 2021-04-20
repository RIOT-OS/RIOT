/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example for showing 6LoWPAN over BLE using NimBLE and GNRC
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "shell.h"
#include "nimble_rpble.h"
#include "net/bluetil/addr.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void _dump_evt(const char *text, const uint8_t *addr)
{
    printf("[ble_event] %s (", text);
    bluetil_addr_print(addr);
    printf(")\n");
}

static void _on_rpble_event(int handle, nimble_netif_event_t event,
                          const uint8_t *addr)
{
    (void)handle;

    switch (event) {
        case NIMBLE_NETIF_CONNECTED_MASTER:
            _dump_evt("parent selected", addr);
            break;
        case NIMBLE_NETIF_CONNECTED_SLAVE:
            _dump_evt("child added", addr);
            break;
        case NIMBLE_NETIF_CLOSED_MASTER:
            _dump_evt("parent lost", addr);
            break;
        case NIMBLE_NETIF_CLOSED_SLAVE:
            _dump_evt("child lost", addr);
            break;
        default:
            /* not interested in any other BLE events here */
            break;
    }

}

int main(void)
{
    puts("RPL-over-BLE Example Application");

    /* register the custom event handler */
    nimble_rpble_eventcb(_on_rpble_event);

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets (ping6) */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
