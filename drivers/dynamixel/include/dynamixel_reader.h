/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     drivers_dynamixel
 *
 * @{
 *
 * @file
 * @brief       Interface definition for Dynamixel packet reader
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */


#include <stdlib.h>
#include <stdbool.h>

#include "dynamixel_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DXL_PING_SIZE        (10)
#define DXL_STATUS_SIZE(len) (11+len)
#define DXL_READ_SIZE        (14)
#define DXL_WRITE_SIZE(len)  (12+len)

/**
 * @brief Dynamixel packet reader struct
 */
typedef struct {
    const uint8_t *buffer; /**< data buffer */
    size_t size;           /**< data buffer's size */
} dynamixel_reader_t;

/**
 * @brief Initialize the Dynamixel packet reader
 *
 * @param[out] reader    the packet reader
 * @param[in] buffer     the buffer used to store data
 * @param[in] size       the size of the buffer
 */
static inline void dynamixel_reader_init(dynamixel_reader_t *reader, const uint8_t *buffer, size_t size)
{
    reader->buffer = buffer;
    reader->size = size;
}

/**
 * @brief Check if the packet is valid
 *
 * @param[in] reader      the packet reader
 *
 * @return true if the packet is valid
 * @return false otherwise
 */
bool dynamixel_reader_is_valid(const dynamixel_reader_t *reader);

/**
 * @brief Get the packet's device id
 *
 * @param[in] reader      the packet reader
 *
 * @return the packet's device id
 */
static inline uint8_t dynamixel_reader_get_id(const dynamixel_reader_t *reader)
{
    return reader->buffer[4];
}

/**
 * @brief Get the packet's instruction code
 *
 * @param[in] reader      the packet reader
 *
 * @return the packet's instruction code
 */
static inline uint8_t dynamixel_reader_get_instr(const dynamixel_reader_t *reader)
{
    return reader->buffer[7];
}

/**
 * @brief Get the packet's length field
 *
 * @param[in] reader      the packet reader
 *
 * @return the packet's length field
 */
static inline uint16_t dynamixel_reader_get_length(const dynamixel_reader_t *reader)
{
    return
            (((uint16_t)reader->buffer[5]) & 0xFF) |
            ((((uint16_t)reader->buffer[6]) & 0xFF) << 8);
}

/**
 * @brief Get the packet's crc
 *
 * @param[in] reader      the packet reader
 *
 * @return the packet's length field
 */
static inline uint16_t dynamixel_reader_get_crc(const dynamixel_reader_t *reader)
{
    return
            (((uint16_t)reader->buffer[reader->size - 2]) & 0xFF) |
            ((((uint16_t)reader->buffer[reader->size - 1]) & 0xFF) << 8);
}

/**
 * @brief Get the packet's payload (response)
 *
 * @param[in] reader      the packet reader
 *
 * @return the address of the beginning of the payload
 */
static inline const uint8_t *dynamixel_reader_status_get_payload(const dynamixel_reader_t *reader)
{
    return &reader->buffer[9];
}

/**
 * @brief Get the packet's payload size (response)
 *
 * @param[in] reader      the packet reader
 *
 * @return the size of the payload
 */
static inline size_t dynamixel_reader_status_get_payload_size(const dynamixel_reader_t *reader)
{
    return dynamixel_reader_get_length(reader) - 4;
}

#ifdef __cplusplus
}
#endif

/** @} */
