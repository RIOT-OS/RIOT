/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Run IP-over-BLE using the 'statconn' BLE connection manager
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "nimble_statconn.h"
#include "net/bluetil/addr.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void _print_evt(const char *msg, int handle, const uint8_t *addr)
{
    printf("[ble] %s (%i|", msg, handle);
    if (addr) {
        bluetil_addr_print(addr);
    }
    else {
        printf("n/a");
    }
    puts(")");
}

static void _on_ble_evt(int handle, nimble_netif_event_t event,
                        const uint8_t *addr)
{
    switch (event) {
        case NIMBLE_NETIF_CONNECTED_MASTER:
            _print_evt("CONNECTED master", handle, addr);
            break;
        case NIMBLE_NETIF_CONNECTED_SLAVE:
            _print_evt("CONNECTED slave", handle, addr);
            break;
        case NIMBLE_NETIF_CLOSED_MASTER:
            _print_evt("CLOSED master", handle, addr);
            break;
        case NIMBLE_NETIF_CLOSED_SLAVE:
            _print_evt("CLOSED slave", handle, addr);
            break;
        case NIMBLE_NETIF_CONN_UPDATED:
            _print_evt("UPDATED", handle, addr);
            break;
        default:
            /* do nothing */
            return;
    }
}

int main(void)
{
    puts("IPv6-over-BLE with statconn BLE connection manager");

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* register for BLE events */
    nimble_statconn_eventcb(_on_ble_evt);

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
