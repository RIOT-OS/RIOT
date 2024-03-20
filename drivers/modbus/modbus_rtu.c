/*
 * Copyright (C) 2020-2023 Denis Litvinov <li.denis.iv@gmail.com>
 * Copyright (C) 2023-2024 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_modbus
 * @brief       Modbus
 *
 * @{
 * @file
 * @brief       Modbus interface implementation
 *
 * @author      Denis Litvinov <li.denis.iv@gmail.com>
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "modbus.h"
#include "modbus_rtu.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "byteorder.h"
#include "checksum/ucrc16.h"
#include "msg.h"
#include "mutex.h"
#include "thread.h"
#include "timex.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_MODBUS_RTU

enum {
    ID = 0,     /* ID field */
    FUNC,       /* Function code position */
    ADD_HI,     /* Address high byte */
    ADD_LO,     /* Address low byte */
    NB_HI,      /* Number of coils or registers high byte */
    NB_LO,      /* Number of coils or registers low byte */
    BYTE_CNT    /* byte counter */
};

static uint16_t crc_calculate(const uint8_t *buf, size_t len)
{
    return ucrc16_calc_le(buf, len, 0xA001, 0xFFFF);
}

static bool crc_check(const uint8_t *buf, size_t len)
{
    assert(len > 2);

    uint16_t actual = crc_calculate(buf, len - 2);
    uint16_t expected = (buf[len - 2] << 0) |
                        (buf[len - 1] << 8);

    if (actual != expected) {
        DEBUG("[modbus_rtu] crc_check: actual 0x%04x != expected 0x%04x\n", actual, expected);
    }

    return actual == expected;
}

static inline uint16_t read_address(modbus_rtu_t *modbus)
{
    return byteorder_bebuftohs(modbus->buffer + ADD_HI);
}

static inline void write_address(modbus_rtu_t *modbus, uint16_t addr)
{
    byteorder_htobebufs(modbus->buffer + ADD_HI, addr);
}

static inline uint16_t read_count(modbus_rtu_t *modbus)
{
    return byteorder_bebuftohs(modbus->buffer + NB_HI);
}

static inline void write_count(modbus_rtu_t *modbus, uint16_t count)
{
    byteorder_htobebufs(modbus->buffer + NB_HI, count);
}

static inline void write_size(modbus_rtu_t *modbus, uint8_t size, bool request)
{
    modbus->buffer[request ? BYTE_CNT : 2] = size;
}

static inline uint8_t read_size(modbus_rtu_t *modbus, bool request)
{
    return modbus->buffer[request ? BYTE_CNT : 2];
}

static inline void enable_rts(modbus_rtu_t *modbus)
{
    if (gpio_is_valid(modbus->params->pin_rts)) {
        gpio_write(modbus->params->pin_rts, modbus->params->pin_rts_enable);
    }
}

static inline void disable_rts(modbus_rtu_t *modbus)
{
    if (gpio_is_valid(modbus->params->pin_rts)) {
        gpio_write(modbus->params->pin_rts, !modbus->params->pin_rts_enable);
    }
}

static inline void send(modbus_rtu_t *modbus)
{
    uint16_t crc = crc_calculate(modbus->buffer, modbus->buffer_size);

    modbus->buffer[modbus->buffer_size++] = (crc >> 0) & 0xff;
    modbus->buffer[modbus->buffer_size++] = (crc >> 8) & 0xff;

    enable_rts(modbus);
    DEBUG("[modbus_rtu] send: %d bytes\n", modbus->buffer_size);
    uart_write(modbus->params->uart, modbus->buffer, modbus->buffer_size);
    disable_rts(modbus);
}

static int receive(modbus_rtu_t *modbus, uint8_t expected_size)
{
    msg_t msg;

    while (modbus->buffer_size < expected_size) {
        if (ztimer_msg_receive_timeout(ZTIMER_USEC, &msg, modbus->rx_timeout) < 0) {
            return -1;
        }
    }

    return 0;
}

static void uart_cb(void *arg, uint8_t data)
{
    static msg_t msg;
    modbus_rtu_t *modbus = (modbus_rtu_t *)arg;

    //DEBUG("[modbus_rtu] uart_cb: received byte 0x%02x\n", data);

    if (mutex_trylock(&(modbus->buffer_mutex)) == 0) {
        return;
    }

    modbus->buffer[modbus->buffer_size++] = data;

    msg_send(&msg, modbus->pid);

    mutex_unlock(&(modbus->buffer_mutex));
}

