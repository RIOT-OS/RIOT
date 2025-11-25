/*
 * SPDX-FileCopyrightText: 2009 Freie Universitaet Berlin (FUB)
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_sht1x
 * @{
 *
 * @file
 * @brief       Internal defines required by the SHT10/SHT11/SHT15 driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdint.h>
#include <periph/gpio.h>
#include <mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Possible values to pass as `ack` parameter to `write_byte`
 * @see write_byte
 * @{
 */
#define SHT1X_NO_ACK           (1)      /**< don't ack read in `read_byte` */
#define SHT1X_ACK              (0)      /**< do acknowledge read in `read_byte` */
/** @} */

/**
 * @name Commands that can be sent to the SHT1X driver
 * @{
 */
#define SHT1X_STATUS_REG_W     (0x06)   /**< will write to status register */
#define SHT1X_STATUS_REG_R     (0x07)   /**< will read from status register */
#define SHT1X_MEASURE_TEMP     (0x03)   /**< tell sensor to measure temperature */
#define SHT1X_MEASURE_HUM      (0x05)   /**< tell sensor to measure humidity */
#define SHT1X_RESET            (0x1E)   /**< reset the sensor */
/** @} */

/**
 * @name Timing parameters for the SHT10/SHT1X/SHT15
 * @{
 */
#define SHT1X_HALF_CLOCK        (1)     /**< Half clock length in µsec */
#define SHT1X_MEASURE_TIMEOUT   (1000)  /**< Timeout for the SHT1x to complete
                                             the measurement (in millisec) */
#define SHT1X_RESET_WAIT        (11000) /**< Wait 11ms after soft reset */
/** @} */

#define SHT1X_CONF_MASK         (0x07)  /**< Bitmask to get writable bits of the
                                             status byte */
#define SHT1X_SAUL_RETRIES      (3)     /**< How often reading the sensor should
                                             be retried in case of communication
                                             failures */

#ifdef __cplusplus
}
#endif

/** @} */
