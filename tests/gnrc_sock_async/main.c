/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       TODO
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "event/timeout.h"
#include "net/sock/udp.h"
#include "net/gnrc/netif.h"
#include "od.h"

#define PRINT_ADDR_TIME         (5U * US_PER_SEC)

#define RECV_BUFFER_SIZE        (128)

#define SOCK_EVENT_PRINT_ADDR   (0x80000000)

#define UDP_PORT                (1337U)

static void _print_addrs(event_t *event);

static uint8_t recv_buffer[128];
static event_queue_t sock_event_queue;
static event_t print_addr_event = { .handler = _print_addrs };
static event_timeout_t print_addr_timeout;
static sock_udp_t sock;

static void _print_addrs(event_t *event)
{
    (void)event;
    ipv6_addr_t addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    int res;

    if ((netif != NULL) &&
        ((res = gnrc_netif_ipv6_addrs_get(netif, addrs, sizeof(addrs))) > 0)) {
        puts("Send a UDP packet to the following addresses:");
        for (unsigned i = 0; i < (res / sizeof(ipv6_addr_t)); i++) {
            printf("[");
            ipv6_addr_print(&addrs[i]);
            printf("]:%u\n", UDP_PORT);
        }
    }
    event_timeout_set(&print_addr_timeout, PRINT_ADDR_TIME);
}

static void _recv(event_t *e)
{
    sock_event_t *event = (sock_event_t *)e;

    if (event->type & SOCK_EVENT_RECV) {
        sock_udp_ep_t remote;
        ssize_t res;

        while ((res = sock_udp_recv(&sock, recv_buffer, sizeof(recv_buffer),
                                    0, &remote)) >= 0) {
            printf("Received from [");
            ipv6_addr_print((ipv6_addr_t *)remote.addr.ipv6);
            printf("%%%i]:%u:\n", remote.netif, remote.port);
            od_hex_dump(recv_buffer, res, OD_WIDTH_DEFAULT);
        }
    }
}

int main(void)
{
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;

    local.port = UDP_PORT;
    sock_udp_create(&sock, &local, NULL, 0);

    event_queue_init(&sock_event_queue);
    event_timeout_init(&print_addr_timeout, &sock_event_queue,
                       &print_addr_event);
    sock_udp_set_event_queue(&sock, &sock_event_queue, _recv);
    event_timeout_set(&print_addr_timeout, PRINT_ADDR_TIME);
    event_loop(&sock_event_queue);
    return 0;
}
