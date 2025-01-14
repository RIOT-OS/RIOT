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

#include "net/nanocoap_sock.h"
#include "time_units.h"
#include "ztimer.h"

#if MODULE_NANOCOAP_SERVER_TCP
#  include "event/thread.h"
#endif

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#if MODULE_NANOCOAP_SERVER_TCP
static nanocoap_tcp_server_ctx_t tcp_ctx;
#endif

#if MODULE_NANOCOAP_SERVER_WS && MODULE_NANOCOAP_WS_UDP_YOLO
static coap_ws_over_udp_yolo_init_arg_t _ws_ctx;
#endif

int main(void)
{
    puts("RIOT nanocoap example application");

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    ztimer_sleep(ZTIMER_USEC, 3 * US_PER_SEC);

    /* print network addresses */
    printf("{\"IPv6 addresses\": [\"");
    netifs_print_ipv6("\", \"");
    puts("\"]}");

#if MODULE_NANOCOAP_SERVER_TCP
    nanocoap_server_tcp(&tcp_ctx, EVENT_PRIO_MEDIUM, NULL);
    printf("CoAP+TCP on PORT %u\n", (unsigned)tcp_ctx.local.port);
#endif

#if MODULE_NANOCOAP_SERVER_WS && MODULE_NANOCOAP_WS_UDP_YOLO
    sock_udp_ep_t local_ws = { .port = 1337, .family = AF_INET6 };
    nanocoap_server_ws(&coap_ws_over_udp_yolo, &_ws_ctx, &local_ws, sizeof(local_ws));
    printf("CoAP+YOLO on PORT %u\n", (unsigned)local_ws.port);
#endif

#if MODULE_NANOCOAP_UDP
    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    printf("CoAP (UDP) on PORT %u\n", (unsigned)local.port);
    nanocoap_server_udp(&local, buf, sizeof(buf));
#endif

    /* should be never reached */
    return 0;
}
