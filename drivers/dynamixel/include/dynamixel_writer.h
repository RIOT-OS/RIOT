/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_dynamixel
 *
 * @{
 *
 * @file
 * @brief       Interface definition for Dynamixel packet writer
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef DYNAMIXEL_WRITER_H
#define DYNAMIXEL_WRITER_H

#include "dynamixel_protocol.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dynamixel packet writer struct
 */
typedef struct {
    uint8_t *buffer; /**< data buffer */
    size_t size;     /**< packet's size */
    size_t limit;    /**< data buffer's size */
} dynamixel_writer_t;

/**
 * @brief Initialize the Dynamixel packet writer
 *
 * @param[out] writer    the packet writer
 * @param[in] buffer     the buffer used to store data
 * @param[in] limit      the size of the buffer (= maximum packet size)
 */
void dynamixel_writer_init(dynamixel_writer_t *writer, uint8_t *buffer, size_t limit);

/**
 * @brief Get the data buffer to send
 *
 * @param[out] writer    the packet writer
 *
 * @return the beginning address of the buffer
 */
const uint8_t *dynamixel_writer_get_data(const dynamixel_writer_t *writer);

/**
 * @brief Get the data buffer's size to send
 *
 * @param[out] writer    the packet writer
 *
 * @return the buffer's size
 */
size_t dynamixel_writer_get_size(const dynamixel_writer_t *writer);

/**
 * @brief Build a PING packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 */
void dynamixel_writer_ping_make(dynamixel_writer_t *writer, uint8_t id);

/**
 * @brief Build a WRITE packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] reg        the register to write in
 * @param[in] buffer     the data buffer to write
 * @param[in] size       the data buffer's size
 */
void dynamixel_writer_write_make(dynamixel_writer_t *writer, uint8_t id, uint16_t reg, const uint8_t *buffer, size_t size);

/**
 * @brief Build a READ packet
 *
 * @param[out] writer    the packet writer
 * @param[in] id         the destination's id
 * @param[in] reg        the register to read
 * @param[in] size       the size to read
 */
void dynamixel_writer_read_make(dynamixel_writer_t *writer, uint8_t id, uint16_t reg, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DYNAMIXEL_WRITER_H */
