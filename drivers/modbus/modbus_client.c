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
 * @brief       Modbus client implementation.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "modbus.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _encode_and_send(modbus_t *modbus, modbus_message_t *message)
{
    /* encode the request PDU */
    ssize_t pdu_size = modbus_encode_request(modbus->buffer, modbus->buffer_capacity, message);

    if (pdu_size == 0) {
        DEBUG("[modbus_client] _encode_and_send: unsupported function code %d\n",
              message->func);
        return -EBADMSG;
    }
    if (pdu_size < 0) {
        DEBUG("[modbus_client] _encode_and_send: encode failed (%d)\n", (int)pdu_size);
        return (int)pdu_size;
    }
    modbus->buffer_size = (size_t)pdu_size;

    /* send the request */
    int res = modbus->transport->send(modbus, message);

    if (res != 0) {
        DEBUG("[modbus_client] _encode_and_send: send failed (%d)\n", res);
    }

    return res;
}

static int _do_request(modbus_t *modbus, modbus_message_t *message)
{
    int res = _encode_and_send(modbus, message);

    if (res != 0) {
        return res;
    }

    /* remember what we expect to come back */
    uint8_t expected_id = message->id;
    modbus_fc_t expected_func = message->func;

    /* let the transport receive a full ADU (which should also set
     * modbus->buffer and message->id) */
    message->id = MODBUS_ID_INVALID;

    res = modbus->transport->recv(modbus, message, false);

    if (res != 0) {
        DEBUG("[modbus_client] _do_request: recv failed (%d)\n", res);
        return res;
    }

    if (message->id == MODBUS_ID_INVALID) {
        DEBUG_PUTS("[modbus_client] _do_request: transport failed to set id");
        return -EPROTO;
    }

    /* the response must come from the addressed server */
    if (message->id != expected_id) {
        DEBUG("[modbus_client] _do_request: expected id=%d but got id=%d\n",
              expected_id, message->id);
        return -EPROTO;
    }

    /* decode the response PDU */
    res = modbus_decode_response(modbus->buffer, modbus->buffer_size, message);

    if (res != 0) {
        DEBUG("[modbus_client] _do_request: decode failed (%d)\n", res);
        return res;
    }

    /* the response must echo the request's function code */
    if (message->func != expected_func) {
        DEBUG("[modbus_client] _do_request: expected func=%d but got func=%d\n",
              expected_func, message->func);
        return -EPROTO;
    }

    return 0;
}

int modbus_client_request(modbus_t *modbus, modbus_message_t *message)
{
    assert(modbus != NULL);
    assert(message != NULL);
    assert(message->id != MODBUS_ID_BROADCAST);

    DEBUG("[modbus_client] modbus_client_request: sending request to %d\n",
          message->id);

    /* perform request */
    mutex_lock(&modbus->lock);

    int res = _do_request(modbus, message);

    mutex_unlock(&modbus->lock);

    return res;
}

int modbus_client_broadcast(modbus_t *modbus, modbus_message_t *message)
{
    assert(modbus != NULL);
    assert(message != NULL);

    DEBUG_PUTS("[modbus_client] modbus_client_broadcast: sending broadcast");

    /* set broadcast identifier */
    message->id = MODBUS_ID_BROADCAST;

    /* perform request (no response expected for broadcasts) */
    mutex_lock(&modbus->lock);

    int res = _encode_and_send(modbus, message);

    mutex_unlock(&modbus->lock);

    return res;
}
