/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sx1276 SX1276
 * @brief       Semtech SX1276
 * @{
 * @file
 * @brief       Public interface for SX1276 driver
 * @author      Eugene P. [ep@unwds.com]
 */

#ifndef SX1276_INTERNAL_H_
#define SX1276_INTERNAL_H_

#include <inttypes.h>
#include "sx1276.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief generates 32 bits random value based on the RSSI readings
 * This function sets the radio in LoRa mode and disables all interrupts from it.
 * After calling this function either sx1276_set_rx_config or sx1276_set_tx_config
 * functions must be called.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @return random 32 bits value
 */
uint32_t sx1276_random(sx1276_t *dev);

/**
 * @brief Writes the radio register at specified address.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr Register address
 *
 * @param	[IN]	data New register value
 */
void sx1276_reg_write(sx1276_t *dev, uint8_t addr, uint8_t data);

/**
 * @brief Reads the radio register at specified address.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr	Register address
 *
 * @return	Register value
 */
uint8_t sx1276_reg_read(sx1276_t *dev, uint8_t addr);

/**
 * @brief Writes multiple radio registers starting at address (burst-mode).
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr	First radio register address
 *
 * @param	[IN]	buffer	buffer containing the new register's values
 *
 * @param	[IN]	size	Number of registers to be written
 */
void sx1276_reg_write_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                            uint8_t size);

/**
 * @brief Reads multiple radio registers starting at address.
 *
 * @param	[IN]	dev		The sx1276 device structure pointer
 *
 * @param	[IN]	addr	First radio register address
 *
 * @param	[OUT]	bufer	Buffer where to copy registers data
 *
 * @param	[IN]	size	Number of registers to be read
 */
void sx1276_reg_read_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                           uint8_t size);

/**
 * @brief Writes the buffer contents to the SX1276 FIFO
 *
 * @param [IN] buffer Buffer containing data to be put on the FIFO.
 * @param [IN] size Number of bytes to be written to the FIFO
 */
void sx1276_write_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size);

/**
 * @brief Reads the contents of the SX1276 FIFO
 *
 * @param [OUT] buffer Buffer where to copy the FIFO read data.
 * @param [IN] size Number of bytes to be read from the FIFO
 */
void sx1276_read_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size);

#ifdef __cplusplus
}  /* extern "C" */
#endif
#endif
