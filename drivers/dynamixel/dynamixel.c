/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_dynamixel
 * @{
 *
 * @file
 * @brief       Driver implementation for Dynamixel devices
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "dynamixel.h"

#include "dynamixel_protocol.h"
#include "dynamixel_reader.h"
#include "dynamixel_writer.h"

#include <string.h>

void dynamixel_init(dynamixel_t *device, uart_half_duplex_t *stream, dynamixel_id_t id)
{
    device->stream = stream;
    device->id = id;
}

int dynamixel_ping(uart_half_duplex_t *stream, dynamixel_id_t id)
{
    dynamixel_writer_t pw;

    uart_half_duplex_set_tx(stream);
    dynamixel_writer_init(&pw, stream->buffer, stream->size);
    dynamixel_writer_ping_make(&pw, id);
    uart_half_duplex_send(stream, pw.size);

    uart_half_duplex_set_rx(stream);
    if (uart_half_duplex_recv(stream, DXL_STATUS_SIZE(3)) != DXL_STATUS_SIZE(3)) {
        return DYNAMIXEL_TIMEOUT;
    }

    return DYNAMIXEL_OK;
}

int dynamixel_write(const dynamixel_t *device, dynamixel_addr_t reg, const uint8_t *data, size_t length)
{
    uart_half_duplex_set_tx(device->stream);
    if (device->stream->size < length) {
        return DYNAMIXEL_BUFFER_TOO_SMALL;
    }

    dynamixel_writer_t pw;

    dynamixel_writer_init(&pw, device->stream->buffer, device->stream->size);
    dynamixel_writer_write_make(&pw, device->id, reg, data, length);
    uart_half_duplex_send(device->stream, pw.size);

    uart_half_duplex_set_rx(device->stream);
    if (uart_half_duplex_recv(device->stream, DXL_STATUS_SIZE(0)) != DXL_STATUS_SIZE(0)) {
        return DYNAMIXEL_TIMEOUT;
    }

    return DYNAMIXEL_OK;
}

int dynamixel_write8(const dynamixel_t *device, dynamixel_addr_t reg, uint8_t value)
{
    return dynamixel_write(device, reg, &value, 1);
}

int dynamixel_write16(const dynamixel_t *device, dynamixel_addr_t reg, uint16_t value)
{
    return dynamixel_write(device, reg, (uint8_t*)&value, 2);
}

int dynamixel_read(const dynamixel_t *device, dynamixel_addr_t reg, uint8_t *data, size_t length)
{
    uart_half_duplex_set_tx(device->stream);
    if (device->stream->size < length) {
        return DYNAMIXEL_BUFFER_TOO_SMALL;
    }

    dynamixel_writer_t pw;

    dynamixel_writer_init(&pw, device->stream->buffer, device->stream->size);
    dynamixel_writer_read_make(&pw, device->id, reg, length);
    uart_half_duplex_send(device->stream, pw.size);

    uart_half_duplex_set_rx(device->stream);
    const size_t esize = DXL_STATUS_SIZE(length);
    if (uart_half_duplex_recv(device->stream, esize) != esize) {
        return DYNAMIXEL_TIMEOUT;
    }

    dynamixel_reader_t pr;
    dynamixel_reader_init(&pr, device->stream->buffer, esize);
    if (!dynamixel_reader_is_valid(&pr)) {
        return DYNAMIXEL_INVALID_MESSAGE;
    }

    if (dynamixel_reader_status_get_payload_size(&pr) != length) {
        return DYNAMIXEL_INVALID_MESSAGE;
    }

    memcpy(data, dynamixel_reader_status_get_payload(&pr), length);
    return DYNAMIXEL_OK;
}

int dynamixel_read8(const dynamixel_t *device, dynamixel_addr_t reg, uint8_t *value)
{
    return dynamixel_read(device, reg, value, 1);
}

int dynamixel_read16(const dynamixel_t *device, dynamixel_addr_t reg, uint16_t *value)
{
    return dynamixel_read(device, reg, (uint8_t*)value, 2);
}
