/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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

#include "board.h"
#include "periph/i2c.h"
#include "atca.h"
#include "cryptoauthlib.h"

#include "kernel_defines.h"

#ifdef CUSTOM_ATCA_PARAMS
#include "custom_atca_params.h"
#endif

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
#include "psa/crypto_types.h"
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

#ifndef ATCA_PARAM_I2C
#define ATCA_PARAM_I2C  (I2C_DEV(0))
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

#ifndef ATCA_PARAMS
/**
 * @brief   Configuration parameters for the primary ATCA device
 */
#define ATCA_PARAMS                     {   .iface_type = ATCA_I2C_IFACE, \
                                            .devtype = ATCA_DEVTYPE, \
                                            .atcai2c.address = ATCA_PARAM_ADDR, \
                                            .atcai2c.bus = ATCA_PARAM_I2C, \
                                            .atcai2c.baud = -1, /**< Not used in RIOT */ \
                                            .wake_delay = 1500, \
                                            .rx_retries = ATCA_RX_RETRIES }
#endif

/**@}*/

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
/**
 * @brief   Structure to store ATCA device configuration
 */
typedef struct {
    psa_key_location_t atca_loc;
    ATCAIfaceCfg cfg;   /**< ATCA configuration parameters */
} atca_params_t;

/**
 * @brief   Allocation of ATCA device descriptors
 */
static const atca_params_t atca_params[] =
{
    ATCA_PARAMS
};
#else
/**
 * @brief   Allocation of ATCA device descriptors
 */
static const ATCAIfaceCfg atca_params[] =
{
    ATCA_PARAMS
};
#endif

/**
 * @brief   Number of connected devices
 */
#define ATCA_NUMOF (ARRAY_SIZE(atca_params))

/**
 * @brief   List of device pointers for all available devices
 */
extern ATCADevice atca_devs_ptr[ATCA_NUMOF];

#ifdef __cplusplus
}
#endif

/** @} */
