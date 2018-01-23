/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx150x
 * @{
 *
 * @file
 * @brief       Default parameters for SX150x devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SX150X_PARAMS_H
#define SX150X_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the SX150X
 * @{
 */
#ifndef SX150X_PARAM_I2C_DEV
#define SX150X_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef SX150X_PARAM_I2C_ADDR
#define SX150X_PARAM_I2C_ADDR        SX150X_ADDR_DEFAULT
#endif

#ifndef SX150X_PARAMS
#define SX150X_PARAMS                { .bus  = SX150X_PARAM_I2C_DEV,  \
                                       .addr = SX150X_PARAM_I2C_ADDR, }
#endif
/**@}*/

/**
 * @brief   Configure SX150x
 */
static const sx150x_params_t sx150x_params[] =
{
    SX150X_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* SX150X_PARAMS_H */
/** @} */
