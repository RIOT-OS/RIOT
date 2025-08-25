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
 * @brief       Interface definition for Feetech packet writer
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

/**
 * @brief Feetech packet writer struct
 */
typedef struct {
    uint8_t *buffer; /**< data buffer */
    size_t size;     /**< packet's size */
    size_t buffer_max_size;    /**< data buffer's size */
} feetech_writer_t;

/**
 * @brief Initialize the Feetech packet writer
 *
 * @param[out] writer    the packet writer
 * @param[in] buffer     the buffer used to store data
 * @param[in] buffer_max_size      the size of the buffer (= maximum packet size)
 */
void feetech_writer_init(feetech_writer_t *writer, uint8_t *buffer, size_t buffer_max_size);

/**
 * @brief Get the data buffer to send
 *
 * @param[out] writer    the packet writer
 *
 * @return the beginning address of the buffer
 */
const uint8_t *feetech_writer_get_data(const feetech_writer_t *writer);

/**
 * @brief Get the data buffer's size to send
 *
 * @param[out] writer    the packet writer
 *
 * @return the buffer's size
 */
size_t feetech_writer_get_size(const feetech_writer_t *writer);

/**
 * @brief Build a response packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the responder's id
 * @param[in] buffer     the response data
 * @param[in] size       the response size
 */
void feetech_writer_response_make(feetech_writer_t *writer, uint8_t id, const uint8_t *buffer, size_t size);

/**
 * @brief Build an ack packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the responder's id
 */
void feetech_writer_ack_make(feetech_writer_t *writer, uint8_t id);

/**
 * @brief Build a PING packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 */
void feetech_writer_ping_make(feetech_writer_t *writer, uint8_t id);

/**
 * @brief Build a WRITE packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] reg        the register to write in
 * @param[in] buffer     the data buffer to write
 * @param[in] size       the data buffer's size
 */
void feetech_writer_write_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, const uint8_t *buffer, size_t size);

/**
 * @brief Build a WRITE packet (8 bits)
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] reg        the register to write in
 * @param[in] value      the value to write in the register
 */
void feetech_writer_write8_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, uint8_t value);

/**
 * @brief Build a WRITE packet (16 bits)
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] reg        the register to write in
 * @param[in] value      the value to write in the register
 */
void feetech_writer_write16_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, uint16_t value);

/**
 * @brief Build a READ packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] reg        the register to read
 * @param[in] size       the size to read
 */
void feetech_writer_read_make(feetech_writer_t *writer, uint8_t id, uint8_t reg, size_t size);

/**
 * @brief Begin to build a SYNC_WRITE packet
 *
 * @param[out] writer    the packet writer
 * @param[in] reg        the register to write in
 * @param[in] size       the data buffer's size
 */
void feetech_writer_sync_write_begin(feetech_writer_t *writer, uint8_t reg, size_t size);

/**
 * @brief End the building of a SYNC_WRITE packet
 *
 * @param[out] writer    the packet writer
 */
void feetech_writer_sync_write_end(feetech_writer_t *writer);

/**
 * @brief Add an item to a SYNC_WRITE packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] buffer     the data buffer to write
 * @param[in] size       the data buffer's size
 */
void feetech_writer_sync_write_add(feetech_writer_t *writer, uint8_t id, const uint8_t *buffer, size_t size);

/**
 * @brief Add an item to a SYNC_WRITE packet (8 bits)
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] value      the value to write
 */
void feetech_writer_sync_write_add_8bits(feetech_writer_t *writer, uint8_t id, uint8_t value);

/**
 * @brief Add an item to a SYNC_WRITE packet (16 bits)
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] value      the value to write
 */
void feetech_writer_sync_write_add_16bits(feetech_writer_t *writer, uint8_t id, uint16_t value);

#ifdef __cplusplus
}
#endif

/** @} */
