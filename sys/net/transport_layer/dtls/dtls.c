/**
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 */

#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include "inet_pton.h"
#include "ringbuffer.h"
#include "record.h"
#include "dtls.h"
#include "crypto/ciphers.h"

sockaddr6_t g_socket_addr;

int dtls_listen(uint32_t port, dtls_listen_cb_t cb)
{
    dtls_connection_t conn = DTLS_CONNECTION_INIT;
    int status;
    char buffer[DTLS_BUFFER_SIZE];
    uint8_t *fragment[DTLS_BUFFER_SIZE];
    dtls_record_header_t header = DTLS_RECORD_HEADER_INIT;
    dtls_record_t record = {&header,fragment};

    conn.socket  = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    memset(&g_socket_addr, 0, sizeof(g_socket_addr));
    g_socket_addr.sin6_family = AF_INET;
    g_socket_addr.sin6_port = HTONS(port);
//    conn.socket_addr.sin6_port = HTONS(port);

    status = bind(conn.socket, (struct sockaddr *)&g_socket_addr,
                  sizeof(g_socket_addr));
    if (status < 0)
    {
        close(conn.socket);
        return -1;
    }

    cipher_init(&conn.cipher, CIPHER_NULL, NULL, 0);
    ringbuffer_init(&conn.buffer, buffer, DTLS_BUFFER_SIZE);

    if (dtls_handshake_verify(&conn, &record, DTLS_HANDSHAKE_CLIENT_HELLO) < 0)
        return -1;

    while (1)
    {
        if (dtls_record_receive(&conn, &record) < 0)
            continue;

        if (cb(&conn, record.fragment, record.header->length) == 1)
            break;
    }

    close(conn.socket);

    return status;
}


int dtls_connect(dtls_connection_t *conn, char *addr, uint32_t port)
{
    ipv6_addr_t ipv6_addr;
    char buffer[DTLS_BUFFER_SIZE];
    int status;
    uint8_t *fragment[DTLS_BUFFER_SIZE];
    dtls_record_header_t header = DTLS_RECORD_HEADER_INIT;
    dtls_record_t record = {&header,fragment};

    if (!inet_pton(AF_INET6, addr, &ipv6_addr))
        return -1;

    conn->socket = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == conn->socket)
        return -1;


    memset(&conn->socket_addr, 0, sizeof(conn->socket_addr));
    memcpy(&conn->socket_addr.sin6_addr, &ipv6_addr, 16);
    conn->socket_addr.sin6_family = AF_INET;
    conn->socket_addr.sin6_port = HTONS(port);

    memset(&g_socket_addr, 0, sizeof(g_socket_addr));
    g_socket_addr.sin6_family = AF_INET;
    g_socket_addr.sin6_port = HTONS(port);

    status = bind(conn->socket, (struct sockaddr *)&g_socket_addr,
                  sizeof(g_socket_addr));
    if (status < 0)
    {
        printf("Socket error : %s\n", strerror(errno));
        close(conn->socket);
        return -1;
    }

    /** NDP BUG WORKAROUND START**/
    if (!ndp_neighbor_cache_search(&ipv6_addr)) {
        ndp_neighbor_cache_add(0, &ipv6_addr, &(ipv6_addr.uint16[7]), 2, 0,
                           NDP_NCE_STATUS_REACHABLE,
                           NDP_NCE_TYPE_TENTATIVE,
                           0xffff);
    }
    /** NDP BUG WORKAROUND END **/

    cipher_init(&conn->cipher, CIPHER_NULL, NULL, 0);
    ringbuffer_init(&conn->buffer, buffer, DTLS_BUFFER_SIZE);

    if( dtls_handshake_start(conn, &record) < 0)
        return -1;

    return 0;
}


int dtls_send(dtls_connection_t *conn, uint8_t *data, size_t size)
{
    /* TODO: check state, was there a successful handshake? is conn intialized?*/

    return dtls_record_stream_send(conn, TLS_CONTENT_TYPE_APPLICATION_DATA,
              data, size);
}


int dtls_close(dtls_connection_t *conn)
{
    return close(conn->socket);
}
