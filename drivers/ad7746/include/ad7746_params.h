/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ad7746
 * @{
 *
 * @file
 * @brief       Default configuration for AD7746 capaticance-to-digital
 *              converter
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */


#include "board.h"
#include "saul_reg.h"
#include "ad7746.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AD7746 driver
 * @{
 */
#ifndef AD7746_PARAM_I2C
#define AD7746_PARAM_I2C        (I2C_DEV(0))
#endif
#ifndef AD7746_PARAM_ADDR
#define AD7746_PARAM_ADDR       (0x48)
#endif
#ifndef AD7746_PARAM_CAP_SR
#define AD7746_PARAM_CAP_SR     (AD7746_CAP_SR_091)
#endif
#ifndef AD7746_PARAM_VT_SR
#define AD7746_PARAM_VT_SR      (AD7746_VT_SR_082)
#endif
#ifndef AD7746_PARAM_VT_MD
#define AD7746_PARAM_VT_MD      (AD7746_VT_MD_TEMP)
#endif
#ifndef AD7746_PARAM_DAC_A
#define AD7746_PARAM_DAC_A      (39) /* ~1.2pF */
#endif
#ifndef AD7746_PARAM_DAC_B
#define AD7746_PARAM_DAC_B      (0)
#endif
#ifndef AD7746_PARAM_EXC_CONFIG
#define AD7746_PARAM_EXC_CONFIG (AD7746_EXC_A)
#endif
#ifndef AD7746_PARAM_CAP_IN
#define AD7746_PARAM_CAP_IN     (AD7746_CAP_IN_1)
#endif

#ifndef AD7746_PARAMS
#define AD7746_PARAMS          { .i2c        = AD7746_PARAM_I2C,         \
                                 .addr       = AD7746_PARAM_ADDR,        \
                                 .dac_a_cap = AD7746_PARAM_DAC_A,        \
                                 .dac_b_cap = AD7746_PARAM_DAC_B,        \
                                 .exc_config = AD7746_PARAM_EXC_CONFIG,  \
                                 .cap_sample_rate = AD7746_PARAM_CAP_SR, \
                                 .vt_sample_rate = AD7746_PARAM_VT_SR,   \
                                 .vt_mode = AD7746_PARAM_VT_MD,          \
                                 .cap_input = AD7746_PARAM_CAP_IN }
#endif
#ifndef AD7746_SAUL_INFO
#define AD7746_SAUL_INFO       { .name = "ad7746" }
#endif
/** @} */

/**
 * @brief   AD7746 configuration
 */
static const ad7746_params_t ad7746_params[] =
{
    AD7746_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t ad7746_saul_info[] =
{
    AD7746_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
