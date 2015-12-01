/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_iotlab-m3
 * @{
 *
 * @file
 * @brief     ISL29020 board specific configuration
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ISL29020_PARAMS_H
#define ISL29020_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ISL29020 parameter configuration
 */
static const  isl29020_params_t isl29020_params[] =
{
    {
        .i2c = ISL29020_I2C,
        .addr = ISL29020_ADDR,
        .range = ISL29020_RANGE_16K,
        .mode = ISL29020_MODE_AMBIENT,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t isl29020_saul_info[] =
{
    {
        .name = "isl29020",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* ISL29020_PARAMS_H */
/** @} */
