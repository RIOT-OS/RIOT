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
 * @brief       Feetech messages writer
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "feetech_writer.h"

void feetech_writer_init(feetech_writer_t *writer, uint8_t *buffer, size_t buffer_max_size)
{
    writer->buffer = buffer;
    writer->size = 0;
    writer->buffer_max_size = buffer_max_size;
}

const uint8_t *feetech_writer_get_data(const feetech_writer_t *writer)
{
    return (const uint8_t*)writer->buffer;
}

size_t feetech_writer_get_size(const feetech_writer_t *writer)
{
    return writer->size;
}

void feetech_writer_response_make(feetech_writer_t *writer, uint8_t id, const uint8_t *buffer, size_t size)
{
    const size_t len = 2 + size;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = id;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = 0;

        for (size_t i = 0 ; i < size ; i++) {
            sum += writer->buffer[5 + i] = buffer[i];
        }

        writer->buffer[size - 1] = ~sum;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_ack_make(feetech_writer_t *writer, uint8_t id)
{
    const size_t len = 2;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = id;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = 0;
        writer->buffer[5] = ~sum;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_ping_make(feetech_writer_t *writer, uint8_t id)
{
    const size_t len = 2;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = id;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = INST_PING;
        writer->buffer[5] = ~sum;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_write8_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, uint8_t value)
{
    const size_t len = 4;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = id;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = INST_WRITE;

        sum += writer->buffer[5] = reg;
        sum += writer->buffer[6] = value;

        writer->buffer[7] = ~sum;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_write16_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, uint16_t value)
{
    const size_t len = 5;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = id;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = INST_WRITE;

        sum += writer->buffer[5] = reg;
        sum += writer->buffer[6] = (value >> 8) & 0xFF;
        sum += writer->buffer[7] = value & 0xFF;

        writer->buffer[8] = ~sum;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_write_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, const uint8_t *buffer, size_t size)
{
    const size_t len = 3 + size;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = id;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = INST_WRITE;

        sum += writer->buffer[5] = reg;
        for (size_t i = 0 ; i < size ; i++) {
            sum += writer->buffer[6 + i] = buffer[i];
        }

        writer->buffer[writer->size - 1] = ~sum;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_read_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, size_t size)
{
    const size_t len = 4;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = id;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = INST_READ;

        sum += writer->buffer[5] = reg;
        sum += writer->buffer[6] = (uint8_t)size;

        writer->buffer[7] = ~sum;
    }
    else {
        writer->size = 0;
    }
}

size_t feetech_writer_sync_write_required(feetech_writer_t *writer)
{
    if (8 <= writer->size && writer->buffer[4] == INST_SYNC_WRITE) {
        return writer->buffer[6];
    }
    return 0;
}

void feetech_writer_sync_write_end(feetech_writer_t *writer)
{
    if (writer->size <= 8) {
        writer->size = 0;
    }
}

void feetech_writer_sync_write_add(feetech_writer_t *writer, uint8_t id, const uint8_t *buffer, size_t size)
{
    if (feetech_writer_sync_write_required(writer) == size &&
        size != 0 && writer->size + size + 1 <= writer->buffer_max_size) {

        uint8_t sum = ~writer->buffer[writer->size - 1];

        sum += writer->buffer[writer->size - 1] = id;
        for (size_t i = 0 ; i < size ; i++) {
            sum += writer->buffer[writer->size + i] = buffer[i];
        }

        writer->buffer[3] += size + 1;
        writer->buffer[writer->size + size] = ~(sum + size + 1);
        writer->size += size + 1;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_sync_write_add_8bits(feetech_writer_t *writer, uint8_t id, uint8_t value)
{
    if (feetech_writer_sync_write_required(writer) == 1 &&
        writer->size + 2 <= writer->buffer_max_size) {

        uint8_t sum = ~writer->buffer[writer->size - 1];

        sum += writer->buffer[writer->size - 1] = id;
        sum += writer->buffer[writer->size] = value;

        writer->buffer[3] += 2;
        writer->buffer[writer->size + 1] = ~(sum + 2);
        writer->size += 2;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_sync_write_add_16bits(feetech_writer_t *writer, uint8_t id, uint16_t value)
{
    if (feetech_writer_sync_write_required(writer) == 2 &&
        writer->size + 3 <= writer->buffer_max_size) {

        uint8_t sum = ~writer->buffer[writer->size - 1];

        sum += writer->buffer[writer->size - 1] = id;
        sum += writer->buffer[writer->size + 0] = (value >> 8) & 0xFF;
        sum += writer->buffer[writer->size + 1] = value & 0xFF;

        writer->buffer[3] += 3;
        writer->buffer[writer->size + 2] = ~(sum + 3);
        writer->size += 3;
    }
    else {
        writer->size = 0;
    }
}

void feetech_writer_sync_write_begin(feetech_writer_t *writer, uint8_t reg, size_t size)
{
    const size_t len = 4;
    if (len + 4 <= writer->buffer_max_size) {
        writer->size = len + 4;

        uint8_t sum = 0;

        writer->buffer[0] = FEETECH_START;
        writer->buffer[1] = FEETECH_START;
        sum += writer->buffer[2] = 0xFF;
        sum += writer->buffer[3] = len;
        sum += writer->buffer[4] = INST_SYNC_WRITE;

        sum += writer->buffer[5] = reg;
        sum += writer->buffer[6] = (uint8_t)size;

        writer->buffer[7] = ~sum;
    }
}
