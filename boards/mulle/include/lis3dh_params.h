/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_mulle
 * @{
 *
 * @file
 * @brief     LIS3DH board specific configuration
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef LIS3DH_PARAMS_H
#define LIS3DH_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LIS3DH configuration
 */
static const  lis3dh_params_t lis3dh_params[] =
{
    {
        .spi   = LIS3DH_SPI,
        .clk   = LIS3DH_CLK,
        .cs    = LIS3DH_CS,
        .int1  = LIS3DH_INT1,
        .int2  = LIS3DH_INT2,
        .scale = 4,
        .odr   = LIS3DH_ODR_100Hz,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lis3dh_saul_info[] =
{
    {
        .name = "lis3dh",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* LIS3DH_PARAMS_H */
/** @} */
