/*
 * Copyright (C) 2017   HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mag3110
 * @{
 *
 * @file
 * @brief       Default configuration for MAG3110 devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef MAG3110_PARAMS_H
#define MAG3110_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "mag3110.h"
#include "mag3110_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Default configuration parameters for the MAG3110 driver
 * @{
 */
#ifndef MAG3110_PARAM_I2C
#define MAG3110_PARAM_I2C       (I2C_DEV(0))
#endif
#ifndef MAG3110_PARAM_ADDR
#define MAG3110_PARAM_ADDR      (MAG3110_I2C_ADDRESS)
#endif
#ifndef MAG3110_PARAM_OFFSET
#define MAG3110_PARAM_OFFSET    { 0, 0, 0 }
#endif
#ifndef MAG3110_PARAMS
#define MAG3110_PARAMS          { .i2c    = MAG3110_PARAM_I2C,    \
                                  .addr   = MAG3110_PARAM_ADDR,   \
                                  .type   = MAG3110_ID,           \
                                  .dros   = MAG3110_DROS_DEFAULT, \
                                  .offset = MAG3110_PARAM_OFFSET }
#endif
#ifndef MAG3110_SAUL_INFO
#define MAG3110_SAUL_INFO       { .name = "mag3110" }
#endif
/**@}*/

/**
 * @brief   MAG3110 configuration
 */
static const mag3110_params_t mag3110_params[] =
{
    MAG3110_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mag3110_saul_info[] =
{
    MAG3110_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* MAG3110_PARAMS_H */
/** @} */
