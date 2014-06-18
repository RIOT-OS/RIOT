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
#include "inet_pton.h"
#include "ringbuffer.h"
#include "record.h"
#include "dtls.h"

#define DTLS_LISTEN_BUFFER_SIZE 32

int dtls_listen(uint32_t port, dtls_listen_cb_t cb)
{
    dtls_connection_t conn = DTLS_CONNECTION_INIT;
    int status;
    ringbuffer_t rb;
    char buffer[DTLS_LISTEN_BUFFER_SIZE];
    uint8_t *fragment[DTLS_LISTEN_BUFFER_SIZE];
    dtls_record_t record = {{0},fragment};

    conn.socket  = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    memset(&conn.socket_addr, 0, sizeof(conn.socket_addr));
    conn.socket_addr.sin6_family = AF_INET;
    conn.socket_addr.sin6_port = HTONS(port);

    status = bind(conn.socket, (struct sockaddr *)&conn.socket_addr,
                  sizeof(conn.socket_addr));
    if (-1 == status)
    {
        close(conn.socket);
        return -1;
    }

    ringbuffer_init(&rb, buffer, 32);

    while (1)
    {
        if (dtls_record_receive(&conn, &rb, &record) < 0)
            continue;

        // TODO: add missing handshake HERE
        // if (conn->state == unknown)
        //   do handshake (like verify if record contains CLIENT_HELLO

        if (cb(&conn, record.fragment, record.header.length) == 1)
            break;
    }

    close(conn.socket);

    return status;
}


int dtls_connect(dtls_connection_t *conn, char *addr, uint32_t port)
{
    ipv6_addr_t ipv6_addr;

    if (!inet_pton(AF_INET6, addr, &ipv6_addr))
        return -1;

    conn->socket = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == conn->socket)
        return -1;

    memset(&conn->socket_addr, 0, sizeof(conn->socket_addr));
    conn->socket_addr.sin6_family = AF_INET;
    memcpy(&conn->socket_addr.sin6_addr, &ipv6_addr, 16);
    conn->socket_addr.sin6_port = HTONS(port);


    // TODO: add missing handshake HERE

    return 0;
}


int dtls_close(dtls_connection_t *conn)
{
    return close(conn->socket);
}
