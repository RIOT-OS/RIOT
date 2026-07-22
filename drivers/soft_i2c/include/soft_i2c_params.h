/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_soft_i2c
 * @{
 *
 * @file
 * @brief       Software I2C configuration
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef SOFT_I2C_PARAMS_H
#define SOFT_I2C_PARAMS_H

#include "board.h"
#include "kernel_defines.h"
#include "macros/units.h"
#include "periph/gpio.h"
#include "soft_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOFT_I2C_PARAM_SCL
#define SOFT_I2C_PARAM_SCL                  GPIO_UNDEF
#endif
#ifndef SOFT_I2C_PARAM_SDA
#define SOFT_I2C_PARAM_SDA                  GPIO_UNDEF
#endif
#ifndef SOFT_I2C_PARAM_SPEED
#define SOFT_I2C_PARAM_SPEED                I2C_SPEED_NORMAL
#endif
#ifndef SOFT_I2C_PARAM_INTERNAL_PULL_UPS
#define SOFT_I2C_PARAM_INTERNAL_PULL_UPS    true
#endif

#ifndef SOFT_I2C_PARAMS
#define SOFT_I2C_PARAMS \
{ \
    .scl_pin                    = SOFT_I2C_PARAM_SCL, \
    .sda_pin                    = SOFT_I2C_PARAM_SDA, \
    .speed                      = SOFT_I2C_PARAM_SPEED, \
    .enable_internal_pull_ups   = SOFT_I2C_PARAM_INTERNAL_PULL_UPS, \
}
#endif

/**
 * @brief   Sotware I2C port descriptor array
 */
static const soft_i2c_conf_t soft_i2c_config[] = {
    SOFT_I2C_PARAMS,
};

#define SOFT_I2C_NUMOF             ARRAY_SIZE(soft_i2c_config)

#ifdef __cplusplus
}
#endif

#endif /* SOFT_I2C_PARAMS_H */
/** @} */
