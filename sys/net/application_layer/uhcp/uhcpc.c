/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "net/uhcp.h"

#include "net/af.h"
#include "net/conn/udp.h"
#include "net/ipv6/addr.h"
#include "xtimer.h"

static void _timeout(void *arg) {
    kernel_pid_t pid = *(kernel_pid_t*)arg;
    msg_t msg;
    msg_send_int(&msg, pid);
    msg_send_int(&msg, pid);
    msg_send_int(&msg, pid);
}

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
    ipv6_addr_t target;
    ipv6_addr_from_str(&target, "ff15::abcd");

    /* prepare UHCP header */
    uhcp_req_t req;
    uhcp_hdr_set(&req.hdr, UHCP_REQ);
    req.prefix_len = 64;

    /* create listening socket */
    ipv6_addr_t zero = {{0}};
    conn_udp_t conn;
    int res = conn_udp_create(&conn, &zero, 16, AF_INET6, UHCP_PORT);

    uint8_t srv_addr[16];
    size_t srv_addr_len;
    uint16_t srv_port;
    uint8_t buf[sizeof(uhcp_push_t) + 16];

    kernel_pid_t pid = thread_getpid();
    xtimer_t timeout;
    timeout.callback = _timeout;
    timeout.arg = &pid;

    while(1) {
        xtimer_set(&timeout, 10U*SEC_IN_USEC);
        puts("uhcp_client(): sending REQ...");
        conn_udp_sendto(&req, sizeof(uhcp_req_t), NULL, 0, &target, 16, AF_INET6 , 12345, 12345);
        res = conn_udp_recvfrom(&conn, buf, sizeof(buf), srv_addr, &srv_addr_len, &srv_port);
        if (res > 0) {
            xtimer_remove(&timeout);
            uhcp_handle_udp(buf, res, srv_addr, srv_port, iface);
            xtimer_sleep(60);
        }
        else {
            msg_t msg;
            msg_try_receive(&msg);
            msg_try_receive(&msg);
            msg_try_receive(&msg);
            puts("uhcp_client(): timeout waiting for reply");
        }
    }
}
