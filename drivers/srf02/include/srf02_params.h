/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   drivers_srf02
 * @{
 *
 * @file
 * @brief     Default configuration for SRF02 devices
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SRF02_PARAMS_H
#define SRF02_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default parameters for SRF02 devices
 * @{
 */
#ifndef SRF02_PARAM_I2C
#define SRF02_PARAM_I2C             (I2C_DEV(0))
#endif
#ifndef SRF02_PARAM_ADDR
#define SRF02_PARAM_ADDR            (SRF02_DEFAULT_ADDR)
#endif

#define SRF02_PARAMS_DEFAULT        {.i2c = SRF02_PARAM_I2C, \
                                     .addr = SRF02_PARAM_ADDR}
/** @} */

/**
 * @brief   SRF02 configuration
 */
static const srf02_params_t srf02_params[] =
{
#ifdef SRF02_PARAMS_BOARD
    SRF02_PARAMS_BOARD
#else
    SRF02_PARAMS_DEFAULT
#endif
};

/**
 * @brief   Get the number of configured SRF02 devices
 */
#define SRF02_NUMOF     (sizeof(srf02_params) / sizeof(srf02_params[0]))

#ifdef MODULE_SAUL_REG
/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
saul_reg_t srf02_saul_reg[] =
{
    {
        .name = "srf02",
        .driver = &srf02_saul_driver
    }
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* SRF02_PARAMS_H */
/** @} */
