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
 * @brief       Modbus protocol data unit (PDU) translation between message
 *              types and a byte buffers.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "architecture.h"
#include "byteorder.h"

#include "modbus.h"

#define ENABLE_DEBUG 0
#include "debug.h"

ssize_t modbus_encode_request(uint8_t *buf, size_t len, const modbus_message_t *message)
{
    assert(buf != NULL);
    assert(message != NULL);

    size_t size;

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* func(1) + addr(2) + count(2) */
        if (len < 5) {
            return -ENOBUFS;
        }
        buf[0] = message->func;
        byteorder_htobebufs(buf + 1, message->addr);
        byteorder_htobebufs(buf + 3, message->count);
        return 5;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        /* func(1) + addr(2) + data(2) */
        if (len < 5) {
            return -ENOBUFS;
        }
        if (message->data == NULL || message->data_size < 2) {
            return -EINVAL;
        }
        buf[0] = message->func;
        byteorder_htobebufs(buf + 1, message->addr);
        memcpy(buf + 3, message->data, 2);
        return 5;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* func(1) + addr(2) + data(2) */
        size = modbus_reg_count_to_size(1);
        if (len < 3 + size) {
            return -ENOBUFS;
        }
        if (message->data == NULL || message->data_size < size) {
            return -EINVAL;
        }
        buf[0] = message->func;
        byteorder_htobebufs(buf + 1, message->addr);
        memcpy(buf + 3, message->data, size);
        return (ssize_t)(3 + size);

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        /* func(1) + addr(2) + count(2) + size(1) + data(size) */
        size = modbus_bit_count_to_size(message->count);
        if (len < 6 + size) {
            return -ENOBUFS;
        }
        if (message->data == NULL || message->data_size < size || size > UINT8_MAX) {
            return -EINVAL;
        }
        buf[0] = message->func;
        byteorder_htobebufs(buf + 1, message->addr);
        byteorder_htobebufs(buf + 3, message->count);
        buf[5] = (uint8_t)size;
        memcpy(buf + 6, message->data, size);
        return (ssize_t)(6 + size);

    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* func(1) + addr(2) + count(2) + size(1) + data(size) */
        size = modbus_reg_count_to_size(message->count);
        if (len < 6 + size) {
            return -ENOBUFS;
        }
        if (message->data == NULL || message->data_size < size || size > UINT8_MAX) {
            return -EINVAL;
        }
        buf[0] = message->func;
        byteorder_htobebufs(buf + 1, message->addr);
        byteorder_htobebufs(buf + 3, message->count);
        buf[5] = (uint8_t)size;
        memcpy(buf + 6, message->data, size);
        return (ssize_t)(6 + size);

    default:
        return 0;
    }
}

ssize_t modbus_encode_response(uint8_t *buf, size_t len, const modbus_message_t *message)
{
    assert(buf != NULL);
    assert(message != NULL);

    size_t size;

    /* exception responses set the exception bit on the function code */
    if (message->exc != MODBUS_EXC_NONE) {
        if (len < 2) {
            return -ENOBUFS;
        }
        buf[0] = message->func | 0x80;
        buf[1] = message->exc;
        return 2;
    }

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* func(1) + size(1) + data(size) */
        size = modbus_bit_count_to_size(message->count);
        if (len < 2 + size) {
            return -ENOBUFS;
        }
        if (message->data == NULL || size > UINT8_MAX) {
            return -EINVAL;
        }
        buf[0] = message->func;
        buf[1] = (uint8_t)size;
        if (message->data != buf + 2) {
            if (message->data_size < size) {
                return -EINVAL;
            }
            memcpy(buf + 2, message->data, size);
        }
        return (ssize_t)(2 + size);

    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* func(1) + size(1) + data(size) */
        size = modbus_reg_count_to_size(message->count);
        if (len < 2 + size) {
            return -ENOBUFS;
        }
        if (message->data == NULL || size > UINT8_MAX) {
            return -EINVAL;
        }
        buf[0] = message->func;
        buf[1] = (uint8_t)size;
        if (message->data != buf + 2) {
            if (message->data_size < size) {
                return -EINVAL;
            }
            memcpy(buf + 2, message->data, size);
        }
        return (ssize_t)(2 + size);

    case MODBUS_FC_WRITE_SINGLE_COIL:
    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* echo of request: func(1) + addr(2) + data(2) */
        if (len < 5) {
            return -ENOBUFS;
        }
        if (message->data == NULL || message->data_size < 2) {
            return -EINVAL;
        }
        buf[0] = message->func;
        byteorder_htobebufs(buf + 1, message->addr);
        /* data may already be in place when the caller decoded the request
         * into the same buffer (zero-copy) */
        if (message->data != buf + 3) {
            memcpy(buf + 3, message->data, 2);
        }
        return 5;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* abbreviated echo of request: func(1) + addr(2) + count(2) */
        if (len < 5) {
            return -ENOBUFS;
        }
        buf[0] = message->func;
        byteorder_htobebufs(buf + 1, message->addr);
        byteorder_htobebufs(buf + 3, message->count);
        return 5;

    default:
        return 0;
    }
}

