/*
 * Copyright (C) 2016 University of California, Berkeley
 * Copyright (C) 2016 Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#pragma once

/**
 * @ingroup     drivers_mma7660
 * @{
 *
 * @file
 * @brief       Default configuration for MMA7660 accelerometer.
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 */


#include "board.h"
#include "saul_reg.h"
#include "mma7660.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for MMA7660 devices
 * @{
 */
#ifndef MMA7660_PARAM_I2C
#define MMA7660_PARAM_I2C         I2C_DEV(0)
#endif
#ifndef MMA7660_PARAM_ADDR
#define MMA7660_PARAM_ADDR        (MMA7660_ADDR)
#endif
#ifndef MMA7660_PARAM_AMSR
#define MMA7660_PARAM_AMSR        (MMA7660_SR_AM1)
#endif
#ifndef MMA7660_PARAM_AWSR
#define MMA7660_PARAM_AWSR        (MMA7660_SR_AW1)
#endif
#ifndef MMA7660_PARAM_FILT
#define MMA7660_PARAM_FILT        (1)
#endif

#ifndef MMA7660_PARAMS
#define MMA7660_PARAMS            { .i2c = MMA7660_PARAM_I2C,   \
                                    .addr = MMA7660_PARAM_ADDR, \
                                    .amsr = MMA7660_PARAM_AMSR, \
                                    .awsr = MMA7660_PARAM_AWSR, \
                                    .filt = MMA7660_PARAM_FILT}
#endif
#ifndef MMA7660_SAUL_INFO
#define MMA7660_SAUL_INFO         { .name = "mma7660" }
#endif
/**@}*/

/**
 * @brief   MMA7660 configuration
 */
static const mma7660_params_t mma7660_params[] =
{
    MMA7660_PARAMS,
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mma7660_saul_info[] =
{
    MMA7660_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
