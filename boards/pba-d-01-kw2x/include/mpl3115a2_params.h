/*
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
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
 * @brief     mpl3115a2 board specific configuration
 *
 * @author    Johann Fischer <j.fischer@phytec.de>
 */

#ifndef MPL3115A2_PARAMS_H
#define MPL3115A2_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    mpl3115a2 parameter configuration
 */
static const  mpl3115a2_params_t mpl3115a2_params[] =
{
    {
        .i2c = MPL3115A2_I2C,
        .addr = MPL3115A2_ADDR,
        .ratio = MPL3115A2_OS_RATIO_DEFAULT
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mpl3115a2_saul_info[] =
{
    {
        .name = "mpl3115a2",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* MPL3115A2_PARAMS_H */
/** @} */
