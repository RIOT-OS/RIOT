/*
 * Copyright (C) 2016,2017,2018 Kees Bakker, SODAQ
 * Copyright (C) 2017 George Psimenos
 * Copyright (C) 2018 Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_sht2x
 *
 * @{
 * @file
 * @brief       Default configuration for SHT2x humidity and temperature sensor
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#include "sht2x.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !DOXYGEN
/* Mapping of Kconfig defines to the respective driver enumeration values */

#if CONFIG_SHT2X_RES_12_14BIT
#define CONFIG_SHT2X_RESOLUTION     (SHT2X_RES_12_14BIT)
#elif CONFIG_SHT2X_RES_11_11BIT
#define CONFIG_SHT2X_RESOLUTION     (SHT2X_RES_11_11BIT)
#elif CONFIG_SHT2X_RES_10_13BIT
#define CONFIG_SHT2X_RESOLUTION     (SHT2X_RES_10_13BIT)
#elif CONFIG_SHT2X_RES_8_12BIT
#define CONFIG_SHT2X_RESOLUTION     (SHT2X_RES_8_12BIT)
#else
#define CONFIG_SHT2X_RESOLUTION     (SHT2X_RES_12_14BIT)
#endif

#if CONFIG_SHT2X_MEASURE_MODE_HOLD
#define CONFIG_SHT2X_MEASURE_MODE   (SHT2X_MEASURE_MODE_HOLD)
#elif CONFIG_SHT2X_MEASURE_MODE_NO_HOLD
#define CONFIG_SHT2X_MEASURE_MODE   (SHT2X_MEASURE_MODE_NO_HOLD)
#else
#define CONFIG_SHT2X_MEASURE_MODE   (SHT2X_MEASURE_MODE_NO_HOLD)
#endif

#ifndef CONFIG_SHT2X_CRC_MODE
#define CONFIG_SHT2X_CRC_MODE       (1)
#endif

#endif /* !DOXYGEN */

/**
 * @name   Default SHT2x hardware configuration
 * @{
 */
#ifndef SHT2X_PARAM_I2C_DEV
/** I2C device used */
#define SHT2X_PARAM_I2C_DEV         (I2C_DEV(0))
#endif

#ifndef SHT2X_PARAM_I2C_ADDR
/** I2C slave slave of the SHT2x sensor */
#define SHT2X_PARAM_I2C_ADDR        (0x40)
#endif
/** @} */

/**
 * @name   Default sensor configuration for the SHT2x sensor
 * @{
 */
#ifndef SHT2X_PARAM_RESOLUTION
/** SHT2x resolution */
#define SHT2X_PARAM_RESOLUTION      (CONFIG_SHT2X_RESOLUTION)
#endif

#ifndef SHT2X_PARAM_MEASURE_MODE
/** SHT2x measurement mode */
#define SHT2X_PARAM_MEASURE_MODE    (CONFIG_SHT2X_MEASURE_MODE)
#endif

#ifndef SHT2X_PARAM_CRC_MODE
/** SHT2x CRC mode */
#define SHT2X_PARAM_CRC_MODE        (CONFIG_SHT2X_CRC_MODE)
#endif

/** Default SHT2x parameter set */
#define SHT2X_PARAMS_DEFAULT        {.i2c_dev = SHT2X_PARAM_I2C_DEV,  \
                                     .i2c_addr = SHT2X_PARAM_I2C_ADDR, \
                                     .resolution = SHT2X_PARAM_RESOLUTION, \
                                     .measure_mode = SHT2X_PARAM_MEASURE_MODE, \
                                     .is_crc_enabled = SHT2X_PARAM_CRC_MODE, \
                                    }

#ifndef SHT2X_SAUL_INFO
/** Default SAUL device info */
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

/** @} */
