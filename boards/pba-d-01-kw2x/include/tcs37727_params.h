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
 * @brief     tcs37727 board specific configuration
 *
 * @author    Johann Fischer <j.fischer@phytec.de>
 */

#ifndef TCS37727_PARAMS_H
#define TCS37727_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    tcs37727 parameter configuration
 */
static const  tcs37727_params_t tcs37727_params[] =
{
    {
        .i2c = TCS37727_I2C,
        .addr = TCS37727_ADDR,
        .atime = TCS37727_ATIME_DEFAULT,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t tcs37727_saul_info[] =
{
    {
        .name = "tcs37727",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* TCS37727_PARAMS_H */
/** @} */
