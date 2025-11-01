/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_max31865
 *
 * @{
 * @file
 * @brief       Default configuration for the MAX31865 driver
 *
 * @author      David Picard
 */

#include "board.h"
#include "saul_reg.h"

#include "max31865.h"
#include "max31865_internal.h"
#include "max31865_lut.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration for the MAX31865 driver
 * @{
 */

/**
 * @brief    Default SPI bus for the MAX31865 driver
 */
#ifndef MAX31865_PARAM_SPI
#  define MAX31865_PARAM_SPI        (SPI_DEV(0))
#endif

/**
 * @brief    Default CS pin for the MAX31865 driver
 */
#ifndef MAX31865_PARAM_CS_PIN
#  define MAX31865_PARAM_CS_PIN     (GPIO_PIN(0, 5))
#endif

/**
 * @brief Configuration byte definition
 */
#ifndef MAX31865_PARAM_CFG_BYTE
#  define MAX31865_PARAM_CFG_BYTE   MAX31865_CFG_VBIAS_ON | \
                                    MAX31865_CFG_CONV_AUTO | \
                                    MAX31865_CFG_3WIRE | \
                                    MAX31865_CFG_FILTER_50HZ
#endif

/**
 * @brief Low temperature threshold
 */
#ifndef MAX31865_PARAM_TEMP_THRES_LOW
#  define MAX31865_PARAM_TEMP_THRES_LOW     -19900
#endif

/**
 * @brief High temperature threshold
 */
#ifndef MAX31865_PARAM_TEMP_THRES_HIGH
#  define MAX31865_PARAM_TEMP_THRES_HIGH    64900
#endif

/**
 * @brief    Default parameters for the MAX31865 driver
 */
#ifndef MAX31865_PARAMS
#  define MAX31865_PARAMS { \
            .spi = MAX31865_PARAM_SPI, \
            .cs_pin = MAX31865_PARAM_CS_PIN, \
            .cfg_byte = MAX31865_PARAM_CFG_BYTE, \
            .temp_low_threshold = MAX31865_PARAM_TEMP_THRES_LOW, \
            .temp_high_threshold = MAX31865_PARAM_TEMP_THRES_HIGH, \
            .lut = &max31865_lut, \
            .lut_numlines = MAX31865_LUT_NUMLINES, \
}
#endif
/**
 * @brief    Default SAUL info for the MAX31865 driver
 */
#ifndef MAX31865_SAUL_INFO
#define MAX31865_SAUL_INFO { .name = "max31865_rtd" }
#endif
/**@}*/

/**
 * @brief   Configuration structs for the MAX31865 driver
 */
static const max31865_params_t max31865_params[] =
{
    MAX31865_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t max31865_saul_info[] =
{
    MAX31865_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
