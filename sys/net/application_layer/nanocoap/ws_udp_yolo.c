/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       nanoCoAP WebSocket transports
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "bitfield.h"
#include "net/nanocoap_sock.h"
#include "net/nanocoap_ws.h"
#include "net/sock/async.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static coap_ws_over_udp_yolo_init_arg_t _yolo_url_connect_handles[2];
static BITFIELD(_yolo_url_connect_handles_used, ARRAY_SIZE(_yolo_url_connect_handles));

static coap_ws_over_udp_yolo_conn_t * _yolo_new_conn(coap_ws_over_udp_yolo_init_arg_t *yolo)
{
    for (unsigned i = 0; i < ARRAY_SIZE(yolo->conns); i++) {
        if (yolo->conns[i].conn.handle == NULL) {
            yolo->conns[i].conn.handle = &yolo->handle;
            return &yolo->conns[i];
        }
    }

    return NULL;
}

static void _yolo_sock_udp_cb(sock_udp_t *sock, sock_async_flags_t flags,
                              void *arg)
{
    if (!(flags & SOCK_ASYNC_MSG_RECV)) {
        return;
    }
    coap_ws_over_udp_yolo_init_arg_t *yolo = arg;

    void *data, *buf_ctx = NULL;

    sock_udp_ep_t remote;
    ssize_t retval = sock_udp_recv_buf(sock, &data, &buf_ctx, 0, &remote);
    DEBUG("ws_yolo: sock_udp_recv_buf() --> data = %p, buf_ctx = %p\n",
          data, buf_ctx);
    if (retval < 0) {
        DEBUG("ws_yolo: recv failed: %" PRIdSIZE "\n", retval);
        return;
    }

    unsigned idx;
    for (idx = 0; idx < ARRAY_SIZE(yolo->conns); idx++) {
        if (sock_udp_ep_equal(&remote, &yolo->conns[idx].remote)) {
            yolo->cbs->conn_recv_cb(&yolo->conns[idx].conn, data, retval);
            break;
        }
    }

    if (idx >= ARRAY_SIZE(yolo->conns)) {
        /* message from unknown endpoint --> new connection */
        if (yolo->cbs->conn_accept_cb) {
            unsigned irq_state = irq_disable();
            coap_ws_over_udp_yolo_conn_t *conn = _yolo_new_conn(yolo);
            memcpy(&conn->remote, &remote, sizeof(conn->remote));
            irq_restore(irq_state);
            if (conn) {
                if (yolo->cbs->conn_accept_cb(&yolo->handle, &conn->conn)) {
                    yolo->cbs->conn_recv_cb(&conn->conn, data, retval);
                    DEBUG("ws_yolo: Accepted connection #%u on handle %p\n",
                          (unsigned)index_of(yolo->conns, conn), (void *)&yolo->handle);
                }
                else {
                    /* connection not accepted, closing it again */
                    irq_state = irq_disable();
                    memset(conn, 0, sizeof(*conn));
                    irq_restore(irq_state);
                }
            }
        }

    }

    /* free network stack owned buffer */
    retval = sock_udp_recv_buf(sock, &data, &buf_ctx, 0, NULL);
    assert((retval == 0) && (data == NULL));
}

static coap_ws_handle_t * _yolo_open_handle(const coap_ws_cbs_t *cbs, void *arg,
                                            const void *ep, size_t ep_len)
{
    if (ep_len != sizeof(sock_udp_ep_t)) {
        DEBUG_PUTS("ws_yolo: Failed to create handle: invalid listen endpoint");
        return NULL;
    }
    coap_ws_over_udp_yolo_init_arg_t *yolo = arg;
    memset(yolo, 0, sizeof(*yolo));
    int retval = sock_udp_create(&yolo->sock, ep, NULL, 0);
    if (retval) {
        DEBUG("ws_yolo: Failed to create handle: sock_udp_create() --> %d\n",
              retval);
        return NULL;
    }
    yolo->cbs = cbs;
    yolo->handle.transport = &coap_ws_over_udp_yolo;
    sock_udp_set_cb(&yolo->sock, _yolo_sock_udp_cb, arg);
    return &yolo->handle;
}

static void _yolo_close_handle(coap_ws_handle_t *handle)
{
    coap_ws_over_udp_yolo_init_arg_t *yolo = container_of(handle, coap_ws_over_udp_yolo_init_arg_t, handle);

    /* The memory of the connections handed to NanoCoAP are still owned by
     * NanoCoAP. Reclaim them to get ownership back */
    if (yolo->cbs->conn_reclaim_cb) {
        for (unsigned i = 0; i < ARRAY_SIZE(yolo->conns); i++) {
            if (yolo->conns[i].conn.handle != NULL) {
                yolo->cbs->conn_reclaim_cb(&yolo->conns[i].conn);
            }
        }
    }

    sock_udp_close(&yolo->sock);

    if (yolo->cleanup) {
        yolo->cleanup(yolo);
    }
    DEBUG("ws_yolo: Closed handle %p\n", (void *)handle);
}

static coap_ws_conn_t * _yolo_connect(coap_ws_handle_t *handle,
                                      const void *ep, size_t ep_len, void *arg)
{
    coap_ws_over_udp_yolo_init_arg_t *yolo = container_of(handle, coap_ws_over_udp_yolo_init_arg_t, handle);
    if (ep_len != sizeof(sock_udp_ep_t)) {
        DEBUG_PUTS("ws_yolo: Endpoint not a UPD endpint");
        return NULL;
    }

    unsigned irq_state = irq_disable();
    coap_ws_over_udp_yolo_conn_t *conn = _yolo_new_conn(yolo);
    if (!conn) {
        irq_restore(irq_state);
        DEBUG_PUTS("ws_yolo: No unused connection to hand out");
        return NULL;
    }

    conn->conn.arg = arg;
    memcpy(&conn->remote, ep, sizeof(conn->remote));
    irq_restore(irq_state);
    DEBUG("ws_yolo: Created connection #%u on handle %p\n",
          (unsigned)index_of(yolo->conns, conn), (void *)&yolo->handle);
    return &conn->conn;
}

