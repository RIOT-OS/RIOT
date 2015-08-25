/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_periph_spi SPI
 * @ingroup     driver_periph
 * @brief       Low-level SPI peripheral driver interface
 *
 * @{
 * @file
 * @brief       Low-level SPI peripheral driver interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default SPI device access macro
 */
#ifndef SPI_DEV
#define SPI_DEV(x)      (x)
#endif

/**
 * @brief   Define global value for undefined SPI device
 */
#ifndef SPI_UNDEF
#define SPI_UNDEF       (-1)
#endif

/**
 * @brief   Default SPI chip select pin access macro
 */
#ifndef SPI_CS_DEV
#define SPI_CS_DEV(x)   (-x)
#endif

/**
 * @brief   Define value for unused CS line
 */
#ifndef SPI_CS_UNDEF
#define SPI_CS_UNDEF    (GPIO_UNDEF)
#endif

/**
 * @brief   Define default SPI device identifier type
 */
#ifndef HAVE_SPI_T
typedef int spi_t;
#endif

/**
 * @brief   Default CS pin definition
 */
#ifndef HAVE_SPI_CS_T
typedef gpio_t spi_cs_t;
#endif

/**
 * @brief   The SPI mode is defined by the four possible combinations of clock
 *          polarity and clock phase
 */
#ifndef HAVE_SPI_CONF_T
typedef enum {
    /**
     * The first data bit is sampled by the receiver on the first SCK edge. The
     * first edge of SCK is rising. This is sometimes also referred to as SPI
     * mode 0, or (CPOL=0, CPHA=0).
     */
    SPI_CONF_FIRST_RISING = 0,
    /**
     * The first data bit is sampled by the receiver on the second SCK edge. The
     * first edge of SCK is rising, i.e. the sampling edge is falling. This is
     * sometimes also referred to as SPI mode 1, or (CPOL=0, CPHA=1).
     */
    SPI_CONF_SECOND_RISING = 1,
    /**
     * The first data bit is sampled by the receiver on the first SCK edge. The
     * first edge of SCK is falling. This is sometimes also referred to as SPI
     * mode 2, or (CPOL=1, CPHA=0).
     */
    SPI_CONF_FIRST_FALLING = 2,
    /**
     * The first data bit is sampled by the receiver on the second SCK edge. The
     * first edge of SCK is falling, i.e. the sampling edge is rising. This is
     * sometimes also referred to as SPI mode 3, or (CPOL=1, CPHA=1).
     */
    SPI_CONF_SECOND_FALLING = 3
} spi_conf_t;
#endif

/**
 * @brief   Pre-defined set of SPI clock speeds
 *
 * The actual speed of the bus can vary to some extend, as the combination of
 * CPU clock and available prescaler values on certain platforms may not make
 * the exact values possible.
 */
#ifndef HAVE_SPI_SPEED_T
typedef enum {
    SPI_SPEED_100KHZ = 0,       /**< drive the SPI bus with 100KHz */
    SPI_SPEED_400KHZ,           /**< drive the SPI bus with 400KHz */
    SPI_SPEED_1MHZ,             /**< drive the SPI bus with 1MHz */
    SPI_SPEED_5MHZ,             /**< drive the SPI bus with 5MHz */
    SPI_SPEED_10MHZ             /**< drive the SPI bus with 10MHz */
} spi_speed_t;
#endif

/**
 * @brief   Configuration options for the chip select polarity
 */
#ifndef HAVE_SPI_CS_POL_T
typedef enum {
    SPI_CS_POL_LOW_ACTIVE,      /**< device is active when CS is set low */
    SPI_CS_POL_HIGH_ACTIVE,     /**< device is active when CS is set high */
} spi_cs_pol_t;
#endif

/**
 * @brief   Prototype for callback that is called on ship select events in slave
 *          mode
 *
 * @param[in] start     True if CS was just activated, false if it was
 *                      deactivated
 *
 * @return              Character that is send to the master on the first
 *                      transfer period. If @p start is false, the return value
 *                      should be ignored
 */
typedef uint8_t(spi_cs_cb_t)(void *arg, bool start);

/**
 * @brief   Prototype for callback that is called after every transfered byte
 *          in slave mode
 *
 * @param[in] data      Byte that was received on the last transfer period
 * @param[in] fresh     Flag set true if CS was just activated, false on
 *
 * @return              The character that is send to the master on the next
 *                      transfer period
 */
typedef uint8_t(spi_data_cb_t)(void *arg, uint8_t data);

/**
 * @brief   Initialize the given SPI device to work in master mode
 *
 * In master mode the SPI device is configured to control the SPI bus. This
 * means the device will start and end all communication on the bus and control
 * the CLK line. For transferring data on the bus the below defined transfer
 * functions should be used.
 *
 * @param[in] dev       SPI device to initialize
 * @param[in] conf      Mode of clock phase and clock polarity
 * @param[in] speed     desired clock speed for driving the SPI bus
 *
 * @return              0 on success
 * @return              -1 on undefined device given
 * @return              -2 on unavailable speed value
 * @return              -3 on configuration not supported
 * @return              -4 on other errors
 */
int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed);

