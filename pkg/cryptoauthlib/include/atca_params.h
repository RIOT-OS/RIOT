/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_cryptoauthlib
 * @{
 *
 * @file
 * @brief       Default configuration for Microchip CryptoAuth devices
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef ATCA_PARAMS_H
#define ATCA_PARAMS_H

#include "board.h"
#include "periph/i2c.h"
#include "atca.h"
#include "cryptoauthlib.h"

#include "kernel_defines.h"

#if IS_ACTIVE(CONFIG_PSA_SE_ATECCX08A)
#include "psa/crypto_values.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_atca_config    ATCA driver compile configuration
 * @ingroup  config_drivers_misc
 * @brief    Set default configuration parameters for the ATCA device
 *
 *          The CryptoAuth library defines the data structure ATCAIfaceCfg for
 *          device initialization. We use this instead of a self defined params
 *          struct and store it in the params array.
 *          ATCAIfaceCfg contains a variable for the bus address, which is never
 *          used by the library. We use it to store RIOT's I2C_DEV.
 *          We also initialize the baud rate with zero, because RIOT doesn't have
 *          an API to change baud.
 *
 * @{
 */

#ifndef ATCA_PARAM_I2C_DEV0
#define ATCA_PARAM_I2C_DEV0      I2C_DEV(0)
#endif

#if IS_ACTIVE(CONFIG_PSA_MULTIPLE_SECURE_ELEMENTS)
#ifndef ATCA_PARAM_I2C_DEV1
#define ATCA_PARAM_I2C_DEV1      I2C_DEV(1)
#endif
#endif

#ifndef ATCA_PARAM_ADDR
#define ATCA_PARAM_ADDR          (ATCA_I2C_ADDR)
#endif
#ifndef ATCA_RX_RETRIES
#define ATCA_RX_RETRIES          (20)
#endif
#ifndef ATCA_DEVTYPE
#define ATCA_DEVTYPE            (ATECC608A)
#endif

#if IS_ACTIVE(CONFIG_PSA_SE_ATECCX08A)
#define PSA_ATCA_LOCATION_DEV0  (PSA_KEY_LOCATION_SE_MIN + 1)
#define PSA_ATCA_LOCATION_DEV1  (PSA_KEY_LOCATION_SE_MIN + 2)
#endif

#ifndef ATCA_PARAMS_DEV0
#define ATCA_PARAMS_DEV0                {   .iface_type = ATCA_I2C_IFACE, \
                                            .devtype = ATCA_DEVTYPE, \
                                            .atcai2c.address = ATCA_PARAM_ADDR, \
                                            .atcai2c.bus = ATCA_PARAM_I2C_DEV0, \
                                            .atcai2c.baud = -1, /**< Not used in RIOT */ \
                                            .wake_delay = 1500, \
                                            .rx_retries = ATCA_RX_RETRIES }
#endif

#if IS_ACTIVE(CONFIG_PSA_MULTIPLE_SECURE_ELEMENTS)
#ifndef ATCA_PARAMS_DEV1
#define ATCA_PARAMS_DEV1                {   .iface_type = ATCA_I2C_IFACE, \
                                            .devtype = ATCA_DEVTYPE, \
                                            .atcai2c.address = ATCA_PARAM_ADDR, \
                                            .atcai2c.bus = ATCA_PARAM_I2C_DEV1, \
                                            .atcai2c.baud = -1, /**< Not used in RIOT */ \
                                            .wake_delay = 1500, \
                                            .rx_retries = ATCA_RX_RETRIES }
#endif
#endif

/**@}*/

/**
 * @brief   Structure to store ATCA device configuration
 */
typedef struct {
#if IS_ACTIVE(CONFIG_PSA_SE_ATECCX08A)
    psa_key_location_t atca_loc;
#endif
    ATCAIfaceCfg cfg;
} atca_params_t;

/**
 * @brief   Allocation of ATCA device descriptors
 */
static const atca_params_t atca_params[] =
{
#if IS_ACTIVE(CONFIG_PSA_MULTIPLE_SECURE_ELEMENTS)
    {
        .atca_loc = PSA_ATCA_LOCATION_DEV1,
        .cfg = ATCA_PARAMS_DEV1
    },
#endif
    {
#if IS_ACTIVE(CONFIG_PSA_SE_ATECCX08A)
        .atca_loc = PSA_ATCA_LOCATION_DEV0,
#endif
        .cfg = ATCA_PARAMS_DEV0
    }
};

#ifdef __cplusplus
}
#endif

#endif /* ATCA_PARAMS_H */
/** @} */
