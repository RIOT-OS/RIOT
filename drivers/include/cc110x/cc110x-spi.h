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
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       CC110X SPI functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CC110X_SPI_H
#define CC110X_SPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write a set of bytes using burst mode (if available)
 *
 * @param dev       Device to work on
 * @param addr      Destination register
 * @param buffer    Data to be written
 * @param count     Size of data
 */
void cc110x_writeburst_reg(cc110x_t *dev, uint8_t addr, const char *buffer, uint8_t count);

/**
 * @brief Read a set of bytes using burst mode (if available)
 *
 * @param dev       Device to work on
 * @param addr      Source register
 * @param buffer    Buffer to store read data
 * @param count     Size of data to be read
 */
void cc110x_readburst_reg(cc110x_t *dev, uint8_t addr, char *buffer, uint8_t count);

/**
 * @brief Write one byte to a register
 *
 * @param dev       Device to work on
 * @param addr      Destinatoin register
 * @param value     New value
 */
void cc110x_write_reg(cc110x_t *dev, uint8_t addr, uint8_t value);

/**
 * @brief Read a byte from register
 *
 * @param dev       Device to work on
 * @param addr  Source register
 *
 * @return Read state and value of register
 */
uint8_t cc110x_read_reg(cc110x_t *dev, uint8_t addr);

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
uint8_t cc110x_get_reg_robust(cc110x_t *dev, uint8_t addr);

/**
 * @brief Read state of a register
 *
 * @param dev       Device to work on
 * @param addr      Source register
 *
 * @return State of register
 */
uint8_t cc110x_read_status(cc110x_t *dev, uint8_t addr);

/**
 * @brief Sends a command strobe
 *
 * @param dev       Device to work on
 * @param c         Command code
 *
 * @return Command response
 */
uint8_t cc110x_strobe(cc110x_t *dev, uint8_t c);

/**
 * @brief Pull CS to low and wait for CC110x stabilization
 *
 * @param dev       Device to work on
 */
void cc110x_cs(cc110x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC110X_SPI_H */
