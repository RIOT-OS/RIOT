/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mma8x5x
 * @{
 *
 * @file
 * @brief       Default configuration for MMA8x5x devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MMA8X5X_PARAMS_H
#define MMA8X5X_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "mma8x5x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the MMA8x5x driver
 * @{
 */
#ifndef MMA8X5X_PARAM_I2C
#define MMA8X5X_PARAM_I2C       (I2C_DEV(0))
#endif
#ifndef MMA8X5X_PARAM_ADDR
#define MMA8X5X_PARAM_ADDR      (MMA8X5X_I2C_ADDRESS)
#endif
#ifndef MMA8X5X_PARAM_TYPE
#define MMA8X5X_PARAM_TYPE      (MMA8X5X_TYPE_MMA8652)
#endif
#ifndef MMA8X5X_PARAM_RATE
#define MMA8X5X_PARAM_RATE      (MMA8X5X_RATE_200HZ)
#endif
#ifndef MMA8X5X_PARAM_RANGE
#define MMA8X5X_PARAM_RANGE     (MMA8X5X_RANGE_2G)
#endif
#ifndef MMA8X5X_PARAM_OFFSET
#define MMA8X5X_PARAM_OFFSET    { 0, 0, 0 }
#endif

#ifndef MMA8X5X_PARAMS
#define MMA8X5X_PARAMS          { .i2c    = MMA8X5X_PARAM_I2C, \
                                  .addr   = MMA8X5X_PARAM_ADDR, \
                                  .type   = MMA8X5X_PARAM_TYPE, \
                                  .rate   = MMA8X5X_PARAM_RATE, \
                                  .range  = MMA8X5X_PARAM_RANGE, \
                                  .offset = MMA8X5X_PARAM_OFFSET }
#endif
/**@}*/

/**
 * @brief   MMA8x5x configuration
 */
static const mma8x5x_params_t mma8x5x_params[] =
{
    MMA8X5X_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mma8x5x_saul_info[] =
{
    {
        .name = "mma8652"
    }
};

#ifdef __cplusplus
}
#endif

#endif /* MMA8X5X_PARAMS_H */
/** @} */