int modbus_decode_request(uint8_t *buf, size_t len, modbus_message_t *message)
{
    assert(buf != NULL);
    assert(message != NULL);

    uint16_t max;

    if (len < MODBUS_PDU_MIN_REQUEST_SIZE) {
        return -EBADMSG;
    }

    /* the exception bit must not be set on a request */
    if (buf[0] & 0x80) {
        DEBUG("[modbus_pdu] modbus_decode_request: exception bit set in function code 0x%02x\n",
              buf[0]);
        return -EPROTO;
    }

    message->func = buf[0];
    message->exc = MODBUS_EXC_NONE;

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* addr(2) + count(2) */
        if (len < 5) {
            return -EBADMSG;
        }
        message->addr = byteorder_bebuftohs(buf + 1);
        message->count = byteorder_bebuftohs(buf + 3);
        if (message->count < 1 || message->count > 2000) {
            DEBUG("[modbus_pdu] modbus_decode_request: invalid read bits count %" PRIu16 "\n",
                  message->count);
            return -ERANGE;
        }
        /* zero-copy for the response, callback may override */
        message->data = buf + 2;
        message->data_size = modbus_bit_count_to_size(message->count);
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* addr(2) + count(2) */
        if (len < 5) {
            return -EBADMSG;
        }
        message->addr = byteorder_bebuftohs(buf + 1);
        message->count = byteorder_bebuftohs(buf + 3);
        if (message->count < 1 || message->count > 125) {
            DEBUG("[modbus_pdu] modbus_decode_request: invalid read registers count %" PRIu16 "\n",
                  message->count);
            return -ERANGE;
        }
        /* zero-copy for the response, callback may override */
        message->data = buf + 2;
        message->data_size = modbus_reg_count_to_size(message->count);
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        /* addr(2) + data(2) */
        if (len < 5) {
            return -EBADMSG;
        }
        message->addr = byteorder_bebuftohs(buf + 1);
        message->count = 1;
        message->data = buf + 3;
        message->data_size = 2;
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* addr(2) + data(2) */
        if (len < 5) {
            return -EBADMSG;
        }
        message->addr = byteorder_bebuftohs(buf + 1);
        message->count = 1;
        message->data = buf + 3;
        message->data_size = modbus_reg_count_to_size(1);
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* addr(2) + count(2) + size(1) + data(size) */
        if (len < 6) {
            return -EBADMSG;
        }
        message->addr = byteorder_bebuftohs(buf + 1);
        message->count = byteorder_bebuftohs(buf + 3);
        message->data_size = buf[5];
        if (len < 6 + message->data_size) {
            return -EBADMSG;
        }
        /* reject out-of-range quantities: a callback trusting count could
         * otherwise read or write past the received data */
        max = (message->func == MODBUS_FC_WRITE_MULTIPLE_COILS) ? 1968 : 123;
        if (message->count < 1 || message->count > max) {
            DEBUG("[modbus_pdu] modbus_decode_request: invalid write multiple count %" PRIu16 "\n",
                  message->count);
            return -ERANGE;
        }
        /* the declared byte count must match the requested quantity, otherwise
         * a callback trusting count would read past the received data */
        size_t expected = (message->func == MODBUS_FC_WRITE_MULTIPLE_COILS)
                        ? modbus_bit_count_to_size(message->count)
                        : modbus_reg_count_to_size(message->count);

        if (message->data_size != expected) {
            DEBUG("[modbus_pdu] modbus_decode_request: byte count %" PRIuSIZE " does not "
                  "match quantity %" PRIu16 "\n", message->data_size, message->count);
            return -EBADMSG;
        }
        message->data = buf + 6;
        break;

    default:
        message->exc = MODBUS_EXC_ILLEGAL_FUNCTION;
        break;
    }

    return 0;
}

int modbus_decode_response(uint8_t *buf, size_t len, modbus_message_t *message)
{
    assert(buf != NULL);
    assert(message != NULL);

    size_t size;

    if (len < MODBUS_PDU_MIN_RESPONSE_SIZE) {
        return -EBADMSG;
    }

    message->func = buf[0] & ~0x80;
    message->exc = MODBUS_EXC_NONE;

    /* exception response */
    if (buf[0] & 0x80) {
        if (buf[1] == MODBUS_EXC_NONE) {
            DEBUG_PUTS("[modbus_pdu] modbus_decode_response: exception bit set, but code is "
                       "MODBUS_EXC_NONE");
            return -EPROTO;
        }
        message->exc = buf[1];
        return 0;
    }

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* size(1) + data(size) */
        size = buf[1];
        if (len < 2 + size) {
            return -EBADMSG;
        }
        if (size != modbus_bit_count_to_size(message->count)) {
            return -EPROTO;
        }
        if (message->data == NULL) {
            message->data = buf + 2;
        }
        else {
            if (message->data_size < size) {
                return -ENOBUFS;
            }
            memcpy(message->data, buf + 2, size);
        }
        message->data_size = size;
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* size(1) + data(size) */
        size = buf[1];
        if (len < 2 + size) {
            return -EBADMSG;
        }
        if (size != modbus_reg_count_to_size(message->count)) {
            return -EPROTO;
        }
        if (message->data == NULL) {
            message->data = buf + 2;
        }
        else {
            if (message->data_size < size) {
                return -ENOBUFS;
            }
            memcpy(message->data, buf + 2, size);
        }
        message->data_size = size;
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* addr(2) + data(2) */
        if (len < 5) {
            return -EBADMSG;
        }
        if (byteorder_bebuftohs(buf + 1) != message->addr) {
            return -EPROTO;
        }
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* addr(2) + count(2) */
        if (len < 5) {
            return -EBADMSG;
        }
        if (byteorder_bebuftohs(buf + 1) != message->addr) {
            return -EPROTO;
        }
        if (byteorder_bebuftohs(buf + 3) != message->count) {
            return -EPROTO;
        }
        break;

    default:
        return -EPROTO;
    }

    return 0;
}
