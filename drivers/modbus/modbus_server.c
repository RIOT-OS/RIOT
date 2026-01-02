/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_modbus
 * @brief       Modbus
 *
 * @{
 * @file
 * @brief       Modbus server implementation.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "modbus.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _dispatch_request(modbus_t *modbus, modbus_message_t *message)
{
    int res;

    mutex_lock(&modbus->servers_lock);
    modbus_server_t *server = modbus_server_iter(modbus, NULL, message->id);
    mutex_unlock(&modbus->servers_lock);

    if (server == NULL) {
        DEBUG("[modbus_server] _dispatch_request: no server found for id=%d\n",
              message->id);
        return -ENOENT;
    }

    while (server != NULL) {
        res = server->cb(modbus, server, message);

        if (res != 0) {
            DEBUG(
                "[modbus_server] _dispatch_request: server callback failed for id=%d (%d)\n",
                server->id, res);
            return res;
        }

        mutex_lock(&modbus->servers_lock);
        server = modbus_server_iter(modbus, server, message->id);
        mutex_unlock(&modbus->servers_lock);
    }

    return 0;
}

static int _do_listen(modbus_t *modbus, modbus_message_t *message)
{
    /* let the transport receive a full ADU (which should also set
     * modbus->buffer and message->id) */
    message->id = MODBUS_ID_INVALID;

    int res = modbus->transport->recv(modbus, message, true);

    if (res != 0) {
        DEBUG("[modbus_server] _do_listen: recv failed (%d)\n", res);
        return res;
    }

    if (message->id == MODBUS_ID_INVALID) {
        DEBUG_PUTS("[modbus_server] _do_listen: transport failed to set id");
        return -EPROTO;
    }

    /* with multiple bus members, a response message from another bus member
     * could be received as a request (which would fail processing), so return
     * early if we know no server is interested in the message */
    mutex_lock(&modbus->servers_lock);

    if (modbus_server_iter(modbus, NULL, message->id) == NULL) {
        DEBUG("[modbus_server] _do_listen: no server found for id=%d\n",
              message->id);
        mutex_unlock(&modbus->servers_lock);
        return -ENOENT;
    }

    mutex_unlock(&modbus->servers_lock);

    /* decode the request PDU */
    return modbus_decode_request(modbus->buffer, modbus->buffer_size, message);
}

static int _do_reply(modbus_t *modbus, modbus_message_t *message)
{
    /* encode the response PDU */
    ssize_t pdu_size = modbus_encode_response(modbus->buffer, modbus->buffer_capacity, message);

    if (pdu_size == 0) {
        DEBUG("[modbus_server] _do_reply: unsupported function code %d\n",
              message->func);
        return -EBADMSG;
    }
    if (pdu_size < 0) {
        DEBUG("[modbus_server] _do_reply: encode failed (%d)\n", (int)pdu_size);
        return (int)pdu_size;
    }
    modbus->buffer_size = (size_t)pdu_size;

    int res = modbus->transport->send(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_server] _do_reply: send failed (%d)\n", res);
        return res;
    }

    /* invalidate the data pointer for write responses: the buffer the
     * pointer references is reused on the next request, so leaving it
     * dangling would invite use-after-write bugs in callbacks */
    if (message->exc == MODBUS_EXC_NONE) {
        switch (message->func) {
        case MODBUS_FC_WRITE_SINGLE_COIL:
        case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
        case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
            message->data = NULL;
            message->data_size = 0;
            break;
        default:
            break;
        }
    }

    return 0;
}

int modbus_server_listen(modbus_t *modbus, modbus_message_t *message)
{
    assert(modbus != NULL);
    assert(message != NULL);

    DEBUG_PUTS("[modbus_server] modbus_server_listen: waiting for request");

    /* listen for incoming messages */
    mutex_lock(&modbus->lock);

    int res = _do_listen(modbus, message);

    mutex_unlock(&modbus->lock);

    if (res != 0) {
        return res;
    }

    /* dispatch message to server(s) */
    res = _dispatch_request(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_server] modbus_server_listen: dispatch failed (%d)\n", res);
        return res;
    }

    return 0;
}

int modbus_server_reply(modbus_t *modbus, modbus_message_t *message)
{
    assert(modbus != NULL);
    assert(message != NULL);
    assert(message->id != MODBUS_ID_BROADCAST);

    DEBUG("[modbus_server] modbus_server_reply: sending reply to id=%d\n", message->id);

    mutex_lock(&modbus->lock);
    int res = _do_reply(modbus, message);

    mutex_unlock(&modbus->lock);

    return res;
}

int modbus_server_add(modbus_t *modbus, modbus_server_t *server)
{
    assert(modbus != NULL);
    assert(server != NULL);

    if (server->id < MODBUS_ID_MIN || server->id > MODBUS_ID_MAX) {
        DEBUG("[modbus_server] modbus_server_add: invalid server id=%d\n", server->id);
        return -EINVAL;
    }

    mutex_lock(&modbus->servers_lock);

    if (modbus_server_get(modbus, server->id) != NULL) {
        DEBUG("[modbus_server] modbus_server_add: server with id=%d already exists\n", server->id);
        mutex_unlock(&modbus->servers_lock);
        return -EEXIST;
    }

    server->_next = modbus->servers;
    modbus->servers = server;

    mutex_unlock(&modbus->servers_lock);

    return 0;
}

int modbus_server_remove(modbus_t *modbus, modbus_server_t *server)
{
    assert(modbus != NULL);
    assert(server != NULL);

    mutex_lock(&modbus->servers_lock);

    modbus_server_t *prev = NULL;
    modbus_server_t *curr = modbus->servers;

    while (curr != NULL) {
        if (curr == server) {
            if (prev == NULL) {
                modbus->servers = curr->_next;
            }
            else {
                prev->_next = curr->_next;
            }

            mutex_unlock(&modbus->servers_lock);

            return 0;
        }

        prev = curr;
        curr = curr->_next;
    }

    mutex_unlock(&modbus->servers_lock);

    return -ENOENT;
}

modbus_server_t * modbus_server_get(const modbus_t *modbus, uint8_t id)
{
    assert(modbus != NULL);
    assert(id != MODBUS_ID_BROADCAST);

    modbus_server_t *server = modbus->servers;

    while (server != NULL) {
        if (server->id == id) {
            return server;
        }

        server = server->_next;
    }

    return NULL;
}

modbus_server_t * modbus_server_iter(const modbus_t *modbus, modbus_server_t *prev, uint8_t id)
{
    assert(modbus != NULL);

    modbus_server_t *server = (prev == NULL) ? modbus->servers : prev->_next;

    while (server != NULL) {
        if (server->id == id) {
            return server;
        }
        else if (id == MODBUS_ID_BROADCAST &&
                 server->flags & MODBUS_SERVER_FLAG_RECEIVE_BROADCAST) {
            return server;
        }
        else if (server->flags & MODBUS_SERVER_FLAG_RECEIVE_ANY) {
            return server;
        }

        server = server->_next;
    }

    return NULL;
}
