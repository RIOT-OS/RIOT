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
 * @brief       Modbus interface implementation
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 * @}
 */

#include <stdlib.h>
#include <string.h>

#include "byteorder.h"

#include "modbus.h"

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

static inline modbus_fc_t read_func(const modbus_t *modbus)
{
    assert(modbus->buffer_size >= 1);
    return modbus->buffer[0] & ~0x80;
}

static inline uint16_t read_address(const modbus_t *modbus)
{
    assert(modbus->buffer_size >= sizeof(uint16_t) + 1);
    return byteorder_bebuftohs(modbus->buffer + 1);
}

static inline uint16_t read_count(const modbus_t *modbus)
{
    assert(modbus->buffer_size >= sizeof(uint16_t) + 3);
    return byteorder_bebuftohs(modbus->buffer + 3);
}

static inline uint8_t read_size(const modbus_t *modbus)
{
    assert(modbus->buffer_size >= 6);
    return modbus->buffer[5];
}

static inline void write_func(modbus_t *modbus, modbus_fc_t func)
{
    modbus->buffer[0] = func;
}

static inline void write_exc(modbus_t *modbus, modbus_exc_t exc)
{
    assert(exc != MODBUS_EXC_NONE);
    modbus->buffer[1] = exc;
}

static inline void write_size(modbus_t *modbus, uint8_t size)
{
    modbus->buffer[1] = size;
}

static inline bool is_func_exc(const modbus_t *modbus)
{
    assert(modbus->buffer_size >= 1);
    return (modbus->buffer[0] & 0x80) == 0x80;
}

