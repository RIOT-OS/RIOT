/**
 * @file    cc110x-reg.h
 * @ingroup dev_cc110x_ng
 * @brief   Access to CC110X registers
 *
 * @author  INRIA 
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#ifndef CC110X_REG_H
#define CC110X_REG_H

#include <stdint.h>

/**
 * @brief Write a set of bytes using burst mode (if available)
 *
 * @param addr      Destination register
 * @param buffer    Data to be written
 * @param count     Size of data
 *
 * @return Written bytes
 */
uint8_t cc110x_writeburst_reg(uint8_t addr, char *buffer, uint8_t count);

/**
 * @brief Read a set of bytes using burst mode (if available)
 *
 * @param addr      Source register
 * @param buffer    Buffer to store read data
 * @param count     Size of data to be read
 */
void cc110x_readburst_reg(uint8_t addr, char *buffer, uint8_t count);

/**
 * @brief Read bytes from RXFIFO
 *
 * @param buffer    Buffer to store read data
 * @param count     Size of data to be read
 *
 * @note:   Calls cc110x_readburst_reg if not dedicated fifo read command
 *          available
 */
void cc110x_read_fifo(char *buffer, uint8_t count);

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

#endif
