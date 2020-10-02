/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_arduino_api
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * @brief       Definition of the Arduino 'Wire Library' for TwoWire interfaces
 *
 * This library is the implementation of the [Arduino Wire Library]
 * (https://www.arduino.cc/en/Reference/Wire) for the I2C peripheral
 * interfaces in RIOT. It supports only I2C master mode and the functions
 * that are documented in the official [Arduino Reference]
 * (https://www.arduino.cc/en/Reference/Wire) of this library.
 *
 * The implementation is an adaptation of the original Arduino Wire Library
 * which is published under the following copyright:
 *
 * ```
 * TwoWire.h - TWI/I2C library for Arduino & Wiring
 * Copyright (c) 2006 Nicholas Zambetti. All right reserved.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
 * ```
 *
 * The documentation in this file is partially extracted from the original
 * [Arduino Reference](https://www.arduino.cc/en/Reference/Wire) of this
 * library which is published under the
 * [Creative Commons Attribution-ShareAlike 3.0 License]
 * (https://creativecommons.org/licenses/by-sa/3.0/).
 */

#ifndef WIREPORT_HPP
#define WIREPORT_HPP

#include <inttypes.h>
#include <stddef.h>

/** Default Arduino I2C interface */
#ifndef ARDUINO_I2C_DEV
#define ARDUINO_I2C_DEV    (I2C_DEV(0))
#endif

/** Buffer length used by the Arduino Wire library implementation */
#define WIREPORT_BUFFER_LENGTH 32

/** Class definition for the Arduino Wire library implementation */
class TwoWire
{
  private:

    static uint8_t rxBuffer[];      /**< RX buffer */
    static uint8_t rxBufferIndex;   /**< index for RX buffer read */
    static uint8_t rxBufferLength;  /**< number of bytes in RX buffer */

    static uint8_t txAddress;       /**< address for transfer */
    static uint8_t txBuffer[];      /**< TX buffer */
    static uint8_t txBufferIndex;   /**< index for TX buffer write */
    static uint8_t txBufferLength;  /**< number of bytes in TX buffer */
    static uint8_t txError;         /**< error code in write operations */

    static uint8_t transmitting;    /**< set by #beginTransmission and reset
                                         by #endTransmission to indicate an
                                         ongoing transmission */
  public:

    /**
     * @brief Constructor
     */
    TwoWire(void);

    /**
     * @brief Initializes the I2C device defined by #ARDUINO_I2C_DEV as master
     */
    void begin(void);

    /**
     * @brief Initializes the I2C device defined by #ARDUINO_I2C_DEV as slave
     *
     * @note Since slave mode is not yet supported by the RIOT port of the
     * Arduino Wire library, calling this method leads to a core panic.
     *
     * @param[in]   addr    Address of the device initialized as slave
     */
    void begin(uint8_t addr);

    /**
     * @brief   Set the clock speed of the I2C device defined by
     *          #ARDUINO_I2C_DEV.
     *
     * @note In RIOT, the I2C bus clock speed is determined by the board
     * definition. This method does therefore nothing. It is just realized
     * for compatibility reasons.
     *
     * @param[in]   clk     I2C clock speed in Hz
     */
    void setClock(uint32_t clk);

    /**
     * @brief   Begin a transmission to a I2C slave device
     *
     * This method begins a transmission to the I2C slave device with the
     * given address. Subsequently, queue bytes for transmission with the
     * #write method and transmit them by calling #endTransmission.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireBeginTransmission
     *
     * @param[in]   addr    Address of the slave device
     */
    void beginTransmission(uint8_t addr);

    /**
     * @brief   End a transmission to a I2C slave device
     *
     * Ends a transmission to a slave device that was begun by
     * #beginTransmission() and transmits the bytes that were queued by #write.
     * Sends always a STOP condition after the request.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireEndTransmission
     *
     * @retval  0   success
     * @retval  1   data too long to fit in transmit buffer
     * @retval  2   received NACK on transmit of address
     * @retval  3   received NACK on transmit of data
     * @retval  4   other error
     */
    uint8_t endTransmission(void);

    /**
     * @brief   End a transmission to a I2C slave device
     *
     * Ends a transmission to a slave device that was begun by
     * #beginTransmission() and transmits the bytes that were queued by #write.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireEndTransmission
     *
     * @param   stop    Send STOP condition after transmission if true or
     *                  nothing if false.
     *
     * @retval  0   success
     * @retval  1   data too long to fit in transmit buffer
     * @retval  2   received NACK on transmit of address
     * @retval  3   received NACK on transmit of data
     * @retval  4   other error
     */
    uint8_t endTransmission(uint8_t stop);

    /**
     * @brief   Request bytes from a I2C slave device
     *
     * Used by the master to request bytes from a slave device. The bytes may
     * then be retrieved with the #available and #read methods. Sends always
     * a STOP condition after the request.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireRequestFrom
     *
     * @param[in]   addr    7-bit address of the device to request bytes from
     * @param[in]   size    Number of bytes to request
     *
     * @return  number of bytes returned from the slave device
     */
	uint8_t requestFrom(uint8_t addr, uint8_t size);

    /**
     * @brief   Request bytes from a I2C slave device
     *
     * Used by the master to request bytes from a slave device. The bytes may
     * then be retrieved with the #available and #read methods.
     *
     * @param[in]   addr    7-bit address of the device to request bytes from
     * @param[in]   size    Number of bytes to request
     * @param[in]   stop    Send STOP condition after the request if true or
     *                      nothing if false.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireRequestFrom
     *
     * @return  number of bytes returned from the slave device
     */
    uint8_t requestFrom(uint8_t addr, uint8_t size, uint8_t stop);

    /**
     * @brief   Queue a byte for transmission from a master to slave device
     *
     * The method queues a byte for transmission from a master to slave device
     * in-between calls to #beginTransmission and #endTransmission.
     *
     * @param[in]   data    Data byte
     *
     * Copied from https://www.arduino.cc/en/Reference/WireWrite
     *
     * @return  number of bytes queued
     */
	virtual size_t write(uint8_t data);

    /**
     * @brief   Queue bytes for transmission from a master to slave device
     *
     * The method queues bytes for transmission from a master to slave device
     * in-between calls to #beginTransmission and #endTransmission.
     *
     * @param[in]   data    Array of data to send as bytes
     * @param[in]   size    Number of bytes to transmit
     *
     * Copied from https://www.arduino.cc/en/Reference/WireWrite
     *
     * @return  number of bytes queued
     */
	virtual size_t write(const uint8_t *data, size_t size);

    /**
     * @brief   Return the number of bytes available for retrieval
     *
     * Returns the number of bytes available for retrieval with #read. This
     * should be called on a master device after a call to #requestFrom.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireAvailable
     *
     * @return  number of bytes available for retrieval
     */
	virtual int available(void);

    /**
     * @brief   Reads one byte transmitted from slave device to the master
     *
     * Reads a byte that was transmitted from a slave device to the master after
     * a call to #requestFrom and removes it from receive buffer.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireRead
     *
     * @return  next byte received, or -1 if none is available.
     */
	virtual int read(void);

    /**
     * @brief   Read bytes transmitted from slave device to the master
     *
     * Reads a number of bytes that were transmitted from a slave device to the
     * master after a call to #requestFrom and removes them from receive buffer.
     *
     * @param[out]  buffer  buffer to store the bytes
     * @param[in]   length  number of bytes to read
     *
     * @return  number of bytes placed in the buffer
     */
	virtual size_t readBytes(uint8_t *buffer, size_t length);
    /**
     * @brief   Peeks one byte transmitted from slave device to the master
     *
     * Reads a byte that was transmitted from a slave device to the master after
     * a call to #requestFrom without advancing to the next one. That is,
     * successive calls to #peek will return the same value, as will the
     * next call to read.
     *
     * Copied from https://www.arduino.cc/en/Reference/WireRead and
     * https://www.arduino.cc/en/Reference/StreamPeek
     *
     * @return  next byte received, or -1 if none is available.
     */
	virtual int peek(void);

    /**
     * @brief   Flush the RX and TX buffer
     *
     * This method clears the RX as well as the TX buffer. It is not necessary
     * to call this method explicitly. RX buffer is flushed implicitly when
     * method #requestFrom is called. Tx buffer is flushed implicitly when
     * method #beginTransmission is called.
     */
	virtual void flush(void);
};

extern TwoWire Wire;

#endif /* WIREPORT_HPP */
/** @} */