static int read_request(modbus_t *modbus, modbus_message_t *message)
{
    int res;

    /* the driver should read the identifier on start, so by setting it to an
     * invalid value, we can assert this assumption afterwards */
    message->id = MODBUS_ID_INVALID;

    /* func(1) */
    res = modbus->driver->recv(modbus, message, 1,
                               MODBUS_RECV_FLAG_REQUEST | MODBUS_RECV_FLAG_START);

    if (res != 0) {
        return res;
    }

    assert(message->id != MODBUS_ID_INVALID);

    /* with multiple bus members, a response message from another bus member
     * could be received as a request (which will fail processing), unless
     * we return early if we know nobody is interested in the message */
    modbus_server_t *server = modbus_server_iter(modbus, NULL, message->id);

    if (server == NULL) {
        DEBUG("[modbus_server] modbus_server_listen: no server found for id=%d\n",
              message->id);
        return -ENOENT;
    }

    message->func = read_func(modbus);
    message->exc = MODBUS_EXC_NONE;

    if (is_func_exc(modbus)) {
        DEBUG("[modbus_server] read_request: exception bit set in function code %u\n",
              message->func);
        return -EPROTO;
    }

    /* parse request */
    TRACE("[modbus_server] read_request: id=%d func=%d\n", message->id, message->func);

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* addr(2) + count(2) */
        res = modbus->driver->recv(modbus, message, 4,
                                   MODBUS_RECV_FLAG_REQUEST | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        message->addr = read_address(modbus);
        message->count = read_count(modbus);

        /* to support zero-copy for the response, the callback can directly
         * write to the buffer at the right index in the response message, but
         * it can also provide its own pointer if necessary */
        message->data = modbus->buffer + 2;
        message->data_size = modbus_bit_count_to_size(message->count);
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* addr(2) + count(2) */
        res = modbus->driver->recv(modbus, message, 4,
                                   MODBUS_RECV_FLAG_REQUEST | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        message->addr = read_address(modbus);
        message->count = read_count(modbus);

        /* to support zero-copy for the response, the callback can directly
         * write to the buffer at the right index in the response message, but
         * it can also provide its own pointer if necessary */
        message->data = modbus->buffer + 2;
        message->data_size = modbus_reg_count_to_size(message->count);
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        /* addr(2) + data(2) */
        res = modbus->driver->recv(modbus, message, 4,
                                   MODBUS_RECV_FLAG_REQUEST | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        message->addr = read_address(modbus);
        message->count = 1;
        message->data = modbus->buffer + 3;
        message->data_size = 2; /* always 2 bytes */
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* addr(2) + data(2) */
        res = modbus->driver->recv(modbus, message, 4,
                                   MODBUS_RECV_FLAG_REQUEST | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        message->addr = read_address(modbus);
        message->count = 1;
        message->data = modbus->buffer + 3;
        message->data_size = modbus_reg_count_to_size(1);
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* addr(2) + count(2) + size(1) */
        res = modbus->driver->recv(modbus, message, 5,
                                   MODBUS_RECV_FLAG_REQUEST | MODBUS_RECV_FLAG_MORE);

        if (res != 0) {
            return res;
        }

        message->addr = read_address(modbus);
        message->count = read_count(modbus);
        message->data_size = read_size(modbus);

        /* data(size) */
        res = modbus->driver->recv(modbus, message, message->data_size,
                                   MODBUS_RECV_FLAG_REQUEST | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        message->data = modbus->buffer + 6;
        break;

    default:
        message->exc = MODBUS_EXC_ILLEGAL_FUNCTION;
        break;
    }

    return 0;
}

static inline int dispatch_request(modbus_t *modbus, modbus_message_t *message)
{
    int res;

    modbus_server_t *server = modbus_server_iter(modbus, NULL, message->id);

    while (server != NULL) {
        res = server->cb(modbus, server, message);

        if (res != 0) {
            DEBUG(
                "[modbus_server] modbus_server_listen: server callback failed for id=%d (%d)\n",
                server->id, res);
            return res;
        }

        server = modbus_server_iter(modbus, server, message->id);
    }

    return 0;
}

static inline int write_response(modbus_t *modbus, modbus_message_t *message)
{
    uint8_t size;

    /* func(1) */
    write_func(modbus, message->func);
    modbus->buffer_size = 1;

    /* handle exception responses */
    if (message->exc != MODBUS_EXC_NONE) {
        write_func(modbus, message->func | 0x80);
        write_exc(modbus, message->exc);
        modbus->buffer_size = 2;
    }

    /* handle normal responses */
    else {
        switch (message->func) {
        case MODBUS_FC_READ_COILS:
        case MODBUS_FC_READ_DISCRETE_INPUTS:
            /* size(1) + data(count) */
            size = modbus_bit_count_to_size(message->count);
            write_size(modbus, size);
            /* copy data if callback provided a different data buffer */
            if (message->data != modbus->buffer + 2) {
                assert(message->data_size >= size);
                memcpy(modbus->buffer + 2, message->data, size);
            }
            message->data_size = size;
            modbus->buffer_size += (1 + size);
            break;

        case MODBUS_FC_READ_HOLDING_REGISTERS:
        case MODBUS_FC_READ_INPUT_REGISTERS:
            /* size(2) + data(size) */
            size = modbus_reg_count_to_size(message->count);
            write_size(modbus, size);
            /* copy data if callback provided a different data buffer */
            if (message->data != modbus->buffer + 2) {
                assert(message->data_size >= size);
                memcpy(modbus->buffer + 2, message->data, size);
            }
            message->data_size = size;
            modbus->buffer_size += (1 + size);
            break;

        case MODBUS_FC_WRITE_SINGLE_COIL:
        case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
            /* addr(2) + data(size) */
            /* the response equals the request */
            modbus->buffer_size += 4;
            /* reset data pointer to prevent unintended/incorrect use */
            message->data_size = 0;
            message->data = NULL;
            break;

        case MODBUS_FC_WRITE_MULTIPLE_COILS:
        case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
            /* addr(2) + count(2) */
            /* the response equals the request, minus the data */
            modbus->buffer_size = 5;
            /* reset data pointer to prevent unintended/incorrect use */
            message->data_size = 0;
            message->data = NULL;
            break;

        default:
            /* should not happen if response is checked before sending */
            modbus->buffer_size = 0;
            break;
        }
    }

    return modbus->driver->send(modbus, message, MODBUS_SEND_FLAG_RESPONSE);
}

int modbus_server_listen(modbus_t *modbus, modbus_message_t *message)
{
    assert(message != NULL);

    int res;

    TRACE_PUTS("[modbus_server] modbus_server_listen: waiting for request");

    /* read request from client */
    mutex_lock(&(modbus->lock));

    res = read_request(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_server] modbus_server_listen: read request failed (%d)\n", res);
        mutex_unlock(&(modbus->lock));
        return res;
    }

    mutex_unlock(&(modbus->lock));

    /* dispatch message to server(s) */
    res = dispatch_request(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_server] modbus_server_listen: dispatch request failed (%d)\n", res);
        return res;
    }

    return 0;
}

int modbus_server_reply(modbus_t *modbus, modbus_message_t *message)
{
    assert(message != NULL);
    assert(message->id != MODBUS_ID_BROADCAST);

    int res;

    TRACE("[modbus_server] modbus_server_reply: sending reply to id=%d\n", message->id);

    /* write response to client */
    mutex_lock(&(modbus->lock));

    res = write_response(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_server] modbus_server_reply: write response failed (%d)\n", res);
        mutex_unlock(&(modbus->lock));
        return res;
    }

    mutex_unlock(&(modbus->lock));

    return 0;
}

int modbus_server_add(modbus_t *modbus, modbus_server_t *server)
{
    assert(modbus != NULL);
    assert(server != NULL);

    if (modbus_server_get(modbus, server->id) != NULL) {
        DEBUG("[modbus] modbus_server_add: server with id=%d already exists\n", server->id);
        return -EEXIST;
    }

    server->next = modbus->servers;
    modbus->servers = server;

    return 0;
}

int modbus_server_remove(modbus_t *modbus, modbus_server_t *server)
{
    assert(modbus != NULL);
    assert(server != NULL);

    modbus_server_t *prev = NULL;
    modbus_server_t *curr = modbus->servers;

    while (curr != NULL) {
        if (curr == server) {
            if (prev == NULL) {
                modbus->servers = curr->next;
            }
            else {
                prev->next = curr->next;
            }

            return 0;
        }

        prev = curr;
        curr = curr->next;
    }

    return -ENOENT;
}

modbus_server_t * modbus_server_get(modbus_t *modbus, uint8_t id)
{
    assert(modbus != NULL);
    assert(id != MODBUS_ID_BROADCAST);

    modbus_server_t *server = modbus->servers;

    while (server != NULL) {
        if (server->id == id) {
            return server;
        }

        server = server->next;
    }

    return NULL;
}

modbus_server_t * modbus_server_iter(modbus_t *modbus, modbus_server_t *prev, uint8_t id)
{
    assert(modbus != NULL);

    modbus_server_t *server = (prev == NULL) ? modbus->servers : prev->next;

    while (server != NULL) {
        if (server->id == id) {
            return server;
        }
        else if (id == MODBUS_ID_BROADCAST && server->flags & MODBUS_SERVER_FLAG_RECEIVE_BROADCAST) {
            return server;
        }
        else if (server->flags & MODBUS_SERVER_FLAG_RECEIVE_ANY) {
            return server;
        }

        server = server->next;
    }

    return NULL;
}
