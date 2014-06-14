/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "net_if.h"
#include "sixlowpan.h"
#include "sys/socket.h"
#include "inet_pton.h"
#include "../../sys/net/transport_layer/dtls/dtls.h"
#include "../../sys/net/transport_layer/dtls/record.h"

#define IF_ID   (0)

int main(void)
{
    uint8_t data[] = {1,2,3,4,5,6,7,8};
    uint8_t data2[] = {1};
    dtls_connection_t conn = DTLS_CONNECTION_INIT;
    ipv6_addr_t dest;
    
    if (!inet_pton(AF_INET6, "ff02::1:0:ff00:6af4", &dest)) {
        printf("ERROR: could not set IPv6 address\n");
        return -1;
    }
    
    // neighbour discovery protocol buggy in RIOT, so we need to add
    // neighbours manually.
    if (!ndp_neighbor_cache_search(&dest)) {
        ndp_neighbor_cache_add(IF_ID, &dest, &(dest.uint16[7]), 2, 0,
                               NDP_NCE_STATUS_REACHABLE,
                               NDP_NCE_TYPE_TENTATIVE,
                               0xffff);
    }

    
    conn.socket = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == conn.socket) {
        printf("Error Creating Socket!");
        return -1;
    }

    memset(&conn.socket_addr, 0, sizeof(conn.socket_addr));
    conn.socket_addr.sin6_family = AF_INET;
    memcpy(&conn.socket_addr.sin6_addr, &dest, 16);
    conn.socket_addr.sin6_port = HTONS(8443);

    dtls_record_stream_send(&conn, TLS_CONTENT_TYPE_CHANGE_CIPHER_SPEC, data2, 1);
    dtls_record_stream_send(&conn, TLS_CONTENT_TYPE_APPLICATION_DATA, data, 8);

    return 0;
}
