/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_modbus_tcp
 * @brief       Modbus TCP transport
 *
 * @{
 * @file
 * @brief       Modbus TCP transport implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "byteorder.h"
#include "container.h"
#include "net/sock/tcp.h"

#include "modbus.h"
#include "modbus_tcp.h"

#define ENABLE_DEBUG 0
#define ENABLE_TRACE 0
#include "debug.h"

#if ENABLE_TRACE
#  define TRACE(...) DEBUG(__VA_ARGS__)
#  define TRACE_PUTS(...) DEBUG_PUTS(__VA_ARGS__)
#else
#  define TRACE(...)
#  define TRACE_PUTS(...)
#endif

/**
 * @brief   The Modbus TCP protocol ID is always zero
 */
#define MODBUS_TCP_PROTOCOL_ID  (0)

/**
 * @brief   Read exactly @p len bytes into @p buf, looping over short reads
 *
 * @p consumed (may be @c NULL) is set to the number of bytes read, so the
 * caller can tell whether a failure happened at a frame boundary (no bytes
 * consumed, byte stream still in sync) or mid-frame (byte stream
 * desynchronized, connection must be dropped).
 */
static int _read_exact(sock_tcp_t *sock, void *buf, size_t len, uint32_t timeout,
                       size_t *consumed)
{
    size_t off = 0;
    uint8_t *p = (uint8_t *)buf;
    int ret = 0;

    while (off < len) {
        ssize_t res = sock_tcp_read(sock, p + off, len - off, timeout);

        if (res < 0) {
            ret = (int)res;
            break;
        }
        if (res == 0) {
            /* peer closed the connection */
            ret = -ECONNRESET;
            break;
        }
        off += (size_t)res;
    }

    if (consumed != NULL) {
        *consumed = off;
    }

    return ret;
}

static void _server_drop_active(modbus_tcp_t *dev)
{
    if (dev->state.server.active != NULL) {
        sock_tcp_disconnect(dev->state.server.active);
        dev->state.server.active = NULL;
    }
}

static int _client_connect(modbus_tcp_t *dev, const sock_tcp_ep_t *remote)
{
    int res = sock_tcp_connect(&dev->state.client.sock, remote, 0, 0);

    if (res < 0) {
        DEBUG("[modbus_tcp] _client_connect: failed (%d)\n", res);
        return res;
    }

    dev->state.client.connected = true;
    return 0;
}

static void _client_disconnect(modbus_tcp_t *dev)
{
    if (dev->state.client.connected) {
        sock_tcp_disconnect(&dev->state.client.sock);
        dev->state.client.connected = false;
    }
}

/**
 * @brief   Write @p buf to the client sock, transparently reconnecting once
 *          if the existing connection is stale.
 *
 * The retry covers the common case where the peer closed the connection
 * between two requests (e.g. server idle-timeout, brief network error): the
 * first @ref sock_tcp_write surfaces the broken pipe, we reconnect, and the
 * second attempt succeeds. The MBAP and PDU bytes in @p buf are unchanged
 * across retries, so the request is byte-identical (same transaction ID).
 */
static int _client_write(modbus_tcp_t *dev, const sock_tcp_ep_t *remote,
                         const void *buf, size_t len)
{
    if (!dev->state.client.connected) {
        int res = _client_connect(dev, remote);

        if (res != 0) {
            return res;
        }
    }

    ssize_t wres = sock_tcp_write(&dev->state.client.sock, buf, len);

    /* on any write error, drop and try once to reconnect + retry */
    if (wres < 0) {
        DEBUG("[modbus_tcp] _client_write: %d, reconnecting and retrying\n", (int)wres);
        _client_disconnect(dev);

        int res = _client_connect(dev, remote);
        if (res != 0) {
            return res;
        }

        wres = sock_tcp_write(&dev->state.client.sock, buf, len);
    }

    if (wres < 0) {
        _client_disconnect(dev);
        return (int)wres;
    }

    if ((size_t)wres != len) {
        _client_disconnect(dev);
        return -EIO;
    }

    return 0;
}

/**
 * @brief   Read exactly @p len bytes from the client sock.
 *
 * No retry on recv: if the connection drops mid-response we cannot replay the
 * request without risking duplicate execution on the server. Whether the
 * connection must be closed is decided by the caller, based on where in the
 * frame the failure happened (see @ref _client_recv).
 */
static int _client_read_exact(modbus_tcp_t *dev, void *buf, size_t len,
                              uint32_t timeout, size_t *consumed)
{
    if (!dev->state.client.connected) {
        if (consumed != NULL) {
            *consumed = 0;
        }
        return -ENOTCONN;
    }

    return _read_exact(&dev->state.client.sock, buf, len, timeout, consumed);
}

/**
 * @brief   Receive the response for the current transaction ID.
 *
 * A response timeout at a frame boundary (no bytes consumed) leaves the byte
 * stream in sync, so the connection is kept for the next request. As a
 * consequence, a late response to an earlier timed-out request can still
 * arrive in front of the response to the current request: such stale frames
 * (transaction ID older than the current one) are read and discarded here.
 *
 * The connection is only closed when the byte stream can no longer be trusted,
 * such as an error or timeout mid-frame, a protocol violation, or a
 * transaction ID that was never issued.
 */
static int _client_recv(modbus_tcp_t *dev, const modbus_tcp_params_t *params,
                        modbus_message_t *message)
{
    uint32_t timeout = (params->response_timeout != 0)
                       ? params->response_timeout
                       : SOCK_NO_TIMEOUT;

    while (1) {
        size_t consumed;

        /* read MBAP */
        int res = _client_read_exact(dev, dev->buf, MODBUS_TCP_MBAP_SIZE,
                                     timeout, &consumed);

        if (res != 0) {
            if (res == -ETIMEDOUT && consumed == 0) {
                /* no response (yet): still at a frame boundary, keep the
                 * connection for the next request */
                return -ETIMEDOUT;
            }

            DEBUG("[modbus_tcp] _client_recv: header read failed (%d), disconnecting\n", res);
            _client_disconnect(dev);
            return res;
        }

        uint16_t transaction_id = byteorder_bebuftohs(&dev->buf[0]);
        uint16_t proto = byteorder_bebuftohs(&dev->buf[2]);
        uint16_t length = byteorder_bebuftohs(&dev->buf[4]);

        if (proto != MODBUS_TCP_PROTOCOL_ID) {
            DEBUG("[modbus_tcp] _client_recv: unexpected protocol id %" PRIu16 "\n", proto);
            _client_disconnect(dev);
            return -EBADMSG;
        }
        if (length < 1 + MODBUS_PDU_MIN_RESPONSE_SIZE ||
            length > 1 + MODBUS_PDU_MAX_SIZE) {
            DEBUG("[modbus_tcp] _client_recv: invalid length %" PRIu16 "\n", length);
            _client_disconnect(dev);
            return -EBADMSG;
        }

        size_t pdu_size = length - 1U;

        /* read PDU into modbus->buffer (which aliases &dev->buf[7]) */
        res = _client_read_exact(dev, &dev->buf[MODBUS_TCP_MBAP_SIZE], pdu_size,
                                 timeout, NULL);
        if (res != 0) {
            DEBUG("[modbus_tcp] _client_recv: body read failed (%d), disconnecting\n",
                  res);
            _client_disconnect(dev);
            return res;
        }

        if (transaction_id == dev->transaction_id) {
            message->id = dev->buf[6];
            return (int)pdu_size;
        }

        /* stale response to an earlier timed-out request: discard the frame
         * and keep waiting for the current transaction ID */
        if ((uint16_t)(dev->transaction_id - transaction_id) < 0x8000U) {
            DEBUG("[modbus_tcp] _client_recv: discarding stale transaction id %" PRIu16
                  " (expected %" PRIu16 ")\n", transaction_id, dev->transaction_id);
            continue;
        }

        /* a transaction ID that was never issued: the stream cannot be trusted */
        DEBUG("[modbus_tcp] _client_recv: transaction id mismatch (expected %" PRIu16
              ", got %" PRIu16 ")\n", dev->transaction_id, transaction_id);
        _client_disconnect(dev);
        return -EPROTO;
    }
}

/**
 * @brief   Receive the next request, accepting a new connection as needed.
 *
 * A clean close by the client between two frames is a normal event for a TCP
 * server (the client is simply done), as is an idle timeout when
 * @ref modbus_tcp_params_t::idle_timeout is configured. Both drop the active
 * connection and transparently go back to accepting the next one, without
 * surfacing an error to the application. The idle timeout bounds how long a
 * dead connection (e.g. a lost FIN) can block the single connection slot.
 *
 * Failures mid-frame are real errors and are returned to the caller.
 */
static int _server_recv(modbus_tcp_t *dev, const modbus_tcp_params_t *params,
                        modbus_message_t *message)
{
    uint32_t timeout = (params->idle_timeout != 0)
                       ? params->idle_timeout
                       : SOCK_NO_TIMEOUT;

    while (1) {
        /* accept a new connection if no client is currently bound */
        if (dev->state.server.active == NULL) {
            int res = sock_tcp_accept(&dev->state.server.queue, &dev->state.server.active,
                                      SOCK_NO_TIMEOUT);

            if (res < 0) {
                DEBUG("[modbus_tcp] _server_recv: accept failed (%d)\n", res);
                return res;
            }
        }

        size_t consumed;

        /* read MBAP */
        int res = _read_exact(dev->state.server.active, dev->buf, MODBUS_TCP_MBAP_SIZE,
                              timeout, &consumed);

        if (res != 0) {
            if (consumed == 0 && (res == -ECONNRESET || res == -ETIMEDOUT)) {
                /* client closed the connection between frames, or the
                 * connection went idle: accept the next connection */
                DEBUG("[modbus_tcp] _server_recv: connection closed or idle (%d), "
                      "accepting next\n", res);
                _server_drop_active(dev);
                continue;
            }
            DEBUG("[modbus_tcp] _server_recv: header read failed (%d)\n", res);
            _server_drop_active(dev);
            return res;
        }

        uint16_t transaction_id = byteorder_bebuftohs(&dev->buf[0]);
        uint16_t proto = byteorder_bebuftohs(&dev->buf[2]);
        uint16_t length = byteorder_bebuftohs(&dev->buf[4]);

        if (proto != MODBUS_TCP_PROTOCOL_ID) {
            DEBUG("[modbus_tcp] _server_recv: unexpected protocol id %" PRIu16 "\n", proto);
            _server_drop_active(dev);
            return -EBADMSG;
        }

        if (length < 1 + MODBUS_PDU_MIN_REQUEST_SIZE ||
            length > 1 + MODBUS_PDU_MAX_SIZE) {
            DEBUG("[modbus_tcp] _server_recv: invalid length %" PRIu16 "\n", length);
            _server_drop_active(dev);
            return -EBADMSG;
        }

        size_t pdu_size = length - 1U;

        /* read PDU into modbus->buffer (which aliases &dev->buf[7]) */
        res = _read_exact(dev->state.server.active, &dev->buf[MODBUS_TCP_MBAP_SIZE], pdu_size,
                          timeout, NULL);
        if (res != 0) {
            DEBUG("[modbus_tcp] _server_recv: body read failed (%d)\n", res);
            _server_drop_active(dev);
            return res;
        }

        /* store the transaction ID so modbus_tcp_send can echo it in the response */
        dev->transaction_id = transaction_id;
        message->id = dev->buf[6];
        return (int)pdu_size;
    }
}

static int modbus_tcp_recv(modbus_t *modbus, modbus_message_t *message, bool request)
{
    modbus_tcp_t *dev = (modbus_tcp_t *)modbus->dev;
    const modbus_tcp_params_t *params = (const modbus_tcp_params_t *)modbus->params;

    DEBUG("[modbus_tcp] modbus_tcp_recv: request=%d\n", request);

    int pdu_size;

    if (params->mode == MODBUS_TCP_MODE_CLIENT) {
        if (request) {
            DEBUG_PUTS("[modbus_tcp] modbus_tcp_recv: cannot receive requests in client mode");
            return -ENOTSUP;
        }
        pdu_size = _client_recv(dev, params, message);
    }
    else {
        if (!request) {
            DEBUG_PUTS("[modbus_tcp] modbus_tcp_recv: cannot receive response in server mode");
            return -ENOTSUP;
        }
        pdu_size = _server_recv(dev, params, message);
    }

    if (pdu_size < 0) {
        return pdu_size;
    }

    modbus->buffer_size = (size_t)pdu_size;

    return 0;
}

static int modbus_tcp_send(modbus_t *modbus, const modbus_message_t *message)
{
    modbus_tcp_t *dev = (modbus_tcp_t *)modbus->dev;
    const modbus_tcp_params_t *params = (const modbus_tcp_params_t *)modbus->params;

    DEBUG("[modbus_tcp] modbus_tcp_send: size=%" PRIuSIZE "\n", modbus->buffer_size);

    /* assign a new transaction ID for outgoing requests (from client) */
    if (params->mode == MODBUS_TCP_MODE_CLIENT) {
        dev->transaction_id++;
    }

    /* build MBAP header in dev->buf[0..6], the PDU is already at dev->buf[7]
     * (modbus->buffer is wired to that slice in init) */
    byteorder_htobebufs(&dev->buf[0], dev->transaction_id);
    byteorder_htobebufs(&dev->buf[2], MODBUS_TCP_PROTOCOL_ID);
    byteorder_htobebufs(&dev->buf[4], (uint16_t)(1 + modbus->buffer_size));
    dev->buf[6] = message->id;

    size_t total = MODBUS_TCP_MBAP_SIZE + modbus->buffer_size;

    if (params->mode == MODBUS_TCP_MODE_CLIENT) {
        return _client_write(dev, &params->endpoint, dev->buf, total);
    }

    /* server: write on the currently accepted connection */
    if (dev->state.server.active == NULL) {
        DEBUG_PUTS("[modbus_tcp] modbus_tcp_send: no active server connection");
        return -ENOTCONN;
    }

    ssize_t res = sock_tcp_write(dev->state.server.active, dev->buf, total);

    if (res < 0) {
        DEBUG("[modbus_tcp] modbus_tcp_send: write failed (%d)\n", (int)res);
        _server_drop_active(dev);
        return (int)res;
    }
    if ((size_t)res != total) {
        return -EIO;
    }
    return 0;
}

static int modbus_tcp_init(modbus_t *modbus)
{
    modbus_tcp_t *dev = (modbus_tcp_t *)modbus->dev;
    const modbus_tcp_params_t *params = (const modbus_tcp_params_t *)modbus->params;

    assert(params != NULL);

    /* wire the PDU slice into the modbus context */
    modbus->buffer = &dev->buf[MODBUS_TCP_MBAP_SIZE];
    modbus->buffer_capacity = MODBUS_PDU_MAX_SIZE;
    modbus->buffer_size = 0;

    dev->transaction_id = 0;

    if (params->mode == MODBUS_TCP_MODE_CLIENT) {
        dev->state.client.connected = false;

        int res = _client_connect(dev, &params->endpoint);

        if (res != 0) {
            return res;
        }
    }
    else {
        dev->state.server.active = NULL;
        int res = sock_tcp_listen(&dev->state.server.queue, &params->endpoint,
                                  dev->state.server.pool,
                                  ARRAY_SIZE(dev->state.server.pool),
                                  0);

        if (res < 0) {
            DEBUG("[modbus_tcp] modbus_tcp_init: listen failed (%d)\n", res);
            return res;
        }
    }

    return 0;
}

const modbus_transport_desc_t modbus_tcp_transport = {
    .init = modbus_tcp_init,
    .send = modbus_tcp_send,
    .recv = modbus_tcp_recv,
};
