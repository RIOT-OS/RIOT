/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_dynamixel Dynamixel driver
 * @ingroup     drivers_actuators
 * @brief       Drivers for any device using dynamixel's servomotors communication bus
 *
 * The bus is mainly used for servomotors, but a device can be anything : sensors, other actuators.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for Dynamixel devices driver
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <stdlib.h>
#include <stdbool.h>

#include "dynamixel_protocol.h"
#include "uart_half_duplex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t dynamixel_id_t;    /**< device id type */
typedef uint16_t dynamixel_addr_t; /**< register address type */

/**
 * @brief   Descriptor struct for a dynamixel device
 */
typedef struct {
    uart_half_duplex_t *stream; /**< the stream used */
    dynamixel_id_t id;          /**< the device address */
} dynamixel_t;

/**
 * @brief   Possible dynamixel return values
 */
enum {
    DYNAMIXEL_OK,               /**< Success */
    DYNAMIXEL_TIMEOUT,          /**< No response from the device */
    DYNAMIXEL_BUFFER_TOO_SMALL, /**< Buffer is too small for the message */
    DYNAMIXEL_INVALID_MESSAGE,  /**< Invalid message received */
};

/**
 * @brief   Send a PING message to a device
 *
 * @param[in] stream   the stream
 * @param[in] id       the device address
 *
 * @return             DYNAMIXEL_OK if a device answered
 * @return             DYNAMIXEL_TIMEOUT if the device did not answer
 * @return             DYNAMIXEL_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             DYNAMIXEL_INVALID_MESSAGE if an invalid message was received
 */
int dynamixel_ping(uart_half_duplex_t *stream, dynamixel_id_t id);

/**
 * @brief   Initialize a Dynamixel device
 *
 * @param[out] device  the Dynamixel device
 * @param[in] stream   the stream
 * @param[in] id       the device address
 */
void dynamixel_init(dynamixel_t *device, uart_half_duplex_t *stream, dynamixel_id_t id);

/**
 * @brief   Write to a device 8bits register
 *
 * @param[in] device   the Dynamixel device
 * @param[in] reg      the register to write
 * @param[in] value    the value to write
 *
 * @return             DYNAMIXEL_OK on success
 * @return             DYNAMIXEL_TIMEOUT if the device did not answer
 * @return             DYNAMIXEL_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             DYNAMIXEL_INVALID_MESSAGE if an invalid message was received
 */
int dynamixel_write8(const dynamixel_t *device, dynamixel_addr_t reg, uint8_t value);

/**
 * @brief   Write to a device 16bits register
 *
 * @param[in] device   the Dynamixel device
 * @param[in] reg      the register to write
 * @param[in] value    the value to write
 *
 * @return             DYNAMIXEL_OK on success
 * @return             DYNAMIXEL_TIMEOUT if the device did not answer
 * @return             DYNAMIXEL_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             DYNAMIXEL_INVALID_MESSAGE if an invalid message was received
 */
int dynamixel_write16(const dynamixel_t *device, dynamixel_addr_t reg, uint16_t value);

/**
 * @brief   Write to a device address
 *
 * @param[in] device   the Dynamixel device
 * @param[in] reg      the address to start write
 * @param[in] data     the data to write
 * @param[in] length   the data length
 *
 * @return             DYNAMIXEL_OK on success
 * @return             DYNAMIXEL_TIMEOUT if the device did not answer
 * @return             DYNAMIXEL_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             DYNAMIXEL_INVALID_MESSAGE if an invalid message was received
 */
int dynamixel_write(const dynamixel_t *device, dynamixel_addr_t reg, const uint8_t *data, size_t length);

/**
 * @brief   Read from a device 8bits register
 *
 * @param[in] device   the Dynamixel device
 * @param[in] reg      the register to read
 * @param[out] value   the value to read
 *
 * @return             DYNAMIXEL_OK on success
 * @return             DYNAMIXEL_TIMEOUT if the device did not answer
 * @return             DYNAMIXEL_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             DYNAMIXEL_INVALID_MESSAGE if an invalid message was received
 */
int dynamixel_read8(const dynamixel_t *device, dynamixel_addr_t reg, uint8_t *value);

/**
 * @brief   Read from a device 16bits register
 *
 * @param[in] device   the Dynamixel device
 * @param[in] reg      the register to read
 * @param[out] value   the value to read
 *
 * @return             DYNAMIXEL_OK on success
 * @return             DYNAMIXEL_TIMEOUT if the device did not answer
 * @return             DYNAMIXEL_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             DYNAMIXEL_INVALID_MESSAGE if an invalid message was received
 */
int dynamixel_read16(const dynamixel_t *device, dynamixel_addr_t reg, uint16_t *value);

/**
 * @brief   Read from a device address
 *
 * @param[in] device   the Dynamixel device
 * @param[in] reg      the address to start read
 * @param[out] data    the data buffer to fill
 * @param[in] length   the data length
 *
 * @return             DYNAMIXEL_OK on success
 * @return             DYNAMIXEL_TIMEOUT if the device did not answer
 * @return             DYNAMIXEL_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             DYNAMIXEL_INVALID_MESSAGE if an invalid message was received
 */
int dynamixel_read(const dynamixel_t *device, dynamixel_addr_t reg, uint8_t *data, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* DYNAMIXEL_H */
/** @} */