static inline int write_request(modbus_rtu_t *modbus, modbus_message_t *message)
{
    uint8_t size;

    mutex_lock(&(modbus->buffer_mutex));

    modbus->buffer[ID] = message->id;
    modbus->buffer[FUNC] = message->func;

    switch (message->func) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* id + func + addr + count */
        write_address(modbus, message->addr);
        write_count(modbus, message->count);
        modbus->buffer_size = 6;
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        /* id + func + addr + data */
        write_address(modbus, message->addr);
        assert(message->data_size >= 1);
        modbus->buffer[4] = (message->data[0] & 0x01) ? 0xff : 0x00;
        modbus->buffer[5] = 0;
        modbus->buffer_size = 6;
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        /* (id + func + addr + count + size) + data */
        size = modbus_bit_count_to_size(message->count);
        write_address(modbus, message->addr);
        write_count(modbus, message->count);
        write_size(modbus, size, true);
        assert(message->data_size >= size);
        memcpy(modbus->buffer + 7, message->data, size);
        modbus->buffer_size = 7 + size;
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* id + func + addr + data */
        size = modbus_reg_count_to_size(1);
        write_address(modbus, message->addr);
        assert(message->data_size >= size);
        memcpy(modbus->buffer + 4, message->data, size);
        modbus->buffer_size = 4 + size;
        break;

    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* (id + func + addr + count + size) + data */
        size = modbus_reg_count_to_size(message->count);
        write_address(modbus, message->addr);
        write_count(modbus, message->count);
        write_size(modbus, size, true);
        assert(message->data_size >= size);
        memcpy(modbus->buffer + 7, message->data, size);
        modbus->buffer_size = 7 + size;
        break;

    default:
        mutex_unlock(&(modbus->buffer_mutex));
        return MODBUS_ERR_REQUEST;
        break;
    }

    send(modbus);

    mutex_unlock(&(modbus->buffer_mutex));

    return MODBUS_OK;
}

static int read_response(modbus_rtu_t *modbus, modbus_message_t *message)
{
    msg_t msg;
    uint16_t size;

    /* no response is expected for broadcasted requests */
    if (message->id == MODBUS_ID_BROADCAST) {
        return MODBUS_OK;
    }

    modbus->buffer_size = 0;
    modbus->pid = thread_getpid();

    /* wait for answer */
    if (ztimer_msg_receive_timeout(ZTIMER_USEC, &msg, modbus->timeout) < 0) {
        return MODBUS_ERR_TIMEOUT;
    }

    /* read response */
    if (modbus->buffer[ID] != message->id) {
        return MODBUS_ERR_RESPONSE;
    }

    if (receive(modbus, MODBUS_RTU_PACKET_RESPONSE_SIZE_MIN)) {
        return MODBUS_ERR_TIMEOUT;
    }

    /* response must have same function code */
    if ((modbus->buffer[FUNC] & ~0x80) != message->func) {
        return MODBUS_ERR_RESPONSE;
    }

    /* handle exception response */
    if (modbus->buffer[FUNC] & 0x80) {
        message->exc = modbus->buffer[2];
        return MODBUS_OK;
    }

    /* parse response */
    switch (modbus->buffer[FUNC]) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* id + func + size + data + crc */
        size = 3 + read_size(modbus, false) + MODBUS_RTU_PACKET_CRC_SIZE;
        if (receive(modbus, size)) {
            return MODBUS_ERR_TIMEOUT;
        }
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, size)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        size = modbus_bit_count_to_size(message->count);
        if (read_size(modbus, false) != size) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_RESPONSE;
        }
        /* to support zero-copy, point the message data to the buffer at the
           right index */
        if (message->data == NULL) {
            message->data = modbus->buffer + 3;
        }
        else {
            assert(message->data_size >= size);
            memcpy(message->data, modbus->buffer + 3, size);
        }
        mutex_unlock(&(modbus->buffer_mutex));
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* id + func + size + data + crc */
        size = 3 + read_size(modbus, false) + MODBUS_RTU_PACKET_CRC_SIZE;
        if (receive(modbus, size)) {
            return MODBUS_ERR_TIMEOUT;
        }
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, size)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        size = modbus_reg_count_to_size(message->count);
        if (read_size(modbus, false) != size) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_RESPONSE;
        }
        /* to support zero-copy, point the message data to the buffer at the
           right index */
        if (message->data == NULL) {
            message->data = modbus->buffer + 3;
        }
        else {
            assert(message->data_size >= size);
            memcpy(message->data, modbus->buffer + 3, size);
        }
        mutex_unlock(&(modbus->buffer_mutex));
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* id + func + addr + data + crc */
        if (receive(modbus, MODBUS_RTU_PACKET_REQUEST_SIZE_MIN)) {
            return MODBUS_ERR_TIMEOUT;
        }
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, modbus->buffer_size)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        if (read_address(modbus) != message->addr) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_RESPONSE;
        }
        mutex_unlock(&(modbus->buffer_mutex));
        break;
    case MODBUS_FC_WRITE_MULTIPLE_COILS:
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* id + func + addr + count + crc */
        if (receive(modbus, MODBUS_RTU_PACKET_REQUEST_SIZE_MIN)) {
            return MODBUS_ERR_TIMEOUT;
        }
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, modbus->buffer_size)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        if (read_address(modbus) != message->addr) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_RESPONSE;
        }
        if (read_count(modbus) != message->count) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_RESPONSE;
        }
        mutex_unlock(&(modbus->buffer_mutex));
        break;

    default:
        return MODBUS_ERR_RESPONSE;
        break;
    }

    return MODBUS_OK;
}

