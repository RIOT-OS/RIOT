/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_bus_stats       Bus usage statistics
 * @ingroup     sys
 * @brief       This module wraps bus access functions with linker magic to
 *              obtain usage statistics. Currently, SPI and I2C statistic
 *              collection are supported via submodules.
 *
 *
 * # Usage
 *
 * Enable the `bus_stats_spi` module to enable SPI bus statistic collection.
 * Enable the `bus_stats_i2c` module to enable I2C bus statistics collection.
 *
 * The `shell_cmd_bus_stats` module additionally enables the `bust_stats` shell
 * command.
 *
 * # Terminology
 *
 * | Term       | Description                                                                               |
 * |:---------- |:----------------------------------------------------------------------------------------- |
 * | endpoint   | A specific SPI/I2C peripheral device (e.g. a sensor) connected to the bus                 |
 * | transfer   | A read, write, or read-write operation on the bus issued in one function call             |
 * | transaction| All transfers that happened back-to-back to the same endpoint without releasing the bus   |
 *
 * # Configuration
 *
 * Bus statistics will only be collected for preconfigured bus endpoints,
 * identified by the SPI bus number and the CS line, or the I2C bus number and
 * the I2C device address. These are added as array elements to the cross file
 * array (XFA).
 *
 * E.g. adding stats collection for the SPI bus `SDCARD_SPI_PARAM_SPI`
 * when used with the chip select line `SDCARD_SPI_PARAM_CS` can be done
 * by adding the following snipped to your applications C code:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * XFA(bus_stats_spi_xfa, 0) bus_stats_spi_t _sdcard_bus_stats = {
 *     .bus = SDCARD_SPI_PARAM_SPI,
 *     .cs = SDCARD_SPI_PARAM_CS,
 * };
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * # Histograms         {#bus_stats_histograms}
 *
 * The bus statistics make heavy use of a histogram data structure that
 * consists of \f(N\f) buckets (configurable via @ref BUS_STATS_HISTOGRAM_SIZE)
 * to record e.g. distributions of transaction sizes, transfer size,
 * durations of transfers, time between subsequent transfers and so on. The
 * size of the buckets increase exponentially to cover more ground with few
 * buckets, while also having a decent resolution at the lower end. The borders
 * of buckets are at powers of two to efficiently compute the buckets, except
 * for the lower bound of the first and the upper bound of the last bucket:
 *
 * | Bucket Index           | Lower Bound   | Upper Bound       |
 * |:---------------------- |:------------- |:----------------- |
 * | 0                      | 0             | 1                 |
 * | 1                      | 2             | 3                 |
 * | 2                      | 4             | 7                 |
 * | 3                      | 8             | 15                |
 * | \f(i | 0 < i < N\f)    | \f(2^{i}\f)   | \f(2^{i + 1}\f)   |
 * | \f(N)                  | \f(2^{N}\f)   | \f(\infty\f)      |
 *
 * # Design Goals
 *
 * The statistics collection is implemented as wrapper around the SPI / I2C
 * bus API that is enabled with linker flags. This results in a bit of
 * overhead added to each SPI / I2C API access, but will work out of the box
 * with all SPI / I2C drivers without awareness or cooperation from the
 * drivers.
 *
 * In addition, the interface is trivial with only the possibility to
 * clear and retrieve statistics and collection of stats always being on.
 * The assumption is that bus statistics are only enabled during debugging and
 * optimization sessions that involve flashing the MCU anyway, so that a
 * compile-time only option is sufficient. In addition, it is assumed that
 * accessing the statistics is done rarely (e.g. only after a sufficient amount
 * of data is collected), hence the `bus_stats_<BUS_TYPE>_get()` function is
 * optimized for simplicity in use and thread safety, rather then for being
 * efficient.
 *
 * @{
 *
 * @file
 * @brief       APIs and data structures used in the bus use statistics.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef BUS_STATS_H
#define BUS_STATS_H

#include <stdint.h>

#include "macros/utils.h"
#include "periph/i2c.h"
#include "periph/spi.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(BUS_STATS_HISTOGRAM_SIZE) || defined(DOXYGEN)
/**
 *  @brief  The number of buckets to use in the histogram
 */
#define BUS_STATS_HISTOGRAM_SIZE    10
#endif

#if BUS_STATS_HISTOGRAM_SIZE < 3
#error "implementation assumes BUS_STATS_HISTOGRAM_SIZE >= 3"
#endif

/**
 * @brief   Data structure collecting bus access statistics
 */
typedef struct {
    /**
     * @brief   Number of tractions to the monitored endpoint
     */
    uint32_t transaction_numof;
    /**
     * @brief   Total tile of all transactions to the monitored endpoint took
     */
    uint32_t time_of_use_us;
     /**
      * @brief  Time the last transaction ended
      */
    uint32_t last_access_us;
    /**
     * @brief   Time this stats were last cleared
     *
     * This can be used in combination with @ref bus_stats_t::time_of_use_us to
     * compute what fraction of the time the bus was blocked by communication
     * with the monitored endpoint.
     */
    uint32_t last_clear_us;
    /**
     * @brief   Histogram of transfer sizes
     *
     * See @ref bus_stats_histograms for the data format.
     *
     * This data is intended to draw a histogram to get an idea on how
     * DMA-friendly a driver is using an SPI/I2C bus.
     */
    uint32_t transfer_size_histogram[BUS_STATS_HISTOGRAM_SIZE];
    /**
     * @brief   Histogram of transaction sizes
     *
     * See @ref bus_stats_histograms for the data format.
     *
     * This data is intended to draw a histogram to get an idea on:
     *
     * 1. How long the SPI/I2C bus is blocked
     * 2. How efficient the driver is batching transfers (or in other terms,
     *    how much time/energy is wasted due to overhead for setting up the
     *    bus per byte transferred)
     */
    uint32_t transaction_size_histogram[BUS_STATS_HISTOGRAM_SIZE];
    /**
     * @brief   Histogram of times transactions took
     *
     * See @ref bus_stats_histograms for the data format. The durations
     * are stored in µs (microseconds).
     */
    uint32_t time_us_of_transactions_histogram[BUS_STATS_HISTOGRAM_SIZE];
    /**
     * @brief   Histogram of time between transactions
     *
     * See @ref bus_stats_histograms for the data format. The durations are
     * stored in ms (milliseconds).
     */
    uint32_t time_ms_between_transactions_histogram[BUS_STATS_HISTOGRAM_SIZE];
} bus_stats_t;

/**
 * @brief   The SPI endpoint identifier
 */
typedef struct {
    spi_t bus;                          /**< The SPI bus to collect statistics of */
    spi_cs_t cs;                        /**< The CS for which to collect statistics for */
    const char *name;                   /**< Name of the endpoint (short & human readable) */
} bus_stats_spi_endpoint_t;

/**
 * @brief   Data structure collecting SPI bus statistics
 */
typedef struct {
    bus_stats_t stats;                  /**< The statistics collected */
    bus_stats_spi_endpoint_t endpoint;  /**< The SPI endpoint the stats are collected for */
} bus_stats_spi_t;

/**
 * @brief   The I2C endpoint identifier
 */
typedef struct {
    i2c_t bus;                          /**< The I2C bus to collect statistics of */
    uint16_t addr;                      /**< The address to collect statistics for */
    const char *name;                   /**< Name of the endpoint (short & human readable) */
} bus_stats_i2c_endpoint_t;

/**
 * @brief   Data structure collecting I2C bus statistics
 */
typedef struct {
    bus_stats_t stats;                  /**< The statistics collected */
    bus_stats_i2c_endpoint_t endpoint;  /**< The I2C endpoint the stats are collected for */
} bus_stats_i2c_t;

XFA_USE(bus_stats_spi_t, bus_stats_spi_xfa);
XFA_USE(bus_stats_i2c_t, bus_stats_i2c_xfa);

/**
 * @brief   Get the number of SPI bus statistic entries
 *
 * @details This function will return the same result every time it is called,
 *          hence it is marked with attribute `pure`
 */
__attribute__((pure))
static inline size_t bus_stats_spi_numof(void)
{
    return XFA_LEN(bus_stats_spi_t, bus_stats_spi_xfa);
}

/**
 * @brief   Get the number of I2C bus statistic entries
 *
 * @details This function will return the same result every time it is called,
 *          hence it is marked with attribute `pure`
 */
__attribute__((pure))
static inline size_t bus_stats_i2c_numof(void) {
    return XFA_LEN(bus_stats_i2c_t, bus_stats_i2c_xfa);
}

/**
 * @brief   Get the current bus statistics for the bus stats entry at the given
 *          index
 *
 * @param[out]  dest    Write bus statistics here
 * @param[in]   idx     Index of the bus stats entry
 *
 * @pre     `(dest != NULL) && (idx < bus_stats_spi_numof())`
 *
 * @details This may block if the SPI bus is currently busy until bus is
 *          released. The upside is that the values written to @p dest will
 *          always be consistent.
 */
void bus_stats_spi_get(bus_stats_t *dest, size_t idx);

/**
 * @brief   Get the endpoint for the bus stats entry at a given index
 *
 * @param[in]   idx     Index of the bus stats entry
 *
 * @pre     `idx < bus_stats_spi_numof()`
 *
 * @return  A pointer to the entry identifier
 *
 * @details Unlike @ref bus_stats_spi_get this never blocks, as accesses to
 *          read only data do not need to be synchronized.
 */
static inline const bus_stats_spi_endpoint_t * bus_stats_spi_endpoint_get(size_t idx)
{
    return &bus_stats_spi_xfa[idx].endpoint;
}

/**
 * @brief   Clear SPI bus statistics for the stats entry at the given index
 *
 * @details This may block if the SPI bus is currently busy until bus is
 *          released. The stats will be cleared after that transaction.
 */
void bus_stats_spi_clear(size_t idx);

/**
 * @brief   Get the current bus statistics for the bus stats entry at the given
 *          index
 *
 * @param[out]  dest    Write bus statistics here
 * @param[in]   idx     Index of the bus stats entry
 *
 * @pre     `(dest != NULL) && (idx < bus_stats_spi_numof())`
 *
 * @details This may block if the SPI bus is currently busy until bus is
 *          released. The upside is that the values written to @p dest will
 *          always be consistent.
 */
void bus_stats_i2c_get(bus_stats_t *dest, size_t idx);

/**
 * @brief   Get the endpoint for the bus stats entry at a given index
 *
 * @param[in]   idx     Index of the bus stats entry
 *
 * @pre     `idx < bus_stats_i2c_numof()`
 *
 * @return  A pointer to the entry identifier
 *
 * @details Unlike @ref bus_stats_i2c_get this never blocks, as accesses to
 *          read only data do not need to be synchronized.
 */
static inline const bus_stats_i2c_endpoint_t * bus_stats_i2c_endpoint_get(size_t idx)
{
    return &bus_stats_i2c_xfa[idx].endpoint;
}

/**
 * @brief   Clear I2C bus statistics for the stats entry at the given index
 *
 * @details This may block if the I2C bus is currently busy until bus is
 *          released. The stats will be cleared after that transaction.
 */
void bus_stats_i2c_clear(size_t idx);

/**
 * @brief   Print the given bus statistics
 */
void bus_stats_print(bus_stats_t *stats);

/**
 * @brief   Internal function to add a value to a histrogry
 *
 * @details This is not intended to be used by application developers
 *
 * @param[out]  dest        Histogram to add an entry to
 * @param[in]   value       Value to add
 */
void bus_stats_histogram_add(uint32_t dest[BUS_STATS_HISTOGRAM_SIZE], uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* BUS_STATS_H */
/** @} */
