/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_pba-d-01-kw2x
 * @{
 *
 * @file
 * @brief     MMA8652 board specific configuration
 *
 * @author    Cenk Gündoğan <mail@cgundogan.de>
 */

#ifndef MMA8652_PARAMS_H
#define MMA8652_PARAMS_H

#include "board.h"
#include "saul_reg.h"
#include "mma8652.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    MMA852 configuration
 */
static const  mma8652_params_t mma8652_params[] =
{
    {
        .i2c = MMA8652_I2C,
        .addr = MMA8652_ADDR,
        .rate = MMA8652_DATARATE_DEFAULT,
        .scale = MMA8652_FS_RANGE_DEFAULT,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mma8652_saul_info[] =
{
    {
        .name = "mma8652",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* MMA8652_PARAMS_H */
/** @} */
