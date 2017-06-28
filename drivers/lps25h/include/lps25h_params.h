/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lps25h
 *
 * @{
 * @file
 * @brief       Default configuration for LPS25H devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LPS25H_PARAMS_H
#define LPS25H_PARAMS_H

#include "board.h"
#include "lps25h.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the LPS25H driver
 * @{
 */
#ifndef LPS25H_PARAM_I2C
#define LPS25H_PARAM_I2C           I2C_DEV(0)
#endif
#ifndef LPS25H_PARAM_ADDR
#define LPS25H_PARAM_ADDR          (LPS25H_CHIP_ADDR)
#endif
#ifndef LPS25H_PARAM_ODR
#define LPS25H_PARAM_ODR           (LPS25H_ODR_7HZ)
#endif

#define LPS25H_PARAMS_DEFAULT    { .i2c  = LPS25H_PARAM_I2C, \
                                   .addr = LPS25H_PARAM_ADDR, \
                                   .odr = LPS25H_PARAM_ODR }
/**@}*/

/**
 * @brief   LPS25H configuration
 */
static const lps25h_params_t lps25h_params[] =
{
#ifdef LPS25H_PARAMS_BOARD
    LPS25H_PARAMS_BOARD,
#else
    LPS25H_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lps25h_saul_reg_info[] =
{
    { .name = "lps25h" }
};

#ifdef __cplusplus
}
#endif

#endif /* LPS25H_PARAMS_H */
/** @} */
