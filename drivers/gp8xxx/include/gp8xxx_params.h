/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_gp8xxx
 *
 * @{
 * @file
 * @brief       Default configuration for the Guestgood GP8xxx I2C DACs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"

#include "saul_reg.h"

#include "gp8xxx.h"
#include "gp8xxx_constants.h"
#include "gp8xxx_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifdef MODULE_GP8503
#  ifndef GP8503_PARAM_I2C_DEV
#    define GP8503_PARAM_I2C_DEV    I2C_DEV(0)
#  endif
#  ifndef GP8503_PARAM_ADDR
#    define GP8503_PARAM_ADDR       (0x5f)
#  endif
#  ifndef GP8503_PARAM_RANGE
#    define GP8503_PARAM_RANGE      GP8XXX_OUTPUT_RANGE_2_5V
#  endif

#  ifndef GP8503_PARAMS
#    define GP8503_PARAMS           { .i2c_dev = GP8503_PARAM_I2C_DEV, \
                                      .address = GP8503_PARAM_ADDR, \
                                      .range = GP8503_PARAM_RANGE, \
                                      .info = &gp8503_info }
#  endif

#  ifndef GP8503_SAUL_PARAMS
#    define GP8503_SAUL_PARAMS      { .name = "gp8503-0" }, \
                                    { .name = "gp8503-1" }
#  endif
#endif /* MODULE_GP8503 */

#ifdef MODULE_GP8512
#  ifndef GP8512_PARAM_I2C_DEV
#    define GP8512_PARAM_I2C_DEV    I2C_DEV(0)
#  endif
#  ifndef GP8512_PARAM_ADDR
#    define GP8512_PARAM_ADDR       (0x5f)
#  endif
#  ifndef GP8512_PARAM_RANGE
#    define GP8512_PARAM_RANGE      GP8XXX_OUTPUT_RANGE_2_5V
#  endif

#  ifndef GP8512_PARAMS
#    define GP8512_PARAMS           { .i2c_dev = GP8512_PARAM_I2C_DEV, \
                                      .address = GP8512_PARAM_ADDR, \
                                      .range = GP8512_PARAM_RANGE, \
                                      .info = &gp8512_info }
#  endif

#  ifndef GP8512_SAUL_PARAMS
#    define GP8512_SAUL_PARAMS      { .name = "gp8512-0" }
#  endif
#endif /* MODULE_GP8512 */

#ifdef MODULE_GP8403
#  ifndef GP8403_PARAM_I2C_DEV
#    define GP8403_PARAM_I2C_DEV    I2C_DEV(0)
#  endif
#  ifndef GP8403_PARAM_ADDR
#    define GP8403_PARAM_ADDR       (0x5f)
#  endif
#  ifndef GP8403_PARAM_RANGE
#    define GP8403_PARAM_RANGE      GP8XXX_OUTPUT_RANGE_10V
#  endif

#  ifndef GP8403_PARAMS
#    define GP8403_PARAMS           { .i2c_dev = GP8403_PARAM_I2C_DEV, \
                                      .address = GP8403_PARAM_ADDR, \
                                      .range = GP8403_PARAM_RANGE, \
                                      .info = &gp8403_info }
#  endif

#  ifndef GP8403_SAUL_PARAMS
#    define GP8403_SAUL_PARAMS      { .name = "gp8403-0" }, \
                                    { .name = "gp8403-1" }
#  endif
#endif /* MODULE_GP8403 */

#ifdef MODULE_GP8413
#  ifndef GP8413_PARAM_I2C_DEV
#    define GP8413_PARAM_I2C_DEV    I2C_DEV(0)
#  endif
#  ifndef GP8413_PARAM_ADDR
#    define GP8413_PARAM_ADDR       (0x5f)
#  endif
#  ifndef GP8413_PARAM_RANGE
#    define GP8413_PARAM_RANGE      GP8XXX_OUTPUT_RANGE_10V
#  endif

#  ifndef GP8413_PARAMS
#    define GP8413_PARAMS           { .i2c_dev = GP8413_PARAM_I2C_DEV, \
                                      .address = GP8413_PARAM_ADDR, \
                                      .range = GP8413_PARAM_RANGE, \
                                      .info = &gp8413_info }
#  endif

#  ifndef GP8413_SAUL_PARAMS
#    define GP8413_SAUL_PARAMS      { .name = "gp8413-0" }, \
                                    { .name = "gp8413-1" }
#  endif
#endif /* MODULE_GP8413 */

#ifdef MODULE_GP8211S
#  ifndef GP8211S_PARAM_I2C_DEV
#    define GP8211S_PARAM_I2C_DEV   I2C_DEV(0)
#  endif
#  ifndef GP8211S_PARAM_ADDR
#    define GP8211S_PARAM_ADDR      (0x5f)
#  endif
#  ifndef GP8211S_PARAM_RANGE
#    define GP8211S_PARAM_RANGE     GP8XXX_OUTPUT_RANGE_10V
#  endif

#  ifndef GP8211S_PARAMS
#    define GP8211S_PARAMS          { .i2c_dev = GP8211S_PARAM_I2C_DEV, \
                                      .address = GP8211S_PARAM_ADDR, \
                                      .range = GP8211S_PARAM_RANGE, \
                                      .info = &gp8211s_info }
#  endif

#  ifndef GP8211S_SAUL_PARAMS
#    define GP8211S_SAUL_PARAMS     { .name = "gp8211s-0" }
#  endif
#endif /* MODULE_GP8211S */

#ifdef MODULE_GP8302
#  ifndef GP8302_PARAM_I2C_DEV
#    define GP8302_PARAM_I2C_DEV    I2C_DEV(0)
#  endif
#  ifndef GP8302_PARAM_ADDR
#    define GP8302_PARAM_ADDR       (0x58)
#  endif

#  ifndef GP8302_PARAMS
#    define GP8302_PARAMS           { .i2c_dev = GP8302_PARAM_I2C_DEV, \
                                      .address = GP8302_PARAM_ADDR, \
                                      .info = &gp8302_info }
#  endif

#  ifndef GP8302_SAUL_PARAMS
#    define GP8302_SAUL_PARAMS      { .name = "gp8302-0" }
#  endif
#endif /* MODULE_GP8302 */
/** @}*/

/**
 * @brief   Configuration struct
 */
static const gp8xxx_params_t gp8xxx_params[] =
{
#if MODULE_GP8503
    GP8503_PARAMS,
#endif
#if MODULE_GP8512
    GP8512_PARAMS,
#endif
#if MODULE_GP8211S
    GP8211S_PARAMS,
#endif
#if MODULE_GP8403
    GP8403_PARAMS,
#endif
#if MODULE_GP8413
    GP8413_PARAMS,
#endif
#if MODULE_GP8302
    GP8302_PARAMS,
#endif
};

/**
 * @brief   Configure SAUL registry entries
 *
 * This array must match up with the device configuration and its channels, as
 * defined in @ref gp8xxx_params.
 */
static const saul_reg_info_t gp8xxx_saul_info[] =
{
#if MODULE_GP8503
    GP8503_SAUL_PARAMS,
#endif
#if MODULE_GP8512
    GP8512_SAUL_PARAMS,
#endif
#if MODULE_GP8211S
    GP8211S_SAUL_PARAMS,
#endif
#if MODULE_GP8403
    GP8403_SAUL_PARAMS,
#endif
#if MODULE_GP8413
    GP8413_SAUL_PARAMS,
#endif
#if MODULE_GP8302
    GP8302_SAUL_PARAMS,
#endif
};

#ifdef __cplusplus
}
#endif

/** @} */
