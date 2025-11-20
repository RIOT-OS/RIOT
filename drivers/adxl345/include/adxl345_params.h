/*
 * Copyright (C) 2017 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_adxl345
 * @{
 *
 * @file
 * @brief       Default configuration for ADXL345 devices
 *
 * @author     Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "board.h"
#include "saul_reg.h"
#include "adxl345.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ADXL345 driver
 * @{
 */
#ifndef ADXL345_PARAM_I2C
#define ADXL345_PARAM_I2C           (I2C_DEV(0))
#endif
#ifndef ADXL345_PARAM_ADDR
#define ADXL345_PARAM_ADDR          (ADXL345_ADDR_53)
#endif
#ifndef ADXL345_PARAM_RATE
#define ADXL345_PARAM_RATE          (ADXL345_RATE_200HZ)
#endif
#ifndef ADXL345_PARAM_RANGE
#define ADXL345_PARAM_RANGE         (ADXL345_RANGE_16G)
#endif
#ifndef ADXL345_PARAM_INTERRUPT
#define ADXL345_PARAM_INTERRUPT     {0x0F, 0xBF, 0x40, 0xF0, 0xFF, 0x00, 0x00, \
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F}
#endif
#ifndef ADXL345_PARAM_FULL_RES
#define ADXL345_PARAM_FULL_RES      (1)
#endif
#ifndef ADXL345_PARAM_OFFSET
#define ADXL345_PARAM_OFFSET        { 0, 0, 0 }
#endif

#ifndef ADXL345_PARAMS
#define ADXL345_PARAMS              { .i2c    = ADXL345_PARAM_I2C,       \
                                      .addr   = ADXL345_PARAM_ADDR,      \
                                      .offset = ADXL345_PARAM_OFFSET,    \
                                      .range  = ADXL345_PARAM_RANGE,     \
                                      .rate   = ADXL345_PARAM_RATE,      \
                                      .full_res = ADXL345_PARAM_FULL_RES }
#endif
#ifndef ADXL345_SAUL_INFO
#define ADXL345_SAUL_INFO           { .name = "adxl345" }
#endif
/**@}*/

/**
 * @brief   ADXL345 configuration
 */
static const adxl345_params_t adxl345_params[] =
{
    ADXL345_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t adxl345_saul_info[] =
{
    ADXL345_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
