/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief       MAG3110 board specific configuration
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
 * @brief    MAG3310 configuration
 */
static const  mag3110_params_t mag3110_params[] =
{
    {
        .i2c    = MAG3110_I2C,
        .addr   = MAG3110_ADDR,
        .type   = MAG3110_ID,
        .dros   = MAG3110_DROS_DEFAULT,
        .offset = { 0, 0, 0 }
    }
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t mag3110_saul_info[] =
{
    {
        .name = "mag3110"
    }
};

#ifdef __cplusplus
}
#endif

#endif /* MAG3110_PARAMS_H */
/** @} */
