/*
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_qma6100p
 * @{
 *
 * @file
 * @brief       Default configuration for the QST QMA6100P accelerometer
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "saul_reg.h"
#include "qma6100p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the QMA6100P driver
 * @{
 */

/**
 * @name    QMA6100P I2C addresses
 * Determined by the AD0 pin level.
 * @{
 */
#define QMA6100P_I2C_ADDR_LOW       (0x12) /**< AD0 connected to GND */
#define QMA6100P_I2C_ADDR_HIGH      (0x13) /**< AD0 connected to VDD */
/** @} */

/**
 * @def QMA6100P_PARAM_I2C
 * @brief Default I2C bus
 */
#ifndef QMA6100P_PARAM_I2C
#  define QMA6100P_PARAM_I2C        (I2C_DEV(0))
#endif


/**
 * @def QMA6100P_PARAM_ADDR
 * @brief Default I2C address: must be defined by the board depending on AD0 pin wiring.
 *        Use @ref QMA6100P_I2C_ADDR_LOW (0x12) when AD0 is connected to GND,
 *        or @ref QMA6100P_I2C_ADDR_HIGH (0x13) when AD0 is connected to VDD.
 */
#ifndef QMA6100P_PARAM_ADDR
#  define QMA6100P_PARAM_ADDR       (GPIO_UNDEF)
#endif

/**
 * @def QMA6100P_PARAM_RATE
 * @brief Default output data rate
 */
#ifndef QMA6100P_PARAM_RATE
#  define QMA6100P_PARAM_RATE       (QMA6100P_ODR_100HZ)
#endif

/**
 * @def QMA6100P_PARAM_RANGE
 * @brief Default full-scale range
 */
#ifndef QMA6100P_PARAM_RANGE
#  define QMA6100P_PARAM_RANGE      (QMA6100P_RANGE_2G)
#endif

/**
 * @def QMA6100P_PARAM_MCLK
 * @brief Default master clock frequency selection.
 */
#ifndef QMA6100P_PARAM_MCLK
#  define QMA6100P_PARAM_MCLK       (QMA6100P_PM_MCLK_51K2)
#endif

/**
 * @def QMA6100P_PARAM_INT_PIN
 * @brief Default interrupt pin. Set to GPIO_UNDEF to disable interrupt-driven
 *        operation and use polling instead.
 */
#ifndef QMA6100P_PARAM_INT_PIN
#  define QMA6100P_PARAM_INT_PIN    (GPIO_UNDEF)
#endif

/**
 * @def QMA6100P_PARAM_OFFSET
 * @brief Default per-axis calibration offset (X, Y, Z) in raw LSB units
 */
#ifndef QMA6100P_PARAM_OFFSET
#  define QMA6100P_PARAM_OFFSET     { 0, 0, 0 }
#endif

/**
 * @def QMA6100P_PARAMS
 * @brief Default configuration parameters structure for QMA6100P devices
 */
#ifndef QMA6100P_PARAMS
#  define QMA6100P_PARAMS           { .i2c     = QMA6100P_PARAM_I2C,     \
                                      .addr    = QMA6100P_PARAM_ADDR,    \
                                      .rate    = QMA6100P_PARAM_RATE,    \
                                      .range   = QMA6100P_PARAM_RANGE,   \
                                      .mclk    = QMA6100P_PARAM_MCLK,    \
                                      .int_pin = QMA6100P_PARAM_INT_PIN, \
                                      .offset  = QMA6100P_PARAM_OFFSET }
#endif

/**
 * @def QMA6100P_SAUL_INFO
 * @brief Additional SAUL registry information
 */
#ifndef QMA6100P_SAUL_INFO
#  define QMA6100P_SAUL_INFO        { .name = "qma6100p" }
#endif
/** @} */

/**
 * @brief   QMA6100P defaults if not defined for a board or application
 */
static const qma6100p_params_t qma6100p_params[] =
{
    QMA6100P_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t qma6100p_saul_info[] =
{
    QMA6100P_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
