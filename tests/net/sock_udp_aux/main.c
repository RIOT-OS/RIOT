/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for accessing auxiliary data using the UDP SOCK API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "fmt.h"
#include "net/sock/udp.h"
#include "shell.h"
#include "net/ipv6/addr.h"

#define MAIN_QUEUE_SIZE     (8)

#define PORT_UDP            12345
#define _QUOTE(x)           #x
#define QUOTE(x)            _QUOTE(x)

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
char server_thread_stack[THREAD_STACKSIZE_DEFAULT];

static const sock_aux_flags_t flags_rx = SOCK_AUX_GET_LOCAL
                                       | SOCK_AUX_GET_TIMESTAMP
                                       | SOCK_AUX_GET_RSSI;
static const sock_aux_flags_t flags_tx = SOCK_AUX_GET_TIMESTAMP;

static void *server_thread(void *arg)
{
    (void)arg;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    local.port = PORT_UDP;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        print_str("Error creating UDP sock\n");
        return NULL;
    }
    print_str("UDP echo server listening at port " QUOTE(PORT_UDP) "\n");
    while (1) {
        sock_udp_ep_t remote;
        ssize_t res;
        sock_udp_aux_rx_t rx_aux = { .flags = flags_rx };
        sock_udp_aux_tx_t tx_aux = { .flags = flags_tx };
        char buf[128];
        if (0 <= (res = sock_udp_recv_aux(&sock, buf, sizeof(buf),
                                          SOCK_NO_TIMEOUT, &remote, &rx_aux)))
            {
            print_str("Received a message via: [");
            if (!(rx_aux.flags & SOCK_AUX_GET_LOCAL)) {
                char tmp[IPV6_ADDR_MAX_STR_LEN + 1];
                ipv6_addr_to_str(tmp, (ipv6_addr_t *)rx_aux.local.addr.ipv6,
                                 sizeof(tmp));
                print_str(tmp);
                print_str("]:");
                print_u32_dec(rx_aux.local.port);
                print_str("\n");
            }
            else {
                print_str("Unknown endpoint\n");
            }
            print_str("Received a message at: ");
            if (!(rx_aux.flags & SOCK_AUX_GET_TIMESTAMP)) {
                print_u64_dec(rx_aux.timestamp);
                print_str(" ns\n");
            }
            else {
                print_str("No timestamp\n");
            }
            print_str("Received a message with: ");
            if (!(rx_aux.flags & SOCK_AUX_GET_RSSI)) {
                print_s32_dec(rx_aux.rssi);
                print_str(" dBm\n");
            }
            else {
                print_str("No RSSI value\n");
            }
            if (sock_udp_send_aux(&sock, buf, res, &remote, &tx_aux) < 0) {
                print_str("Error sending reply\n");
            }
            else {
                print_str("Sent echo at: ");
                if (!(tx_aux.flags & SOCK_AUX_GET_TIMESTAMP)) {
                    print_u64_dec(tx_aux.timestamp);
                    print_str(" ns\n");
                }
                else {
                    print_str("No timestamp\n");
                }
            }
        }
    }
    return 0;
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    thread_create(server_thread_stack, sizeof(server_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, 0,
                  server_thread, NULL, "UDP echo server");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
