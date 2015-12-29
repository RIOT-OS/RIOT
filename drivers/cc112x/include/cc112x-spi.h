/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc112x
 * @{
 *
 * @file
 * @brief       CC112X SPI functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CC112X_SPI_H
#define CC112X_SPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_READS_SPI_PINS		1

/**
 * @brief Write a set of bytes using burst mode (if available)
 *
 * @param dev       Device to work on
 * @param addr      Destination register
 * @param buffer    Data to be written
 * @param count     Size of data
 */
void cc112x_writeburst_reg(cc112x_t *dev, uint16_t addr, const char *buffer, uint8_t count);

/**
 * @brief Read a set of bytes using burst mode (if available)
 *
 * @param dev       Device to work on
 * @param addr      Source register
 * @param buffer    Buffer to store read data
 * @param count     Size of data to be read
 */
void cc112x_readburst_reg(cc112x_t *dev, uint16_t addr, char *buffer, uint8_t count);

/**
 * @brief Write one byte to a register
 *
 * @param dev       Device to work on
 * @param addr      Destinatoin register
 * @param value     New value
 */
void cc112x_write_reg(cc112x_t *dev, uint16_t addr, uint8_t value);

/**
 * @brief Read a byte from register
 *
 * @param dev       Device to work on
 * @param addr  Source register
 *
 * @return Read state and value of register
 */
uint8_t cc112x_read_reg(cc112x_t *dev, uint16_t addr);

/**
 * @brief Read a byte from register, robust version
 *
 * Datasheet states some registered should be read twice until
 * it returns the same value.
 *
 * @param dev       Device to work on
 * @param addr      Source register
 *
 * @return Read state and value of register
 */
//uint8_t cc112x_get_reg_robust(cc112x_t *dev, uint8_t addr);

/**
 * @brief Read state of a register
 *
 * @param dev       Device to work on
 * @param addr      Source register
 *
 * @return State of register
 */
uint8_t cc112x_read_status(cc112x_t *dev, uint16_t addr);

/**
 * @brief Sends a command strobe
 *
 * @param dev       Device to work on
 * @param c         Command code
 *
 * @return Command response
 */
uint8_t cc112x_strobe(cc112x_t *dev, uint8_t c);

/**
 * @brief Pull CS to low and wait for CC112x stabilization
 *
 * @param dev       Device to work on
 */
void cc112x_cs(cc112x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC112X_SPI_H */
