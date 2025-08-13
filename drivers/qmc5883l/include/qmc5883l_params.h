/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_qmc5883l
 *
 * @{
 * @file
 * @brief       Default configuration for QMC5883L devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "saul_reg.h"
#include "board.h"
#include "qmc5883l.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for QMC5883L devices
 * @{
 */
#ifndef QMC5883L_PARAM_I2C
#define QMC5883L_PARAM_I2C          I2C_DEV(0)
#endif
#ifndef QMC5883L_PARAM_PIN_DRDY
#define QMC5883L_PARAM_PIN_DRDY     (GPIO_UNDEF)
#endif
#ifndef QMC5883L_PARAM_ODR
#define QMC5883L_PARAM_ODR          (QMC5883L_ODR_10HZ)
#endif
#ifndef QMC5883L_PARAM_RNG
#define QMC5883L_PARAM_RNG          (QMC5883L_RNG_2G)
#endif
#ifndef QMC5883L_PARAM_OSR
#define QMC5883L_PARAM_OSR          (QMC5883L_OSR_64)
#endif

#ifndef QMC5883L_PARAMS
#define QMC5883L_PARAMS             { .i2c      = QMC5883L_PARAM_I2C,      \
                                      .pin_drdy = QMC5883L_PARAM_PIN_DRDY, \
                                      .odr      = QMC5883L_PARAM_ODR,      \
                                      .rng      = QMC5883L_PARAM_RNG,      \
                                      .osr      = QMC5883L_PARAM_OSR }
#endif

#ifndef QMC5883L_SAUL_INFO
#define QMC5883L_SAUL_INFO          { .name = "qmc5883l" }
#endif
/**@}*/

/**
 * @brief   QMC5883L configuration
 */
static const qmc5883l_params_t qmc5883l_params[] =
{
    QMC5883L_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t qmc5883l_saul_info[] =
{
    QMC5883L_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
