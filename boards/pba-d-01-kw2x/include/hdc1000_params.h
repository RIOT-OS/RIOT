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
 * @brief     hdc1000 board specific configuration
 *
 * @author    Johann Fischer <j.fischer@phytec.de>
 */

#ifndef HDC1000_PARAMS_H
#define HDC1000_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    hdc1000 parameter configuration
 */
static const  hdc1000_params_t hdc1000_params[] =
{
    {
        .i2c = HDC1000_I2C,
        .addr = HDC1000_ADDR,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hdc1000_saul_info[] =
{
    {
        .name = "hdc1000",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* HDC1000_PARAMS_H */
/** @} */
