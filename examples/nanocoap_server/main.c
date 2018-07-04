/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"

#include "xtimer.h"

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* import "ifconfig" shell command, used for printing addresses */
extern int _gnrc_netif_config(int argc, char **argv);

int main(void)
{
    puts("RIOT nanocoap example application");

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");
    _gnrc_netif_config(0, NULL);

    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
#ifdef MODULE_SOCK_SECURE
    sock_udp_ep_t local = { .port=COAP_PORT + 1, .family=AF_INET6 };
#else
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
#endif

#if 0
    (void) local;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    remote.port = 5684;
    char addr_str[] = "fe80::ac9f:5dff:feb3:866e";
    char path[] = "/riot/board";
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6, addr_str);

    nanocoap_get(&remote, path, buf, COAP_INBUF_SIZE);
#else
    nanocoap_server(&local, buf, sizeof(buf));
#endif
    /* should be never reached */
    return 0;
}
