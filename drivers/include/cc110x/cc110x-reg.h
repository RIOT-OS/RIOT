/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
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
 * @brief       Access to CC110X registers
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author		Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef CC110X_REG_H
#define CC110X_REG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write a set of bytes using burst mode (if available)
 *
 * @param addr      Destination register
 * @param buffer    Data to be written
 * @param count     Size of data
 */
void cc110x_writeburst_reg(uint8_t addr, char *buffer, uint8_t count);

/**
 * @brief Read a set of bytes using burst mode (if available)
 *
 * @param addr      Source register
 * @param buffer    Buffer to store read data
 * @param count     Size of data to be read
 */
void cc110x_readburst_reg(uint8_t addr, char *buffer, uint8_t count);

/**
 * @brief Write one byte to a register
 *
 * @param addr  Destinatoin register
 * @param value New value
 */
void cc110x_write_reg(uint8_t addr, uint8_t value);

/**
 * @brief Read a byte from register
 *
 * @param addr  Source register
 *
 * @return Read state and value of register
 */
uint8_t cc110x_read_reg(uint8_t addr);

/**
 * @brief Read state of a register
 *
 * @param addr  Source register
 *
 * @return State of register
 */
uint8_t cc110x_read_status(uint8_t addr);

/**
 * @brief Sends a command strobe
 *
 * @param c     Command code
 *
 * @return Command response
 */
uint8_t cc110x_strobe(uint8_t c);

/**
 * @brief Pull CS to low and wait for CC110x stabilization
 */
void cc110x_cs(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC110X_REG_H */