/**
 * @brief   Initialize the given SPI device to work in slave mode
 *
 * In slave mode the SPI device is purely reacting to the bus. Transaction will
 * be started and ended by a connected SPI master. When a byte is received, the
 * callback is called in interrupt context with this byte as argument. The
 * return byte of the callback is transferred to the master in the next
 * transmission cycle. This interface enables easy implementation of a register
 * based access paradigm for the SPI slave.
 *
 * @param[in] dev       The SPI device to initialize as SPI slave
 * @param[in] conf      Mode of clock phase and polarity
 * @param[in] cb        callback called every time a byte was received
 *
 * @return              0 on success
 * @return              -1 on undefined device given
 * @return              -2 on slave mode not supported
 * @return              -3 on other errors
 */
int spi_init_slave(spi_t dev, spi_cs_t cs, spi_cs_pol_t pol, spi_conf_t conf,
                   spi_cs_cb_t cs_cb, spi_data_cb_t data_cb, void *arg);

/**
 * @brief   Initialize the given chip select pin
 *
 * @param[in] dev       SPI device to use with the CS pin
 * @param[in] cs        Chip select pin to initialize
 * @param[in] pol       Polarity to use
 *
 * @return              0 on success
 * @return              -1 on undefined device given
 * @return              -2 on other errors
 */
int spi_init_cs(spi_t dev, spi_cs_t cs, spi_cs_pol_t pol);

/**
 * @brief   Get mutually exclusive access to the given SPI bus
 *
 * In case the SPI device is busy, this function will block until the bus is
 * free again.
 *
 * @param[in] dev       SPI device to access
 */
void spi_acquire(spi_t dev);

/**
 * @brief   Release the given SPI device to be used by others
 *
 * @param[in] dev       SPI device to release
 */
void spi_release(spi_t dev);

/**
 * @brief   Transfer one byte on the given SPI bus
 *
 * @param[in] dev       SPI device to use
 * @param[in] cs        Chip select line to use
 * @param[in] cont      Set true for leaving CS active after transfer
 * @param[in] out       Byte to send out, set NULL if only receiving
 *
 * @return              Byte that was read from the slave
 */
uint8_t spi_transfer_byte(spi_t dev, spi_cs_t cs, bool cont, uint8_t out);

/**
 * @brief   Transfer a number bytes on the given SPI bus
 *
 * @param[in] dev       SPI device to use
 * @param[in] cs        Chip select line to use
 * @param[in] cont      Set true for leaving CS active after transfer
 * @param[in] out       Array of bytes to send, set NULL if only receiving
 * @param[out] in       Buffer to receive bytes to, set NULL if only sending
 * @param[in] len       Number of bytes to transfer
 */
void spi_transfer_bytes(spi_t dev, spi_cs_t cs, bool cont,
                        uint8_t *out, uint8_t *in, size_t len);

/**
 * @brief   Transfer one byte to/from a given register address
 *
 * This function is a shortcut function for easier handling of register based
 * SPI devices. As many SPI devices use a register based addressing scheme, this
 * function is a convenient shortcut for interfacing with such devices.
 *
 * @param[in] dev       SPI device to use
 * @param[in] cs        Chip select line to use
 * @param[in] cont      Set true for leaving CS active after transfer
 * @param[in] reg       Register address to transfer data to/from
 * @param[in] out       Byte to send, set NULL if only receiving data
 * @param[out] in       Byte to read, set NULL if only sending
 *
 * @return              Value that was read from the slave's register
 */
uint8_t spi_transfer_reg(spi_t dev, spi_cs_t cs, bool cont, uint8_t reg,
                         uint8_t out);

/**
 * @brief   Transfer a number of bytes from/to a given register address
 *
 * This function is a shortcut function for easier handling of register based
 * SPI devices. As many SPI devices use a register based addressing scheme, this
 * function is a convenient shortcut for interfacing with such devices.
 *
 * @param[in] dev       SPI device to use
 * @param[in] cs        Chip select line to use
 * @param[in] cont      Set true for leaving CS active after transfer
 * @param[in] reg       Register address to transfer data to/from
 * @param[in] out       Byte array to send data from, set NULL if only receiving
 * @param[out] in       Byte buffer to read into, set NULL if only sending
 * @param[in] len       Number of bytes to transfer
 */
void spi_transfer_regs(spi_t dev, spi_cs_t cs, bool cont,
                       uint8_t reg, uint8_t *out, uint8_t *in, size_t len);

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

#ifdef __cplusplus
}
#endif

#endif /* SPI_H */
/** @} */
