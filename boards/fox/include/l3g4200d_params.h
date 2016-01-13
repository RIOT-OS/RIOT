/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_fox
 *
 * @file
 * @brief     L3G4200D board specific configuration
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef L3G4200D_PARAMS_H
#define L3G4200D_PARAMS_H

#include "board.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    L3G4200D configuration
 */
static const  l3g4200d_params_t l3g4200d_params[] =
{
    {
        .i2c = L3G4200D_I2C,
        .addr = L3G4200D_ADDR,
        .int1_pin = L3G4200D_INT,
        .int2_pin = L3G4200D_DRDY,
        .mode = L3G4200D_MODE_200_25,
        .scale = L3G4200D_SCALE_500DPS,
    },
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t l3g4200d_saul_info[] =
{
    {
        .name = "l3g4200d",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* L3G4200D_PARAMS_H */
/** @} */
