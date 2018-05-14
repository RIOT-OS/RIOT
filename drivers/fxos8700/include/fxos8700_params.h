/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_fxos8700
 *
 * @{
 * @file
 * @brief       Default configuration for FXOS8700 devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef FXOS8700_PARAMS_H
#define FXOS8700_PARAMS_H

#include "board.h"
#include "fxos8700.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default configuration parameters for the FXOS8700 driver
 *
 * The valid address range is 0x1E - 0x1F depending on the configuration of the
 * address pins SA0 and SA1.
 *
 * @{
 */
#ifndef FXOS8700_PARAM_I2C
#define FXOS8700_PARAM_I2C            I2C_DEV(0)
#endif
#ifndef FXOS8700_PARAM_ADDR
#define FXOS8700_PARAM_ADDR           0x1E
#endif
#ifndef FXOS8700_PARAM_ACC_RANGE
#define FXOS8700_PARAM_ACC_RANGE      FXOS8700_REG_XYZ_DATA_CFG_FS__8G
#endif
#ifndef FXOS8700_PARAM_RENEW_INTERVAL
#define FXOS8700_PARAM_RENEW_INTERVAL 1000000ul
#endif

#ifndef FXOS8700_PARAMS
#define FXOS8700_PARAMS               { .i2c  = FXOS8700_PARAM_I2C, \
                                        .addr = FXOS8700_PARAM_ADDR, \
                                        .acc_range = FXOS8700_PARAM_ACC_RANGE, \
                                        .renew_interval = FXOS8700_PARAM_RENEW_INTERVAL }
#endif
#ifndef FXOS8700_SAUL_INFO
#define FXOS8700_SAUL_INFO            { .name = "fxos8700" }
#endif
/**@}*/

/**
 * @brief   FXOS8700 configuration
 */
static const fxos8700_params_t fxos8700_params[] =
{
    FXOS8700_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t fxos8700_saul_info[] =
{
    FXOS8700_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* FXOS8700_PARAMS_H */
/** @} */
