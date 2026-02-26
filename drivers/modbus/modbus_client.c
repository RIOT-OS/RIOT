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

static inline modbus_exc_t read_exc(const modbus_t *modbus)
{
    assert(modbus->buffer_size >= 2);
    return modbus->buffer[1];
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
    assert(modbus->buffer_size >= 2);
    return modbus->buffer[1];
}

static inline void write_func(modbus_t *modbus, modbus_fc_t func)
{
    modbus->buffer[0] = func;
}

static inline void write_address(modbus_t *modbus, uint16_t addr)
{
    byteorder_htobebufs(modbus->buffer + 1, addr);
}

static inline void write_count(modbus_t *modbus, uint16_t count)
{
    byteorder_htobebufs(modbus->buffer + 3, count);
}

static inline void write_size(modbus_t *modbus, uint8_t size)
{
    modbus->buffer[5] = size;
}

static inline bool is_func_exc(const modbus_t *modbus)
{
    assert(modbus->buffer_size >= 1);
    return (modbus->buffer[0] & 0x80) == 0x80;
}

static int read_response(modbus_t *modbus, modbus_message_t *message)
{
    int res;
    uint16_t size;

    uint8_t expected_id = message->id;
    modbus_fc_t expected_func = message->func;

    /* the driver should read the identifier on start, so by setting it to an
     * invalid value, we can assert this assumption afterwards */
    message->id = MODBUS_ID_INVALID;

    /* func(1) */
    res = modbus->driver->recv(modbus, message, 1,
                               MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_START);

    if (res != 0) {
        return res;
    }

    assert(message->id != MODBUS_ID_INVALID);

    /* response must have same id */
    if (expected_id != message->id) {
        DEBUG("[modbus_client] read_response: expected id=%d but got id=%d\n",
              expected_id, message->id);
        return -EPROTO;
    }

    message->func = read_func(modbus);
    message->exc = MODBUS_EXC_NONE;

    /* response must have same function code */
    if (message->func != expected_func) {
        DEBUG("[modbus_client] read_response: expected func=0x%d but got func=0x%d\n",
              expected_func, message->func);
        return -EPROTO;
    }

    /* handle exception response */
    if (is_func_exc(modbus)) {
        /* exception(1) */
        res = modbus->driver->recv(modbus, message, 1,
                                   MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        uint8_t exception = read_exc(modbus);

        if (exception == MODBUS_EXC_NONE) {
            DEBUG_PUTS(
                "[modbus_client] read_response: exception bit set, but code is MODBUS_EXC_NONE");
            return -EPROTO;
        }

        message->exc = exception;
        return 0;
    }

    /* parse response */
    TRACE("[modbus_client] read_response: id=%d func=%d\n", message->id, message->func);

    switch (read_func(modbus)) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* size(1) */
        res = modbus->driver->recv(modbus, message, 1,
                                   MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_MORE);

        if (res != 0) {
            return res;
        }

        size = read_size(modbus);

        res = modbus->driver->recv(modbus, message, size,
                                   MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        size = modbus_bit_count_to_size(message->count);

        if (read_size(modbus) != size) {
            return -EPROTO;
        }

        /* to support zero-copy, point the message data to the buffer at the
         * right index */
        if (message->data == NULL) {
            message->data = modbus->buffer + 2;
        }
        else {
            assert(message->data_size >= size);
            memcpy(message->data, modbus->buffer + 2, size);
        }
        message->data_size = size;
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* size(1) */
        res = modbus->driver->recv(modbus, message, 1,
                                   MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_MORE);

        if (res != 0) {
            return res;
        }

        size = read_size(modbus);

        res = modbus->driver->recv(modbus, message, size,
                                   MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }

        size = modbus_reg_count_to_size(message->count);
        if (read_size(modbus) != size) {
            return -EPROTO;
        }

        /* to support zero-copy, point the message data to the buffer at the
         * right index */
        if (message->data == NULL) {
            message->data = modbus->buffer + 2;
        }
        else {
            assert(message->data_size >= size);
            memcpy(message->data, modbus->buffer + 2, size);
        }
        message->data_size = size;
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* addr(2) + data(2) */
        res = modbus->driver->recv(modbus, message, 4,
                                   MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }
        if (read_address(modbus) != message->addr) {
            return -EPROTO;
        }
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* addr(2) + count(2) */
        res = modbus->driver->recv(modbus, message, 4,
                                   MODBUS_RECV_FLAG_RESPONSE | MODBUS_RECV_FLAG_STOP);

        if (res != 0) {
            return res;
        }
        if (read_address(modbus) != message->addr) {
            return -EPROTO;
        }
        if (read_count(modbus) != message->count) {
            return -EPROTO;
        }
        break;

    default:
        return -EPROTO;
        break;
    }

    return 0;
}

static int write_request(modbus_t *modbus, modbus_message_t *message)
{
    uint8_t size;

    TRACE("[modbus_client] write_request: id=%d func=%d\n", message->id, message->func);

    /* func(1) */
    write_func(modbus, message->func);

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* addr(2) + count(2) */
        write_address(modbus, message->addr);
        write_count(modbus, message->count);
        modbus->buffer_size = 5;
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        /* addr(2) + data(2) */
        size = 2; /* always 2 bytes */
        write_address(modbus, message->addr);
        assert(message->data_size >= size);
        memcpy(&modbus->buffer[3], message->data, size);
        modbus->buffer_size = 5;
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        /* addr(2) + count(2) + size(1) + data(size) */
        size = modbus_bit_count_to_size(message->count);
        write_address(modbus, message->addr);
        write_count(modbus, message->count);
        write_size(modbus, size);
        assert(message->data_size >= size);
        memcpy(modbus->buffer + 6, message->data, size);
        modbus->buffer_size = 6 + size;
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* addr(2) + data(2) */
        size = modbus_reg_count_to_size(1);
        write_address(modbus, message->addr);
        assert(message->data_size >= size);
        memcpy(modbus->buffer + 3, message->data, size);
        modbus->buffer_size = 3 + size;
        break;

    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* addr(2) + count(2) + size(1) + data(size) */
        size = modbus_reg_count_to_size(message->count);
        write_address(modbus, message->addr);
        write_count(modbus, message->count);
        write_size(modbus, size);
        assert(message->data_size >= size);
        memcpy(modbus->buffer + 6, message->data, size);
        modbus->buffer_size = 6 + size;
        break;

    default:
        return -EBADMSG;
        break;
    }

    return modbus->driver->send(modbus, message, MODBUS_SEND_FLAG_REQUEST);
}

