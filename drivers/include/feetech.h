/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_feetech Feetech driver
 * @ingroup     drivers_actuators
 * @brief       Drivers for any device using feetech's servomotors communication bus.
 *
 * The bus is mainly used for servomotors, but a device can be anything : sensors, other actuators.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for Feetech devices driver
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#ifndef FEETECH_H
#define FEETECH_H

#include <stdlib.h>

#include "feetech_protocol.h"
#include "uart_half_duplex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t feetech_id_t;    /**< device id type */
typedef uint8_t feetech_addr_t;  /**< address type */

/**
 * @brief   Descriptor struct for a feetech device
 */
typedef struct {
    uart_half_duplex_t *stream; /**< the stream used */
    feetech_id_t id;            /**< the device address */
} feetech_t;

/**
 * @brief   Possible feetech return values
 */
enum {
    FEETECH_OK,               /**< Success */
    FEETECH_TIMEOUT,          /**< No response from the device */
    FEETECH_BUFFER_TOO_SMALL, /**< Buffer is too small for the message */
    FEETECH_INVALID_MESSAGE,  /**< Invalid message received */
};

/**
 * @brief   Send a PING message to a device
 *
 * @param[in] stream   the stream
 * @param[in] id       the device address
 *
 * @return             FEETECH_OK if a device answered
 * @return             FEETECH_TIMEOUT if the device did not answer
 * @return             FEETECH_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             FEETECH_INVALID_MESSAGE if an invalid message was received
 */
int feetech_ping(uart_half_duplex_t *stream, feetech_id_t id);

/**
 * @brief   Initialize a Feetech device
 *
 * @param[out] device  the Feetech device
 * @param[in] stream   the stream
 * @param[in] id       the device address
 */
void feetech_init(feetech_t *device, uart_half_duplex_t *stream, feetech_id_t id);

/**
 * @brief   Write to a device 8bits address
 *
 * @param[in] device   the Feetech device
 * @param[in] addr     the address to write
 * @param[in] value    the value to write
 *
 * @return             FEETECH_OK on success
 * @return             FEETECH_TIMEOUT if the device did not answer
 * @return             FEETECH_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             FEETECH_INVALID_MESSAGE if an invalid message was received
 */
int feetech_write8(const feetech_t *device, feetech_addr_t addr, uint8_t value);

/**
 * @brief   Write to a device 16bits address
 *
 * @param[in] device   the Feetech device
 * @param[in] addr     the address to write
 * @param[in] value    the value to write
 *
 * @return             FEETECH_OK on success
 * @return             FEETECH_TIMEOUT if the device did not answer
 * @return             FEETECH_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             FEETECH_INVALID_MESSAGE if an invalid message was received
 */
int feetech_write16(const feetech_t *device, feetech_addr_t addr, uint16_t value);

/**
 * @brief   Write to a device address
 *
 * @param[in] device   the Feetech device
 * @param[in] addr     the address to start write
 * @param[in] data     the data to write
 * @param[in] length   the data length
 *
 * @return             FEETECH_OK on success
 * @return             FEETECH_TIMEOUT if the device did not answer
 * @return             FEETECH_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             FEETECH_INVALID_MESSAGE if an invalid message was received
 */
int feetech_write(const feetech_t *device, feetech_addr_t addr, const uint8_t *data, size_t length);

/**
 * @brief   Read from a device 8bits address
 *
 * @param[in] device   the Feetech device
 * @param[in] addr     the address to read
 * @param[out] value   the value to read
 *
 * @return             FEETECH_OK on success
 * @return             FEETECH_TIMEOUT if the device did not answer
 * @return             FEETECH_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             FEETECH_INVALID_MESSAGE if an invalid message was received
 */
int feetech_read8(const feetech_t *device, feetech_addr_t addr, uint8_t *value);

/**
 * @brief   Read from a device 16bits address
 *
 * @param[in] device   the Feetech device
 * @param[in] addr     the address to read
 * @param[out] value   the value to read
 *
 * @return             FEETECH_OK on success
 * @return             FEETECH_TIMEOUT if the device did not answer
 * @return             FEETECH_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             FEETECH_INVALID_MESSAGE if an invalid message was received
 */
int feetech_read16(const feetech_t *device, feetech_addr_t addr, uint16_t *value);

/**
 * @brief   Read from a device address
 *
 * @param[in] device   the Feetech device
 * @param[in] addr      the address to start read
 * @param[out] data    the data buffer to fill
 * @param[in] length   the data length
 *
 * @return             FEETECH_OK on success
 * @return             FEETECH_TIMEOUT if the device did not answer
 * @return             FEETECH_BUFFER_TOO_SMALL if buffer is too small for the message
 * @return             FEETECH_INVALID_MESSAGE if an invalid message was received
 */
int feetech_read(const feetech_t *device, feetech_addr_t addr, uint8_t *data, size_t length);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FEETECH_H */
