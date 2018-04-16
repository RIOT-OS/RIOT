/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <arpa/inet.h>
#include <stdio.h>

#include "net/af.h"
#include "net/sock/udp.h"
#include "net/uhcp.h"
#include "xtimer.h"

/**
 * @brief Request prefix from uhcp server
 *
 * Never returns.
 * Calls @c uhcp_handle_prefix() when a prefix or prefix change is received.
 *
 * @param[in]   iface   interface to request prefix on
 */
void uhcp_client(uhcp_iface_t iface)
{
    sock_udp_t sock;
    sock_udp_ep_t local = { .family=AF_INET6, .port=UHCP_PORT, .netif=iface };
    sock_udp_ep_t req_target = { .family=AF_INET6, .port=UHCP_PORT, .netif=iface };
    sock_udp_ep_t remote;

    inet_pton(AF_INET6, "ff15::abcd", req_target.addr.ipv6);

    /* prepare UHCP header */
    uhcp_req_t req;
    uhcp_hdr_set(&req.hdr, UHCP_REQ);
    req.prefix_len = 64;

    /* create listening socket */
    int res = sock_udp_create(&sock, &local, NULL, 0);

    uint8_t buf[sizeof(uhcp_push_t) + 16];

    while(1) {
        puts("uhcp_client(): sending REQ...");
        sock_udp_send(&sock, &req, sizeof(uhcp_req_t), &req_target);
        res = sock_udp_recv(&sock, buf, sizeof(buf), 10U*US_PER_SEC, &remote);
        if (res > 0) {
            uhcp_handle_udp(buf, res, remote.addr.ipv6, remote.port, iface);
            xtimer_sleep(60);
        }
        else {
            puts("uhcp_client(): no reply received");
        }
    }
}