int modbus_client_request(modbus_t *modbus, modbus_message_t *message)
{
    assert(message != NULL);
    assert(message->id != MODBUS_ID_BROADCAST);

    int res;

    DEBUG("[modbus_client] modbus_client_request: sending request to %d\n",
          message->id);

    /* write request to server */
    mutex_lock(&(modbus->lock));

    res = write_request(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_client] modbus_client_request: write request failed (%d)\n", res);
        mutex_unlock(&(modbus->lock));
        return res;
    }

    /* wait for response from server */
    res = read_response(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_client] modbus_client_request: read response failed (%d)\n", res);
        mutex_unlock(&(modbus->lock));
        return res;
    }

    mutex_unlock(&(modbus->lock));

    return 0;
}

int modbus_client_broadcast(modbus_t *modbus, modbus_message_t *message)
{
    assert(message != NULL);

    int res;

    DEBUG_PUTS("[modbus_client] modbus_client_request: sending broadcast\n");

    /* set broadcast identifier */
    message->id = MODBUS_ID_BROADCAST;

    /* write request to all servers */
    mutex_lock(&(modbus->lock));

    res = write_request(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_client] modbus_client_broadcast: broadcast request failed (%d)\n", res);
        mutex_unlock(&(modbus->lock));
        return res;
    }

    mutex_unlock(&(modbus->lock));

    return 0;
}
