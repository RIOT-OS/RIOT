/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_lpsxxx
 *
 * @{
 * @file
 * @brief       Default configuration for LPSXXX family of devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"
#include "lpsxxx.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef LPSXXX_PARAM_I2C
#define LPSXXX_PARAM_I2C                I2C_DEV(0)
#endif
#ifndef LPSXXX_PARAM_ADDR
#define LPSXXX_PARAM_ADDR               (CONFIG_LPSXXX_DEFAULT_ADDRESS)
#endif
#ifndef LPSXXX_PARAM_RATE
#define LPSXXX_PARAM_RATE               (LPSXXX_DEFAULT_RATE)
#endif

#ifndef LPSXXX_PARAMS
#define LPSXXX_PARAMS                   { .i2c  = LPSXXX_PARAM_I2C,  \
                                          .addr = LPSXXX_PARAM_ADDR, \
                                          .rate = LPSXXX_PARAM_RATE }
#endif

#if MODULE_LPS331AP
#define LPSXXX_SAUL_NAME    "lps331ap"
#elif MODULE_LPS25HB
#define LPSXXX_SAUL_NAME    "lps25hb"
#elif MODULE_LPS22HB
#define LPSXXX_SAUL_NAME    "lps22hb"
#elif MODULE_LPS22HH
#define LPSXXX_SAUL_NAME    "lps22hh"
#elif MODULE_LPS22CH
#define LPSXXX_SAUL_NAME    "lps22ch"
#endif
#ifndef LPSXXX_SAUL_INFO
#define LPSXXX_SAUL_INFO                { .name = LPSXXX_SAUL_NAME }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const lpsxxx_params_t lpsxxx_params[] =
{
    LPSXXX_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t lpsxxx_saul_info[] =
{
    LPSXXX_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
