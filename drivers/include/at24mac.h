/*
 * SPDX-FileCopyrightText: 2019 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_at24mac AT24MAC unique ID chip
 * @ingroup     drivers_misc
 * @brief       Device driver interface for the AT24MAC I2C chip
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdint.h>
#include "net/eui48.h"
#include "net/eui64.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device handle type for AT24Mac devices
 */
typedef uint_fast8_t at24mac_t;

#define AT24MAC_ID_LEN  (16)            /**< Length of ID128 */

/**
 * @brief   Type of the AT24Mac device
 */
typedef enum {
    AT24MAC4XX,                         /**< provides EUI-48 */
    AT24MAC6XX                          /**< provides EUI-64 */
} at24mac_type_t;

/**
 * @brief   struct holding all params needed for device communication
 */
typedef struct {
    i2c_t i2c_dev;                      /**< I2C device      */
    uint8_t i2c_addr;                   /**< I2C address     */
    at24mac_type_t type;                /**< Device type     */
} at24mac_params_t;

/**
 * @brief   Get the unique EUI48 address from a AT24MAC4xx chip
 *
 * @param[in] dev       Index of the AT24Mac chip in the at24mac_params
 *                      array.
 * @param[out] addr     memory location to copy the address into.
 *
 * @return              0 on success, error otherwise.
 */
int at24mac_get_eui48(at24mac_t dev, eui48_t *addr);

/**
 * @brief   Get the unique EUI64 address from a AT24MAC6xx chip
 *
 * @param[in] dev       Index of the AT24Mac chip in the at24mac_params
 *                      array.
 * @param[out] addr     memory location to copy the address into.
 *
 * @return              0 on success, error otherwise.
 */
int at24mac_get_eui64(at24mac_t dev, eui64_t *addr);

/**
 * @brief   Get the unique ID from a AT24MACxxx chip
 *
 * @param[in] dev       Index of the AT24MAC chip in the at24mac_params
 *                      array.
 * @param[out] dst      memory location to copy the ID into.
 *                      Must be able to hold at least @ref AT24MAC_ID_LEN bytes.
 *
 * @return              0 on success, error otherwise.
 */
int at24mac_get_id128(at24mac_t dev, void *dst);

/**
 * @brief   Get the type of a AT24MACxxx chip
 *
 * @param[in] dev       Index of the AT24MAC chip in the at24mac_params
 *                      array.
 *
 * @return              The type of the device (AT24MAC4XX or AT24MAC6XX)
 */
at24mac_type_t at24mac_get_type(at24mac_t dev);

#ifdef __cplusplus
}
#endif

/** @} */
