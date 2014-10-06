/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level SPI peripheral driver
 * @{
 *
 * @file
 * @brief       Low-level SPI peripheral driver interface definitions
 *
 * The current design of this interface targets implementations that use the SPI in blocking mode.
 *
 * TODO: add means for asynchronous SPI usage
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __SPI_H
#define __SPI_H

#include "periph_conf.h"

/* add guard for the case that no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief Definition available SPI devices
 */
typedef enum {
#if SPI_0_EN
    SPI_0 = 0,          /**< SPI device 0 */
#endif
#if SPI_1_EN
    SPI_1,              /**< SPI device 1 */
#endif
#if SPI_2_EN
    SPI_2,              /**< SPI device 2 */
#endif
#if SPI_3_EN
    SPI_3,              /**< SPI device 3 */
#endif
} spi_t;

/**
 * @brief The SPI mode is defined by the four possible combinations of clock polarity and
 *        clock phase.
 */
typedef enum {
    SPI_CONF_FIRST_RISING = 0,  /**< first data bit is transacted on the first rising SCK edge */
    SPI_CONF_SECOND_RISING,     /**< first data bit is transacted on the second rising SCK edge */
    SPI_CONF_FIRST_FALLING,     /**< first data bit is transacted on the first falling SCK edge */
    SPI_CONF_SECOND_FALLING     /**< first data bit is transacted on the second falling SCK edge */
} spi_conf_t;

/**
 * @brief Define a set of pre-defined SPI clock speeds.
 *
 * The actual speed of the bus can vary to some extend, as the combination of CPU clock and
 * available prescale values on certain platforms may not make the exact values possible.
 */
typedef enum {
    SPI_SPEED_100KHZ = 0,       /**< drive the SPI bus with 100KHz */
    SPI_SPEED_400KHZ,           /**< drive the SPI bus with 400KHz */
    SPI_SPEED_1MHZ,             /**< drive the SPI bus with 1MHz */
    SPI_SPEED_5MHZ,             /**< drive the SPI bus with 5MHz */
    SPI_SPEED_10MHZ             /**< drive the SPI bus with 10MHz */
} spi_speed_t;

/**
 * @brief Initialize the given SPI device to work in master mode
 *
 * In master mode the SPI device is configured to control the SPI bus. This means the device
 * will start and end all communication on the bus and control the CLK line. For transferring
 * data on the bus the below defined transfer functions should be used.
 *
 * @param[in] dev       SPI device to initialize
 * @param[in] conf      Mode of clock phase and clock polarity
 * @param[in] speed     desired clock speed for driving the SPI bus
 *
 * @return              0 on success
 * @return              -1 on unavailable speed value
 * @return              -2 on other errors
 */
int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed);

/**
 * @brief Initialize the given SPI device to work in slave mode
 *
 * In slave mode the SPI device is purely reacting to the bus. Transaction will be started and
 * ended by a connected SPI master. When a byte is received, the callback is called in interrupt
 * context with this byte as argument. The return byte of the callback is transferred to the
 * master in the next transmission cycle. This interface enables easy implementation of a register
 * based access paradigm for the SPI slave.
 *
 * @param[in] dev       The SPI device to initialize as SPI slave
 * @param[in] conf      Mode of clock phase and polarity
 * @param[in] cb        callback called every time a byte was received
 *
 * @return              0 on success
 * @return              -1 on error
 */
int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data));

/**
 * @brief Configure SCK, MISO and MOSI pins for the given SPI device
 *
 * @param[in] dev       SPI device to use
 *
 * @return              0 on success
 * @return              -1 on error
 */
int spi_conf_pins(spi_t dev);

/**
 * @brief Transfer one byte on the given SPI bus
 *
 * @param[in] dev       SPI device to use
 * @param[in] out       Byte to send out, set NULL if only receiving
 * @param[out] in       Byte to read, set NULL if only sending
 *
 * @return              Number of bytes that were transfered
 * @return              -1 on error
 */
int spi_transfer_byte(spi_t dev, char out, char *in);

/**
 * @brief Transfer a number bytes on the given SPI bus
 *
 * @param[in] dev       SPI device to use
 * @param[in] out       Array of bytes to send, set NULL if only receiving
 * @param[out] in       Buffer to receive bytes to, set NULL if only sending
 * @param[in] length    Number of bytes to transfer
 *
 * @return              Number of bytes that were transfered
 * @return              -1 on error
 */
int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length);

/**
 * @brief Transfer one byte to/from a given register address
 *
 * This function is a shortcut function for easier handling of register based SPI devices. As
 * many SPI devices use a register based addressing scheme, this function is a convenient short-
 * cut for interfacing with such devices.
 *
 * @param[in] dev       SPI device to use
 * @param[in] reg       Register address to transfer data to/from
 * @param[in] out       Byte to send, set NULL if only receiving data
 * @param[out] in       Byte to read, set NULL if only sending
 *
 * @return              Number of bytes that were transfered
 * @return              -1 on error
 */
int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in);

/**
 * @brief Transfer a number of bytes from/to a given register address
 *
 * This function is a shortcut function for easier handling of register based SPI devices. As
 * many SPI devices use a register based addressing scheme, this function is a convenient short-
 * cut for interfacing with such devices.
 *
 * @param[in] dev       SPI device to use
 * @param[in] reg       Register address to transfer data to/from
 * @param[in] out       Byte array to send data from, set NULL if only receiving
 * @param[out] in       Byte buffer to read into, set NULL if only sending
 * @param[in] length    Number of bytes to transfer
 *
 * @return              Number of bytes that were transfered
 * @return              -1 on error
 */
int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length);

/**
 * @brief Tell the SPI driver that a new transaction was started. Call only when SPI in slave mode!
 *
 * @param[in] dev       SPI device that is active
 * @param[in] reset_val The byte that is send to the master as first byte
 */
void spi_transmission_begin(spi_t dev, char reset_val);

/**
 * @brief Power on the given SPI device
 *
 * @param[in] dev       SPI device to power on
 */
void spi_poweron(spi_t dev);

/**
 * @brief Power off the given SPI device
 *
 * @param[in] dev       SPI device to power off
 */
void spi_poweroff(spi_t dev);

#endif /* SPI_NUMOF */

#endif /* __SPI_H */
/** @} */