static int read_request(modbus_rtu_t *modbus, modbus_message_t *message)
{
    uint16_t size;

    modbus->buffer_size = 0;
    modbus->pid = thread_getpid();

    if (receive(modbus, MODBUS_RTU_PACKET_REQUEST_SIZE_MIN)) {
        return MODBUS_ERR_TIMEOUT;
    }

    message->id = modbus->buffer[ID];
    message->func = modbus->buffer[FUNC];
    message->exc = MODBUS_EXC_NONE;

    switch (modbus->buffer[FUNC]) {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        /* id + func + addr + count + crc */
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, MODBUS_RTU_PACKET_REQUEST_SIZE_MIN)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        message->addr = read_address(modbus);
        message->count = read_count(modbus);

        /* to support zero-copy, the callback can directly write to the
           buffer at the right index, but it can also provide its own pointer
           if necessary */
        message->data = modbus->buffer + 3;
        message->data_size = modbus_bit_count_to_size(message->count);
        break;

    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_READ_INPUT_REGISTERS:
        /* id + func + addr + count + crc */
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, MODBUS_RTU_PACKET_REQUEST_SIZE_MIN)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        message->addr = read_address(modbus);
        message->count = read_count(modbus);

        /* to support zero-copy, the callback can directly write to the
           buffer at the right index, but it can also provide its own pointer
           if necessary */
        message->data = modbus->buffer + 3;
        message->data_size = modbus_reg_count_to_size(message->count);
        break;

    case MODBUS_FC_WRITE_SINGLE_COIL:
        /* id + func + addr + data + crc */
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, MODBUS_RTU_PACKET_REQUEST_SIZE_MIN)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        // TODO: check FF00 or 0000.
        message->addr = read_address(modbus);
        message->count = 1;
        message->data = modbus->buffer + 4;
        message->data_size = 1;
        break;

    case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
        /* id + func + addr + data + crc */
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, MODBUS_RTU_PACKET_REQUEST_SIZE_MIN)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        message->addr = read_address(modbus);
        message->count = 1;
        message->data = modbus->buffer + 4;
        message->data_size = 2;
        break;

    case MODBUS_FC_WRITE_MULTIPLE_COILS:
    case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
        /* id + func + addr + count + size + data + crc */
        size = 7 + read_size(modbus, true) + MODBUS_RTU_PACKET_CRC_SIZE;
        if (receive(modbus, size)) {
            return MODBUS_ERR_TIMEOUT;
        }
        mutex_lock(&(modbus->buffer_mutex));
        if (!crc_check(modbus->buffer, size)) {
            mutex_unlock(&(modbus->buffer_mutex));
            return MODBUS_ERR_CRC;
        }
        message->addr = read_address(modbus);
        message->count = read_count(modbus);
        message->data = modbus->buffer + 7;
        message->data_size = read_size(modbus, true);
        break;

    default:
        message->exc = MODBUS_EXC_ILLEGAL_FUNCTION;
        break;
    }

    return MODBUS_OK;
}

static int handle_request(modbus_rtu_t *modbus, modbus_message_t *message, modbus_request_cb_t cb)
{
    int res = cb((modbus_t *)modbus, message);

    if (res == MODBUS_DROP) {
        mutex_unlock(&(modbus->buffer_mutex));
        DEBUG("[modbus_rtu] handle_request: callback dropped response\n");
        return MODBUS_DROP;
    }
    else if (res != MODBUS_OK) {
        mutex_unlock(&(modbus->buffer_mutex));
        DEBUG("[modbus_rtu] handle_request: callback failed (%d)\n", res);
        return res;
    }

    return MODBUS_OK;
}

