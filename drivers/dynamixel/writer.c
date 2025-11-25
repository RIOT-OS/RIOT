/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_dynamixel
 * @{
 *
 * @file
 * @brief       Dynamixel messages writer
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "dynamixel_writer.h"
#include "dynamixel_crc.h"

#include <string.h>

#define LOW(v) (v & 0xFF)
#define HIGH(v) ((v >> 8) & 0xFF)

void dynamixel_writer_init(dynamixel_writer_t *writer, uint8_t *buffer, size_t limit)
{
    writer->buffer = buffer;
    writer->size = 0;
    writer->limit = limit;
}

const uint8_t *dynamixel_writer_get_data(const dynamixel_writer_t *writer)
{
    return (const uint8_t*)writer->buffer;
}

size_t dynamixel_writer_get_size(const dynamixel_writer_t *writer)
{
    return writer->size;
}

void dynamixel_writer_ping_make(dynamixel_writer_t *writer, uint8_t id)
{
    const size_t len = 3;
    if (len + 7 <= writer->limit) {
        writer->size = len + 7;

        writer->buffer[0] = DXL_HEADER[0];
        writer->buffer[1] = DXL_HEADER[1];
        writer->buffer[2] = DXL_HEADER[2];
        writer->buffer[3] = 0x00; /* reserved */
        writer->buffer[4] = id;
        writer->buffer[5] = LOW(len);
        writer->buffer[6] = HIGH(len);
        writer->buffer[7] = DXL_INST_PING;

        uint16_t crc = dynamixel_crc_update(0, writer->buffer, 8);

        writer->buffer[8] = LOW(crc);
        writer->buffer[9] = HIGH(crc);
    }
    else {
        writer->size = 0;
    }
}

void dynamixel_writer_write_make(dynamixel_writer_t *writer, uint8_t id, uint16_t reg, const uint8_t *buffer, size_t size)
{
    const size_t len = 5 + size;
    if (len + 7 <= writer->limit) {
        writer->size = len + 7;

        writer->buffer[0] = DXL_HEADER[0];
        writer->buffer[1] = DXL_HEADER[1];
        writer->buffer[2] = DXL_HEADER[2];
        writer->buffer[3] = 0x00; /* reserved */
        writer->buffer[4] = id;
        writer->buffer[5] = LOW(len);
        writer->buffer[6] = HIGH(len);
        writer->buffer[7] = DXL_INST_WRITE;

        writer->buffer[8] = LOW(reg);
        writer->buffer[9] = HIGH(reg);

        memcpy(&writer->buffer[10], buffer, size);

        uint16_t crc = dynamixel_crc_update(0, writer->buffer, len + 5);

        writer->buffer[writer->size - 2] = LOW(crc);
        writer->buffer[writer->size - 1] = HIGH(crc);
    }
    else {
        writer->size = 0;
    }
}

void dynamixel_writer_read_make(dynamixel_writer_t *writer, uint8_t id, uint16_t reg, size_t size)
{
    const size_t len = 7;
    if (len + 7 <= writer->limit) {
        writer->size = len + 7;

        writer->buffer[0] = DXL_HEADER[0];
        writer->buffer[1] = DXL_HEADER[1];
        writer->buffer[2] = DXL_HEADER[2];
        writer->buffer[3] = 0x00; /* reserved */
        writer->buffer[4] = id;
        writer->buffer[5] = LOW(len);
        writer->buffer[6] = HIGH(len);
        writer->buffer[7] = DXL_INST_READ;

        writer->buffer[8] = LOW(reg);
        writer->buffer[9] = HIGH(reg);
        writer->buffer[10] = LOW(size);
        writer->buffer[11] = HIGH(size);

        uint16_t crc = dynamixel_crc_update(0, writer->buffer, 12);

        writer->buffer[12] = LOW(crc);
        writer->buffer[13] = HIGH(crc);
    }
    else {
        writer->size = 0;
    }
}
