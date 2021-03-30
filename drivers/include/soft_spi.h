/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_soft_spi Soft SPI
 * @ingroup     drivers_soft_periph
 * @brief       Software implemented Serial Peripheral Interface bus
 *
 * This module provides a software implemented Serial Peripheral Interface bus.
 * It is intended to be used in situation where hardware spi is not available.
 * The signatures of the functions are identical to the functions declared in
 * `spi.h`. The clock speed is approximated by using `xtimer_nanosleep()`.
 *
 * To use the software SPI as drop-in replacement for `periph_spi`, add the
 * following to your application's Makefile:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USEMODULE += soft_spi_as_periph_spi
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This will hook up the functions defined here into the `periph_spi` interface,
 * so that a call to e.g. `spi_acquire()` becomes an alias for
 * `soft_spi_acquire()`. Hence, it is not possible to real `periph_spi` and
 * `soft_spi_as_periph_spi` at the same time. Using `soft_spi_as_periph_spi`
 * will therefore disable any other `periph_spi` implementation, when used.
 *
 * Without `soft_spi_as_periph_spi`, you can use both `soft_spi` and
 * `periph_spi` at the same time.
 *
 * @{
 *
 * @file
 * @brief       Software SPI port descriptor definition
 *
 * @author      Markus Blechschmidt <Markus.Blechschmidt@haw-hamburg.de>
 */

#ifndef SOFT_SPI_H
#define SOFT_SPI_H

#include "board.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOFT_SPI_NUMOF
#define SOFT_SPI_NUMOF 1
#endif

/**
 * @defgroup drivers_softspi_config      Software SPI driver compile configuration
 * @ingroup config_drivers_soft_periph
 * @{
 */
/**
 * @brief   Default SPI device access macro
 */
#ifndef SOFT_SPI_DEV
#define SOFT_SPI_DEV(x)      (x)
#endif

/**
 * @brief   Define global value for undefined SPI device
 */
#ifndef SOFT_SPI_UNDEF
#define SOFT_SPI_UNDEF       (UINT_MAX)
#endif

/**
 * @brief   Define value for unused CS line
 */
#ifndef SOFT_SPI_CS_UNDEF
#define SOFT_SPI_CS_UNDEF    (GPIO_UNDEF)
#endif
/** @} */

/**
 * @brief   Default type for SPI devices
 */
typedef unsigned int soft_spi_t;

/**
 * @brief   Chip select pin type overlaps with gpio_t so it can be casted to
 *          this
 */
typedef gpio_t soft_spi_cs_t;

/**
 * @brief   Status codes used by the SPI driver interface
 */
enum {
    SOFT_SPI_OK          = SPI_OK,      /**< everything went as planned */
    SOFT_SPI_NODEV       = SPI_NODEV,   /**< invalid SPI bus specified */
    SOFT_SPI_NOCS        = SPI_NOCS,    /**< invalid chip select line specified */
    SOFT_SPI_NOMODE      = SPI_NOMODE,  /**< selected mode is not supported */
    SOFT_SPI_NOCLK       = SPI_NOCLK    /**< selected clock value is not supported */
};

/**
 * @brief   Available SPI modes, defining the configuration of clock polarity
 *          and clock phase
 *
 * @deprecated  Directly use spi_mode_t instead
 */
typedef spi_mode_t soft_spi_mode_t;

/**
 * @name    Deprecated aliases
 * @{
 */
#define SOFT_SPI_MODE_0 SPI_MODE_0 /**< Deprecated alias for SPI_MODE_0 */
#define SOFT_SPI_MODE_1 SPI_MODE_1 /**< Deprecated alias for SPI_MODE_1 */
#define SOFT_SPI_MODE_2 SPI_MODE_2 /**< Deprecated alias for SPI_MODE_2 */
#define SOFT_SPI_MODE_3 SPI_MODE_3 /**< Deprecated alias for SPI_MODE_3 */

#define SOFT_SPI_CLK_100KHZ SPI_CLK_100KHZ /**< Deprecated alias for SPI_CLK_100KHZ */
#define SOFT_SPI_CLK_400KHZ SPI_CLK_400KHZ /**< Deprecated alias for SPI_CLK_400KHZ */
#define SOFT_SPI_CLK_DEFAULT SPI_CLK_10MHZ /**< Deprecated alias for SPI_CLK_10MHZ */
/** @} */

/**
 * @brief   Available SPI clock speeds
 *
 * The actual speed of the bus varies between CPUs and depends on the speed
 * of the processing. The values of the enum entries represent the approximate
 * delay between two clock edges.
 *
 * @deprecated  Directly use spi_clk_t instead
 */
typedef spi_clk_t soft_spi_clk_t;

/**
 * @brief Software SPI port descriptor
 */
typedef struct {
    gpio_t miso_pin;                /**< MOSI pin */
    gpio_t mosi_pin;                /**< MOSI pin */
    gpio_t clk_pin;                 /**< CLK pin */
    spi_mode_t soft_spi_mode;       /**< data and clock polarity */
    uint16_t sleep_ns;              /**< nanoseconds before toggling the clock pin */
} soft_spi_conf_t;

/**
 * @brief   Basic initialization of the given SPI bus
 *
 * This function does the basic initialization including pin configuration for
 * MISO, MOSI, and CLK pins.
 *
 * Errors (e.g. invalid @p bus parameter) are not signaled through a return
 * value, but should be signaled using the assert() function internally.
 *
 * @note    This function MUST not be called more than once per bus!
 *
 * @param[in] bus       SPI device to initialize
 */
void soft_spi_init(soft_spi_t bus);

/**
 * @brief   Initialize the used SPI bus pins, i.e. MISO, MOSI, and CLK
 *
 * After calling soft_spi_init, the pins must be initialized. In normal cases,
 * this function will not be used.
 *
 * The pins used are configured in the board's periph_conf.h.
 *
 * @param[in] bus       SPI device the pins are configure for
 */
void soft_spi_init_pins(soft_spi_t bus);

/**
 * @brief   Initialize the given chip select pin
 *
 * The chip select must be any generic GPIO pin (e.g. GPIO_PIN(x,y)). It must be
 * called once before the use of the chip select pin in transaction.
 *
 * @param[in] bus       SPI device that is used with the given CS line
 * @param[in] cs        chip select pin to initialize
 *
 * @retval  0           success
 * @retval  -ENXIO      invalid device
 * @retval  -EINVAL     invalid CS pin/line
 */
int soft_spi_init_cs(soft_spi_t bus, soft_spi_cs_t cs);

/**
 * @brief   Start a new SPI transaction
 *
 * Starting a new SPI transaction will get exclusive access to the SPI bus
 * and configure it according to the given values. If another SPI transaction
 * is active when this function is called, this function will block until the
 * other transaction is complete (soft_spi_relase was called).
 *
 * @note    This function expects the @p bus and the @p cs parameters to be
 *          valid (they are checked in soft_spi_init and soft_spi_init_cs before)
 * @warning For clock speeds higher than 400 kHz in @p clk, the SPI is driven as fast as the
 *          implementation can manage. Don't expect the soft SPI to be able to compete with
 *          peripheral SPI implementations in clock speed.
 *
 * @param[in] bus       SPI device to access
 * @param[in] cs        chip select pin/line to use
 * @param[in] mode      mode to use for the new transaction
 * @param[in] clk       bus clock speed to use for the transaction
 *
 * @retval  0           success
 * @retval  -EINVAL     Invalid mode in @p mode
 * @retval  -ENOTSUP    Unsupported but valid mode in @p mode
 */
int soft_spi_acquire(soft_spi_t bus, soft_spi_cs_t cs, soft_spi_mode_t mode, soft_spi_clk_t clk);

/**
 * @brief   Finish an ongoing SPI transaction by releasing the given SPI bus
 *
 * After release, the given SPI bus should be fully powered down until acquired
 * again.
 *
 * @param[in] bus       SPI device to release
 */
void soft_spi_release(soft_spi_t bus);

/**
 * @brief Transfer one byte on the given SPI bus
 * Currently only the use of MOSI in master mode is implemented. Therefore receiving
 * data from a slave is currently not possible.
 *
 * @param[in] bus       SPI device to use
 * @param[in] cs        chip select pin/line to use
 * @param[in] cont      if true, keep device selected after transfer
 * @param[in] out       byte to send out, set NULL if only receiving
 *
 * @return              the received byte
 */
uint8_t soft_spi_transfer_byte(soft_spi_t bus, soft_spi_cs_t cs, bool cont, uint8_t out);

/**
 * @brief   Transfer a number bytes using the given SPI bus
 *
 * @param[in]  bus      SPI device to use
 * @param[in]  cs       chip select pin/line to use
 * @param[in]  cont     if true, keep device selected after transfer
 * @param[in]  out      buffer to send data from, set NULL if only receiving
 * @param[out] in       buffer to read into, set NULL if only sending
 * @param[in]  len      number of bytes to transfer
 */
void soft_spi_transfer_bytes(soft_spi_t bus, soft_spi_cs_t cs, bool cont,
                             const void *out, void *in, size_t len);

/**
 * @brief   Transfer one byte to/from a given register address
 *
 * This function is a shortcut function for easier handling of SPI devices that
 * implement a register based access scheme.
 *
 * @param[in] bus       SPI device to use
 * @param[in]  cs       chip select pin/line to use
 * @param[in] reg       register address to transfer data to/from
 * @param[in] out       byte to send, set NULL if only receiving data
 *
 * @return              value that was read from the given register address
 */
uint8_t soft_spi_transfer_reg(soft_spi_t bus, soft_spi_cs_t cs, uint8_t reg, uint8_t out);

/**
 * @brief   Transfer a number of bytes to/from a given register address
 *
 * This function is a shortcut function for easier handling of SPI devices that
 * implement a register based access scheme.
 *
 * @param[in]  bus      SPI device to use
 * @param[in]  cs       chip select pin/line to use
 * @param[in]  reg      register address to transfer data to/from
 * @param[in]  out      buffer to send data from, set NULL if only receiving
 * @param[out] in       buffer to read into, set NULL if only sending
 * @param[in]  len      number of bytes to transfer
 */
void soft_spi_transfer_regs(soft_spi_t bus, soft_spi_cs_t cs, uint8_t reg,
                            const void *out, void *in, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* SOFT_SPI_H */
/** @} */
