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
#include "cryptoauthlib.h"

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
#define ATCA_PARAM_I2C           I2C_DEV(0)
#endif
#ifndef ATCA_PARAM_ADDR
#define ATCA_PARAM_ADDR          (ATCA_I2C_ADDR)
#endif
#ifndef ATCA_RX_RETRIES
#define ATCA_RX_RETRIES          (20)
#endif
#ifndef ATCA_DEVTYPE
#define ATCA_DEVTYPE            (ATECC508A)
#endif

#ifndef ATCA_PARAMS
#define ATCA_PARAMS                {    .iface_type = ATCA_I2C_IFACE, \
                                        .devtype = ATCA_DEVTYPE, \
                                        .atcai2c.slave_address = ATCA_PARAM_ADDR, \
                                        .atcai2c.bus = ATCA_PARAM_I2C, \
                                        .atcai2c.baud = -1,                        /**< Not used in RIOT */ \
                                        .wake_delay = 1500, \
                                        .rx_retries = ATCA_RX_RETRIES }
#endif

/**@}*/

/**
 * @brief   Allocation of ATCA device descriptors
 */
static const ATCAIfaceCfg atca_params[] =
{
    ATCA_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* ATCA_PARAMS_H */
/** @} */
