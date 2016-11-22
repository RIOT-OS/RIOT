/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pba-d-01-kw2x
 * @{
 *
 * @file
 * @brief       MMA8x5x board specific configuration
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
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
 * @brief    MMA852 configuration
 */
static const  mma8x5x_params_t mma8x5x_params[] =
{
    {
        .i2c    = MMA8652_I2C,
        .addr   = MMA8652_ADDR,
        .type   = MMA8X5X_TYPE_MMA8652,
        .rate   = MMA8X5X_RATE_200HZ,
        .range  = MMA8X5X_RANGE_2G,
        .offset = { 0, 0, 0 }
    }
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
