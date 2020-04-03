/*
 * Copyright (C) 2016,2017,2018 Kees Bakker, SODAQ
 * Copyright (C) 2017 George Psimenos
 * Copyright (C) 2018 Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht2x
 *
 * @{
 * @file
 * @brief       Default configuration for SHT2X
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef SHT2X_PARAMS_H
#define SHT2X_PARAMS_H

#include "sht2x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the SHT2X
 * @{
 */
#ifndef SHT2X_PARAM_I2C_DEV
#define SHT2X_PARAM_I2C_DEV         (I2C_DEV(0))
#endif
#ifndef SHT2X_PARAM_I2C_ADDR
#define SHT2X_PARAM_I2C_ADDR        (0x40)
#endif
#ifndef SHT2X_PARAM_RESOLUTION
#define SHT2X_PARAM_RESOLUTION      (SHT2X_RES_12_14BIT)
#endif
#ifndef SHT2X_PARAM_MEASURE_MODE
#define SHT2X_PARAM_MEASURE_MODE    (SHT2X_MEASURE_MODE_HOLD)
#endif
#ifndef SHT2X_PARAM_CRC_MODE
#define SHT2X_PARAM_CRC_MODE        (1)
#endif

#define SHT2X_PARAMS_DEFAULT        {.i2c_dev = SHT2X_PARAM_I2C_DEV,  \
                                     .i2c_addr = SHT2X_PARAM_I2C_ADDR, \
                                     .resolution = SHT2X_PARAM_RESOLUTION, \
                                     .measure_mode = SHT2X_PARAM_MEASURE_MODE, \
                                     .is_crc_enabled = SHT2X_PARAM_CRC_MODE, \
                                    }

#ifndef SHT2X_SAUL_INFO
#define SHT2X_SAUL_INFO             { .name = "sht2x" }
#endif
/**@}*/

/**
 * @brief   Configure SHT2X
 */
static const sht2x_params_t sht2x_params[] =
{
#ifdef SHT2X_PARAMS_BOARD
    SHT2X_PARAMS_BOARD,
#else
    SHT2X_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Get the number of configured SHT2X devices
 */
#define SHT2X_NUMOF       ARRAY_SIZE(sht2x_params)

/**
 * @brief   Configuration details of SAUL registry entries
 *
 * This array contains static details of the sensors
 * for each device. Please be aware that the indexes are used in
 * auto_init_sht2x, so make sure the indexes match.
 */
static const saul_reg_info_t sht2x_saul_reg_info[SHT2X_NUMOF] =
{
        SHT2X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* SHT2X_PARAMS_H */
/** @} */
