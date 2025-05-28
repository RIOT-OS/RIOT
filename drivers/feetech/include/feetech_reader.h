/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_feetech
 *
 * @{
 *
 * @file
 * @brief       Interface definition for Feetech packet reader
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#include "feetech_protocol.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FEETECH_ACK_SIZE           (6)
#define FEETECH_RESPONSE_SIZE(len) (6 + len)

/**
 * @brief Feetech packet reader struct
 */
typedef struct {
    const uint8_t *buffer; /**< data buffer */
    size_t size;           /**< data buffer's size */
} feetech_reader_t;

/**
 * @brief Initialize the Feetech packet reader
 *
 * @param[out] reader    the packet reader
 * @param[in] buffer     the buffer used to store data
 * @param[in] size       the size of the buffer
 */
static inline void feetech_reader_init(feetech_reader_t *reader, const uint8_t *buffer, size_t size)
{
    reader->buffer = buffer;
    reader->size = size;
}

/**
 * @brief Compute the packet's sum
 *
 * @param[in] reader      the packet reader
 *
 * @return the sum of the packet
 */
uint8_t feetech_reader_compute_sum(const feetech_reader_t *reader);

/**
 * @brief Check if the packet has the minimum required size
 *
 * @param[in] reader      the packet reader
 *
 * @return true if the packet has the minimum required size
 * @return false otherwise
 */
static inline bool feetech_reader_check_minsize(const feetech_reader_t *reader)
{
    return 5 < reader->size;
}

/**
 * @brief Check if the packet begins with 2 FEETECH_START bits
 *
 * @param[in] reader      the packet reader
 *
 * @return true if the packet begins with 2 FEETECH_START bits
 * @return false otherwise
 */
static inline bool feetech_reader_check_start(const feetech_reader_t *reader)
{
    return
            reader->buffer[0] == FEETECH_START &&
            reader->buffer[1] == FEETECH_START;
}

/**
 * @brief Check if the packet's size is the same as the buffer's size
 *
 * @param[in] reader      the packet reader
 *
 * @return true if the packet's size is the same as the buffer's size
 * @return false otherwise
 */
static inline bool feetech_reader_check_size(const feetech_reader_t *reader)
{
    return reader->size == (size_t)(reader->buffer[3] + 4);
}

/**
 * @brief Check if the computed sum and the sum of the packet are equal
 *
 * @param[in] reader      the packet reader
 *
 * @return true if the computed sum and the sum of the packet are equal
 * @return false otherwise
 */
static inline bool feetech_reader_check_sum(const feetech_reader_t *reader)
{
    return feetech_reader_compute_sum(reader) == reader->buffer[reader->size - 1];
}

/**
 * @brief Check if the packet is valid
 *
 * @param[in] reader      the packet reader
 *
 * @return true if the packet is valid
 * @return false otherwise
 */
bool feetech_reader_is_valid(const feetech_reader_t *reader);

/**
 * @brief Get the packet's device id
 *
 * @param[in] reader      the packet reader
 *
 * @return the packet's device id
 */
static inline uint8_t feetech_reader_get_id(const feetech_reader_t *reader)
{
    return reader->buffer[2];
}

/**
 * @brief Get the packet's instruction code
 *
 * @param[in] reader      the packet reader
 *
 * @return the packet's instruction code
 */
static inline uint8_t feetech_reader_get_instr(const feetech_reader_t *reader)
{
    return reader->buffer[4];
}

/**
 * @brief Get the packet's payload (response)
 *
 * @param[in] reader      the packet reader
 *
 * @return the address of the beginning of the payload
 */
static inline const uint8_t *feetech_reader_response_get_payload(const feetech_reader_t *reader)
{
    return &reader->buffer[5];
}

/**
 * @brief Get the packet's payload size (response)
 *
 * @param[in] reader      the packet reader
 *
 * @return the size of the payload
 */
static inline size_t feetech_reader_response_get_payload_size(const feetech_reader_t *reader)
{
    return reader->buffer[3] - 2;
}

/**
 * @brief Get the packet's payload (WRITE)
 *
 * @param[in] reader      the packet reader
 *
 * @return the beginning address of the payload
 */
static inline const uint8_t *feetech_reader_write_get_payload(const feetech_reader_t *reader)
{
    return &reader->buffer[6];
}

/**
 * @brief Get the packet's payload size (WRITE)
 *
 * @param[in] reader      the packet reader
 *
 * @return the size of the payload
 */
static inline size_t feetech_reader_write_get_payload_size(const feetech_reader_t *reader)
{
    return reader->buffer[3] - 3;
}

/**
 * @brief Get the packet's target register address (WRITE)
 *
 * @param[in] reader      the packet reader
 *
 * @return the register address
 */
static inline uint8_t feetech_reader_write_get_reg(const feetech_reader_t *reader)
{
    return reader->buffer[5];
}

/**
 * @brief Get the packet's READ size
 *
 * @param[in] reader      the packet reader
 *
 * @return the READ size
 */
static inline size_t feetech_reader_read_get_size(const feetech_reader_t *reader)
{
    return reader->buffer[6];
}

/**
 * @brief Get the packet's target register address (READ)
 *
 * @param[in] reader      the packet reader
 *
 * @return the register address
 */
static inline uint8_t feetech_reader_read_get_reg(const feetech_reader_t *reader)
{
    return reader->buffer[5];
}

/**
 * @brief Get the packet items' payload size (SYNC_WRITE)
 *
 * @param[in] reader      the packet reader
 *
 * @return the size of the items' payload
 */
static inline size_t feetech_reader_sync_write_get_payload_size(const feetech_reader_t *reader)
{
    return reader->buffer[6];
}

/**
 * @brief Get the packet's target register address (SYNC_WRITE)
 *
 * @param[in] reader      the packet reader
 *
 * @return the register address
 */
static inline uint8_t feetech_reader_sync_write_get_reg(const feetech_reader_t *reader)
{
    return reader->buffer[5];
}

/**
 * @brief Get the packet items' count (SYNC_WRITE)
 *
 * @param[in] reader      the packet reader
 *
 * @return the number of items in the packet
 */
size_t feetech_reader_sync_write_get_items_count(const feetech_reader_t *reader);

/**
 * @brief Get the packet item's device id (SYNC_WRITE)
 *
 * @param[in] reader      the packet reader
 * @param[in] index       the item index
 *
 * @return the item's device id
 */
uint8_t feetech_reader_sync_write_item_get_id(const feetech_reader_t *reader, uint8_t index);

/**
 * @brief Get the packet item's payload (SYNC_WRITE)
 *
 * @param[in] reader      the packet reader
 * @param[in] index       the item index
 *
 * @return the beginning address of the payload
 */
const uint8_t *feetech_reader_sync_write_item_get_payload(const feetech_reader_t *reader, uint8_t index);

#ifdef __cplusplus
}
#endif

/** @} */
