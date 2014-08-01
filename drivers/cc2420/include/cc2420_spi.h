/**
  * cc2420_spi.h - Definition of CC2420 SPI functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */


/**
  * @ingroup CC2420
  * @{
  * @file
  * @brief Definition of CC2420 SPI functions
  * @author Milan Babel <babel@inf.fu-berlin.de>
  *
  */
#ifndef CC2420_SPI_H
#define CC2420_SPI_H

#include <stdio.h>
#include "board.h"

/**
 * @brief Writes a byte to the cc2420 register.
 *
 * @param[in] addr The address of the register to write.
 * @param[in] value The value to write in the register.
 */
void cc2420_write_reg(uint8_t addr, uint16_t value);

/**
 * @brief Reads a byte from the cc2420 register.
 *
 * @param[in] addr The address of the register to read.
 *
 * @return The value in the register.
 */
uint16_t cc2420_read_reg(uint8_t addr);

/**
 * @brief Sends a strobe command to the cc2420.
 *
 * @param[in] c The strobe command to send.
 *
 * @return The result of the strobe command.
 */
uint8_t cc2420_strobe(uint8_t c);


/**
 * @brief Reads multiple bytes from the cc2420 ram.
 *
 * @param[in] addr The ram address to read.
 * @param[out] buffer A buffer to store the value of the ram.
 * @param[in] len The count of bytes which should be read.
 *
 * @return The number of bytes read.
 */
radio_packet_length_t cc2420_read_ram(uint16_t addr, uint8_t* buffer, radio_packet_length_t len);

/**
 * @brief Writes multiple bytes to the cc2420 ram.
 *
 * @param[in] addr The ram address to write.
 * @param[in] buffer A buffer with the value to write to the ram.
 * @param[in] len The count of bytes which should be written.
 *
 * @return The number of bytes written.
 */
radio_packet_length_t cc2420_write_ram(uint16_t addr, uint8_t* buffer, radio_packet_length_t len);

/**
 * @brief Writes multiple bytes to the cc2420 fifo.
 *
 * @param[in] data A buffer with the value to write to the fifo.
 * @param[in] data_length The count of bytes which should be written.
 *
 * @return The number of bytes written.
 */
radio_packet_length_t cc2420_write_fifo(uint8_t* data, radio_packet_length_t data_length);

/**
 * @brief Reads multiple bytes from the cc2420 fifo.
 *
 * @param[out] data A buffer to store the value of the fifo.
 * @param[in] data_length The count of bytes which should be read.
 *
 * @return The number of bytes read.
 */
radio_packet_length_t cc2420_read_fifo(uint8_t* data, radio_packet_length_t data_length);

#endif
