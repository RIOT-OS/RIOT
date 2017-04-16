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
 * @brief       Dynamixel messages reader
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 *
 * @}
 */

#include "dynamixel_reader.h"
#include "dynamixel_crc.h"

static inline bool dynamixel_reader_check_minsize(const dynamixel_reader_t *reader)
{
    return 10 <= reader->size;
}

static inline bool dynamixel_reader_check_start(const dynamixel_reader_t *reader)
{
    return
            reader->buffer[0] == DXL_HEADER[0] &&
            reader->buffer[1] == DXL_HEADER[1] &&
            reader->buffer[2] == DXL_HEADER[2];
}

static inline bool dynamixel_reader_check_size(const dynamixel_reader_t *reader)
{
    return reader->size == (size_t)(dynamixel_reader_get_length(reader) + 7);
}

static inline bool dynamixel_reader_check_sum(const dynamixel_reader_t *reader)
{
    return dynamixel_crc_update(0, reader->buffer, reader->size - 2) == dynamixel_reader_get_crc(reader);
}

bool dynamixel_reader_is_valid(const dynamixel_reader_t *reader)
{
    return
            dynamixel_reader_check_minsize(reader) &&
            dynamixel_reader_check_start(reader)   &&
            dynamixel_reader_check_size(reader)    &&
            dynamixel_reader_check_sum(reader);
}
