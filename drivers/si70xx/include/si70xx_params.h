/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_si70xx
 *
 * @{
 * @file
 * @brief       Default configuration for Si7006/13/20/21
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SI70XX_PARAMS_H
#define SI70XX_PARAMS_H

#include "board.h"
#include "si70xx.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the Si7006/13/20/21 sensor
 * @{
 */
#ifndef SI70XX_PARAM_I2C_DEV
#define SI70XX_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef SI70XX_PARAM_ADDR
#define SI70XX_PARAM_ADDR            (0x80)
#endif

#define SI70XX_PARAMS_DEFAULT        { .i2c_dev = SI70XX_PARAM_I2C_DEV,  \
                                       .address = SI70XX_PARAM_ADDR }
/**@}*/

/**
 * @brief   Configure Si7006/13/20/21
 */
static const si70xx_params_t si70xx_params[] =
{
#ifdef SI70XX_PARAMS_CUSTOM
    SI70XX_PARAMS_CUSTOM,
#else
    SI70XX_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t si70xx_saul_reg_info[][2] =
{
    {
        { .name = "si70xx-temp" },
        { .name = "si70xx-hum" }
    }
};

#ifdef __cplusplus
}
#endif

#endif /* SI70XX_PARAMS_H */
/** @} */
