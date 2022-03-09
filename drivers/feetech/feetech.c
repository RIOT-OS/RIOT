/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_feetech
 * @{
 *
 * @file
 * @brief       Driver implementation for Feetech devices
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "feetech.h"

#include "feetech_protocol.h"
#include "feetech_reader.h"
#include "feetech_writer.h"

#include "periph/uart.h"
#include "byteorder.h"

#include <string.h>

void feetech_init(feetech_t *device, uart_half_duplex_t *stream, feetech_id_t id)
{
    device->stream = stream;
    device->id = id;
}

int feetech_ping(uart_half_duplex_t *stream, feetech_id_t id)
{
    feetech_writer_t pw;

    uart_half_duplex_set_tx(stream);
    feetech_writer_init(&pw, stream->buffer, stream->size);
    feetech_writer_ping_make(&pw, id);
    uart_half_duplex_send(stream, pw.size);

    uart_half_duplex_set_rx(stream);
    if (uart_half_duplex_recv(stream, FEETECH_ACK_SIZE) != FEETECH_ACK_SIZE) {
        return FEETECH_TIMEOUT;
    }

    return FEETECH_OK;
}

int feetech_write(const feetech_t *device, feetech_addr_t reg, const uint8_t *data, size_t length)
{
    uart_half_duplex_set_tx(device->stream);
    if (device->stream->size < length) {
        return FEETECH_BUFFER_TOO_SMALL;
    }

    feetech_writer_t pw;

    feetech_writer_init(&pw, device->stream->buffer, device->stream->size);
    feetech_writer_write_make(&pw, device->id, reg, data, length);
    uart_half_duplex_send(device->stream, pw.size);

    uart_half_duplex_set_rx(device->stream);
    if (uart_half_duplex_recv(device->stream, FEETECH_ACK_SIZE) != FEETECH_ACK_SIZE) {
        return FEETECH_TIMEOUT;
    }

    return FEETECH_OK;
}

int feetech_write8(const feetech_t *device, feetech_addr_t reg, uint8_t value)
{
    return feetech_write(device, reg, &value, 1);
}

int feetech_write16(const feetech_t *device, feetech_addr_t reg, uint16_t value)
{
    value = htons(value);
    return feetech_write(device, reg, (uint8_t*)&value, 2);
}

int feetech_read(const feetech_t *device, feetech_addr_t reg, uint8_t *data, size_t length)
{
    uart_half_duplex_set_tx(device->stream);
    if (device->stream->size < length) {
        return FEETECH_BUFFER_TOO_SMALL;
    }

    feetech_writer_t pw;

    feetech_writer_init(&pw, device->stream->buffer, device->stream->size);
    feetech_writer_read_make(&pw, device->id, reg, length);
    uart_half_duplex_send(device->stream, pw.size);

    uart_half_duplex_set_rx(device->stream);
    const size_t esize = FEETECH_RESPONSE_SIZE(length);
    if (uart_half_duplex_recv(device->stream, esize) != esize) {
        return FEETECH_TIMEOUT;
    }

    feetech_reader_t pr;
    feetech_reader_init(&pr, device->stream->buffer, esize);
    if (!feetech_reader_is_valid(&pr)) {
        return FEETECH_INVALID_MESSAGE;
    }

    if (feetech_reader_response_get_payload_size(&pr) != length) {
        return FEETECH_INVALID_MESSAGE;
    }

    memcpy(data, feetech_reader_response_get_payload(&pr), length);
    return FEETECH_OK;
}

int feetech_read8(const feetech_t *device, feetech_addr_t reg, uint8_t *value)
{
    return feetech_read(device, reg, value, 1);
}

int feetech_read16(const feetech_t *device, feetech_addr_t reg, uint16_t *value)
{
    const int ret = feetech_read(device, reg, (uint8_t*)value, 2);
    if (ret == FEETECH_OK) {
        *value = ntohs(*value);
    }
    return ret;
}