static void _yolo_close(coap_ws_conn_t *conn)
{
    assert(conn->handle && (conn->handle->transport == &coap_ws_over_udp_yolo));
    coap_ws_over_udp_yolo_conn_t *yolo_conn = container_of(conn, coap_ws_over_udp_yolo_conn_t, conn);
    coap_ws_over_udp_yolo_init_arg_t *yolo = container_of(conn->handle, coap_ws_over_udp_yolo_init_arg_t, handle);

    DEBUG("ws_yolo: Closing connection #%u on handle %p\n",
          (unsigned)index_of(yolo->conns, yolo_conn), (void *)&yolo->handle);

    bool clean_handle = false;
    if (yolo->cbs->conn_reclaim_cb) {
        clean_handle = yolo->cbs->conn_reclaim_cb(conn);
    }

    unsigned irq_state = irq_disable();
    memset(yolo_conn, 0, sizeof(*yolo_conn));
    irq_restore(irq_state);

    if (clean_handle) {
        _yolo_close_handle(&yolo->handle);
    }
}

static size_t _yolo_remote_ep(const coap_ws_conn_t *conn, const void **ep)
{
    assert(conn->handle && (conn->handle->transport == &coap_ws_over_udp_yolo));
    assert(ep);
    coap_ws_over_udp_yolo_conn_t *yolo_conn = container_of(conn, coap_ws_over_udp_yolo_conn_t, conn);
    *ep = &yolo_conn->remote;
    return sizeof(yolo_conn->remote);
}

static int _yolo_sendv(coap_ws_conn_t *conn, const iolist_t *iol)
{
    assert(conn->handle && (conn->handle->transport == &coap_ws_over_udp_yolo));
    coap_ws_over_udp_yolo_conn_t *yolo_conn = container_of(conn, coap_ws_over_udp_yolo_conn_t, conn);
    coap_ws_over_udp_yolo_init_arg_t *yolo = container_of(conn->handle, coap_ws_over_udp_yolo_init_arg_t, handle);
    ssize_t retval = sock_udp_sendv(&yolo->sock, iol, &yolo_conn->remote);
    DEBUG("ws_yolo: Send to connection #%u on handle %p: %" PRIdSIZE "\n",
          (unsigned)index_of(yolo->conns, yolo_conn), (void *)&yolo->handle,
          retval);

    return (retval >= 0) ? 0 : (int)retval;
}

const coap_ws_transport_t coap_ws_over_udp_yolo = {
    .open_handle = _yolo_open_handle,
    .close_handle = _yolo_close_handle,
    .connect = _yolo_connect,
    .close = _yolo_close,
    .remote_ep = _yolo_remote_ep,
    .sendv = _yolo_sendv,
};

static void _handle_cleanup(coap_ws_over_udp_yolo_init_arg_t *yolo)
{
    size_t idx = index_of(_yolo_url_connect_handles, yolo);
    bf_unset_atomic(_yolo_url_connect_handles_used, idx);
    DEBUG("ws_yolo: Clean up handle for url_connect #%u\n", (unsigned)idx);
}

static int _yolo_url_connect(const char *url, nanocoap_sock_t *sock)
{
    if (strncmp(url, "coap+yolo://", 12)) {
        return -ENOTSUP;
    }

    coap_ws_over_udp_yolo_init_arg_t *arg = NULL;

    unsigned irq_state = irq_disable();
    size_t idx;
    for (idx = 0; idx < ARRAY_SIZE(_yolo_url_connect_handles); idx++) {
        if (!bf_isset(_yolo_url_connect_handles_used, idx)) {
            bf_set(_yolo_url_connect_handles_used, idx);
            arg = &_yolo_url_connect_handles[idx];
            break;
        }
    }
    irq_restore(irq_state);
    if (!arg) {
        DEBUG_PUTS("ws_yolo: URL connect failed: out of handles");
        return -ENOMEM;
    }

    char hostport[CONFIG_SOCK_HOSTPORT_MAXLEN];
    if (sock_urlsplit(url, hostport, NULL)) {
        bf_unset_atomic(_yolo_url_connect_handles_used, idx);
        DEBUG_PUTS("ws_yolo: URL connect failed: no host/port in URL");
        return -EINVAL;
    }

    sock_udp_ep_t remote;
    if (sock_udp_name2ep(&remote, hostport)) {
        bf_unset_atomic(_yolo_url_connect_handles_used, idx);
        DEBUG_PUTS("ws_yolo: URL connect failed: invalid host/port");
        return -EINVAL;
    }

    sock_udp_ep_t local = {
        .family = remote.family
    };

    if (remote.port == 0) {
        remote.port = 1337;
    }

    int retval = nanocoap_sock_ws_connect(sock, &coap_ws_over_udp_yolo, arg,
                                          &remote, &local, sizeof(remote));
    if (retval) {
        DEBUG("ws_yolo: nanocoap_sock_ws_connect() returned %d\n", retval);
        bf_unset_atomic(_yolo_url_connect_handles_used, idx);
        return retval;
    }

    arg->cleanup = _handle_cleanup;
    return 0;
}

NANOCOAP_SOCK_URL_CONNECT_HANDLER(_yolo_url_connect, 10);
