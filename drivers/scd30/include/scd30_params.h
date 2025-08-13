/*
 * Copyright (C) 2020 Puhang Ding
 *               2020 Nishchay Agrawal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_sensors
 * @{
 * @file
 * @brief       Device driver params interface for the SCD30 sensor.
 *
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @author      Puhang Ding      <czarsir@gmail.com>
 * @}
 */

#include "periph/i2c.h"
#include "scd30.h"

#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SCD30 I2C address
 * @{
 */
#define SCD30_I2C_ADDR  0x61
/** @} */

/**
 * @name    Set default configuration parameters for the SCD30
 * @{
 */
#ifndef SCD30_PARAM_I2C_DEV
#define SCD30_PARAM_I2C_DEV          I2C_DEV(0)
#endif
#ifndef SCD30_PARAM_I2C_ADDR
#define SCD30_PARAM_I2C_ADDR         SCD30_I2C_ADDR
#endif

#ifndef SCD30_PARAMS
#define SCD30_PARAMS                { .i2c_dev = SCD30_PARAM_I2C_DEV,  \
                                      .i2c_addr = SCD30_PARAM_I2C_ADDR }
#endif

#ifndef SCD30_SAUL_INFO
#define SCD30_SAUL_INFO             { .name = "scd30" }
#endif
/**@}*/

/**
 * @brief   Configure SCD30
 */
static const scd30_params_t scd30_params[] =
{
    SCD30_PARAMS
};

/**
 * @brief   Get the number of configured SCD30 devices
 */
#define SCD30_NUMOF       ARRAY_SIZE(scd30_params)

/**
 * @brief   Configure SAUL registry entries
 */
static const saul_reg_info_t scd30_saul_info[] =
{
    SCD30_SAUL_INFO
};

#ifdef __cplusplus
}
#endif
