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
 * @brief       Feetech messages reader
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "feetech_reader.h"

static uint8_t _compute_sum(const feetech_reader_t *reader)
{
    uint8_t sum = 0;
    for (size_t i = 2 ; i < reader->size-1 ; i++) {
        sum += reader->buffer[i];
    }
    return sum;
}

uint8_t feetech_reader_compute_sum(const feetech_reader_t *reader)
{
    return ~_compute_sum(reader);
}

bool feetech_reader_is_valid(const feetech_reader_t *reader)
{
    return
            feetech_reader_check_minsize(reader) &&
            feetech_reader_check_start(reader)   &&
            feetech_reader_check_size(reader)    &&
            feetech_reader_check_sum(reader);
}

size_t feetech_reader_sync_write_get_items_count(const feetech_reader_t *reader)
{
    return (reader->buffer[3] - 4) / (reader->buffer[6] + 1);
}

uint8_t feetech_reader_sync_write_item_get_id(const feetech_reader_t *reader, uint8_t index)
{
    return reader->buffer[7 + index * (feetech_reader_sync_write_get_payload_size(reader) + 1)];
}

const uint8_t *feetech_reader_sync_write_item_get_payload(const feetech_reader_t *reader, uint8_t index)
{
    return &reader->buffer[7 + index * (feetech_reader_sync_write_get_payload_size(reader) + 1) + 1];
}