static inline int write_response(modbus_rtu_t *modbus, modbus_message_t *message)
{
    uint8_t size;

    /* ensure that response is sent back to requester */
    if (message->id != modbus->buffer[ID]) {
        return MODBUS_ERR_RESPONSE;
    }

    /* ensure that no responses are sent for broadcasted requests */
    if (message->id == MODBUS_ID_BROADCAST) {
        return MODBUS_ERR_RESPONSE;
    }

    modbus->buffer[ID] = message->id;
    modbus->buffer[FUNC] = message->func;

    /* handle exception responses */
    if (message->exc != MODBUS_EXC_NONE) {
        modbus->buffer[FUNC] |= 0x80;
        modbus->buffer[2] = message->exc;
        modbus->buffer_size = 3;
    }

    /* handle normal responses */
    else {
        switch (message->func) {
        case MODBUS_FC_READ_COILS:
        case MODBUS_FC_READ_DISCRETE_INPUTS:
            /* id + func + size + data */
            size = modbus_bit_count_to_size(message->count);
            write_size(modbus, size, false);
            /* copy data if callback provided a different data buffer */
            if (message->data != modbus->buffer + 3) {
                assert(message->data_size >= size);
                memcpy(modbus->buffer + 3, message->data, size);
            }
            modbus->buffer_size = 3 + size;
            break;

        case MODBUS_FC_READ_HOLDING_REGISTERS:
        case MODBUS_FC_READ_INPUT_REGISTERS:
            /* id + func + size + data */
            size = modbus_reg_count_to_size(message->count);
            write_size(modbus, size, false);
            /* copy data if callback provided a different data buffer */
            if (message->data != modbus->buffer + 3) {
                assert(message->data_size >= size);
                memcpy(modbus->buffer + 3, message->data, size);
            }
            modbus->buffer_size = 3 + size;
            break;

        case MODBUS_FC_WRITE_SINGLE_COIL:
        case MODBUS_FC_WRITE_SINGLE_HOLDING_REGISTER:
            /* id + func + addr + data */
            /* the response equals the request */
            modbus->buffer_size = 6;
            break;

        case MODBUS_FC_WRITE_MULTIPLE_COILS:
        case MODBUS_FC_WRITE_MULTIPLE_HOLDING_REGISTERS:
            /* id + func + addr + count */
            /* the response equals the request, minus the data */
            modbus->buffer_size = 6;
            break;

        default:
            /* should not happen if response is checked before sending */
            modbus->buffer_size = 0;
            break;
        }
    }

    send(modbus);

    mutex_unlock(&(modbus->buffer_mutex));

    return MODBUS_OK;
}

int modbus_rtu_init(modbus_rtu_t *modbus, const modbus_rtu_params_t *params)
{
    int err;

    modbus->params = params;

    /* initialize buffer */
    modbus->buffer_size = 0;
    mutex_init(&(modbus->buffer_mutex));

    /* usec in sec / byte per second * 1.5 */
    modbus->rx_timeout = US_PER_SEC / (params->baudrate / 10) * 1.5;

    /* initialize GPIO */
    if (gpio_is_valid(modbus->params->pin_rts)) {
        err = gpio_init(modbus->params->pin_rts, GPIO_OUT);

        if (err != 0) {
            DEBUG("[modbus_rtu] modbus_rtu_init: GPIO init failed (%d)\n", err);
            return err;
        }

        gpio_write(modbus->params->pin_rts, !modbus->params->pin_rts_enable);
    }

    /* initialize UART */
    err = uart_init(modbus->params->uart, modbus->params->baudrate, uart_cb, modbus);

    if (err != UART_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_init: UART init failed (%d)\n", err);
        return err;
    }

    return 0;
}

int modbus_rtu_send_request(modbus_rtu_t *modbus, modbus_message_t *message)
{
    assert(message != NULL);

    int res;

    /* write request to slave */
    res = write_request(modbus, message);

    if (res != MODBUS_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_send_request: write request failed (%d)\n", res);
        return res;
    }

    /* wait for response from slave */
    res = read_response(modbus, message);

    if (res != MODBUS_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_send_request: read response failed (%d)\n", res);
        return res;
    }

    return MODBUS_OK;
}

int modbus_rtu_recv_request(modbus_rtu_t *modbus, modbus_message_t *message, modbus_request_cb_t cb)
{
    assert(message != NULL);
    assert(cb != NULL);

    int res;

    /* read request from master */
    res = read_request(modbus, message);

    if (res != MODBUS_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_recv_request: read request failed (%d)\n", res);
        return res;
    }

    /* handle request */
    res = handle_request(modbus, message, cb);

    if (res == MODBUS_DROP) {
        DEBUG("[modbus_rtu] modbus_rtu_recv_request: handle request aborted\n");
        return MODBUS_OK;
    }
    else if (res != MODBUS_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_recv_request: handle request failed (%d)\n", res);
        return res;
    }

    /* write response to master */
    res = write_response(modbus, message);

    if (res != MODBUS_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_recv_request: send response failed (%d)\n", res);
        return res;
    }

    return MODBUS_OK;
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_MODBUS_RTU */
