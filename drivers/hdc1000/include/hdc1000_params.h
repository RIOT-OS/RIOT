/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hdc1000
 *
 * @{
 * @file
 * @brief       Default configuration for HDC1000 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef HDC1000_PARAMS_H
#define HDC1000_PARAMS_H

#include "board.h"
#include "hdc1000.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the HDC1000 driver
 * @{
 */
#ifndef HDC1000_PARAM_I2C
#define HDC1000_PARAM_I2C           I2C_DEV(0)
#endif
#ifndef HDC1000_PARAM_ADDR
#define HDC1000_PARAM_ADDR          (HDC1000_I2C_ADDRESS)
#endif
#ifndef HDC1000_PARAM_RES
#define HDC1000_PARAM_RES           HDC1000_14BIT
#endif

#define HDC1000_PARAMS_DEFAULT    { .i2c  = HDC1000_PARAM_I2C, \
                                    .addr = HDC1000_PARAM_ADDR, \
                                    .res  = HDC1000_PARAM_RES }
/**@}*/

/**
 * @brief   HDC1000 configuration
 */
static const hdc1000_params_t hdc1000_params[] =
{
#ifdef HDC1000_PARAMS_BOARD
    HDC1000_PARAMS_BOARD,
#else
    HDC1000_PARAMS_DEFAULT,
#endif
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
