/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-FileCopyrightText: 2026 Léandre Le Duc <leandre.leduc38@gmail.com>
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
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 */

#include "board.h"
#include "periph/i2c.h"
#include "qma6100p.h"
#include "saul_reg.h"

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
#define QMA6100P_I2C_ADDR_LOW        (0x12) /**< AD0 connected to GND */
#define QMA6100P_I2C_ADDR_HIGH       (0x13) /**< AD0 connected to VDD */
/** @} */

/**
* @def QMA6100P_PARAM_INT_PIN_UNDEF
* @brief   INT pin default: should be defined by the board or the app.
*/
#define QMA6100P_PARAM_INT_PIN_UNDEF (GPIO_UNDEF)

/**
 * @def QMA6100P_PARAM_I2C
 * @brief Default I2C bus
 */
#ifndef QMA6100P_PARAM_I2C
#  define QMA6100P_PARAM_I2C (I2C_DEV(0))
#endif

/**
 * @def CONFIG_QMA6100P_I2C_ADDR
 * @brief Default I2C address (fallback when not configured via Kconfig)
 */
#ifndef CONFIG_QMA6100P_I2C_ADDR
#  define CONFIG_QMA6100P_I2C_ADDR (QMA6100P_I2C_ADDR_LOW)
#endif

/**
 * @def QMA6100P_PARAM_I2C_ADDR
 * @brief Default I2C address
 */
#ifndef QMA6100P_PARAM_I2C_ADDR
#  define QMA6100P_PARAM_I2C_ADDR (CONFIG_QMA6100P_I2C_ADDR)
#endif

/**
 * @def QMA6100P_PARAM_RATE
 * @brief Default output data rate
 */
#if IS_ACTIVE(CONFIG_QMA6100P_ODR_100HZ)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_100HZ)
#elif IS_ACTIVE(CONFIG_QMA6100P_ODR_200HZ)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_200HZ)
#elif IS_ACTIVE(CONFIG_QMA6100P_ODR_400HZ)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_400HZ)
#elif IS_ACTIVE(CONFIG_QMA6100P_ODR_800HZ)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_800HZ)
#elif IS_ACTIVE(CONFIG_QMA6100P_ODR_1600HZ)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_1600HZ)
#elif IS_ACTIVE(CONFIG_QMA6100P_ODR_50HZ)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_50HZ)
#elif IS_ACTIVE(CONFIG_QMA6100P_ODR_25HZ)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_25HZ)
#elif IS_ACTIVE(CONFIG_QMA6100P_ODR_12HZ5)
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_12HZ5)
#endif
#ifndef QMA6100P_PARAM_RATE
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_100HZ)
#endif

/**
 * @def QMA6100P_PARAM_RANGE
 * @brief Default full-scale range
 */
#if IS_ACTIVE(CONFIG_QMA6100P_RANGE_2G)
#  define QMA6100P_PARAM_RANGE (QMA6100P_RANGE_2G)
#elif IS_ACTIVE(CONFIG_QMA6100P_RANGE_4G)
#  define QMA6100P_PARAM_RANGE (QMA6100P_RANGE_4G)
#elif IS_ACTIVE(CONFIG_QMA6100P_RANGE_8G)
#  define QMA6100P_PARAM_RANGE (QMA6100P_RANGE_8G)
#elif IS_ACTIVE(CONFIG_QMA6100P_RANGE_16G)
#  define QMA6100P_PARAM_RANGE (QMA6100P_RANGE_16G)
#elif IS_ACTIVE(CONFIG_QMA6100P_RANGE_32G)
#  define QMA6100P_PARAM_RANGE (QMA6100P_RANGE_32G)
#endif
#ifndef QMA6100P_PARAM_RANGE
#  define QMA6100P_PARAM_RANGE (QMA6100P_RANGE_2G)
#endif

/**
 * @def QMA6100P_PARAM_MCLK
 * @brief Default master clock frequency
 */
#if IS_ACTIVE(CONFIG_QMA6100P_PM_MCLK_51K2)
#  define QMA6100P_PARAM_MCLK (QMA6100P_PM_MCLK_51K2)
#elif IS_ACTIVE(CONFIG_QMA6100P_PM_MCLK_25K6)
#  define QMA6100P_PARAM_MCLK (QMA6100P_PM_MCLK_25K6)
#elif IS_ACTIVE(CONFIG_QMA6100P_PM_MCLK_12K8)
#  define QMA6100P_PARAM_MCLK (QMA6100P_PM_MCLK_12K8)
#elif IS_ACTIVE(CONFIG_QMA6100P_PM_MCLK_6K4)
#  define QMA6100P_PARAM_MCLK (QMA6100P_PM_MCLK_6K4)
#endif
#ifndef QMA6100P_PARAM_MCLK
#  define QMA6100P_PARAM_MCLK (QMA6100P_PM_MCLK_51K2)
#endif

/**
 * @def QMA6100P_PARAM_MODE
 * @brief Default operating mode
 */
#if IS_ACTIVE(CONFIG_QMA6100P_MODE_ACTIVE)
#  define QMA6100P_PARAM_MODE (QMA6100P_MODE_ACTIVE)
#elif IS_ACTIVE(CONFIG_QMA6100P_MODE_ULPS)
#  define QMA6100P_PARAM_MODE (QMA6100P_MODE_ULPS)
#endif
#ifndef QMA6100P_PARAM_MODE
#  define QMA6100P_PARAM_MODE (QMA6100P_MODE_ACTIVE)
#endif

/**
 * @def QMA6100P_PARAM_INT_PIN
 * @brief Default interrupt pin. Set to GPIO_UNDEF to disable interrupt-driven
 *        operation and use polling instead.
 */
#ifndef QMA6100P_PARAM_INT_PIN
#  define QMA6100P_PARAM_INT_PIN (QMA6100P_PARAM_INT_PIN_UNDEF)
#endif
/**
 * @def QMA6100P_PARAM_INT_ACTIVE_LEVEL
 * @brief Default interrupt pin active level
 */
#if IS_ACTIVE(CONFIG_QMA6100P_INTPIN_ACTIVE_HIGH)
#  define QMA6100P_PARAM_INT_ACTIVE_LEVEL (QMA6100P_INTPIN_ACTIVE_HIGH)
#elif IS_ACTIVE(CONFIG_QMA6100P_INTPIN_ACTIVE_LOW)
#  define QMA6100P_PARAM_INT_ACTIVE_LEVEL (QMA6100P_INTPIN_ACTIVE_LOW)
#endif
#ifndef QMA6100P_PARAM_INT_ACTIVE_LEVEL
#  define QMA6100P_PARAM_INT_ACTIVE_LEVEL (QMA6100P_INTPIN_ACTIVE_HIGH)
#endif

/**
 * @def QMA6100P_PARAM_INT_PIN_MODE
 * @brief Default interrupt pin output mode (push-pull or open-drain)
 */
#if IS_ACTIVE(CONFIG_QMA6100P_INTPIN_PUSH_PULL)
#  define QMA6100P_PARAM_INT_PIN_MODE (QMA6100P_INTPIN_PUSH_PULL)
#elif IS_ACTIVE(CONFIG_QMA6100P_INTPIN_OPEN_DRAIN)
#  define QMA6100P_PARAM_INT_PIN_MODE (QMA6100P_INTPIN_OPEN_DRAIN)
#endif
#ifndef QMA6100P_PARAM_INT_PIN_MODE
#  define QMA6100P_PARAM_INT_PIN_MODE (QMA6100P_INTPIN_PUSH_PULL)
#endif

/**
 * @def QMA6100P_PARAM_INT_SHADOW
 * @brief Default shadow mode for acceleration data registers
 */
#if IS_ACTIVE(CONFIG_QMA6100P_INT_CFG_SHADOW_DIS)
#  define QMA6100P_PARAM_INT_SHADOW (QMA6100P_INT_CFG_SHADOW_DIS)
#elif IS_ACTIVE(CONFIG_QMA6100P_INT_CFG_SHADOW_EN)
#  define QMA6100P_PARAM_INT_SHADOW (QMA6100P_INT_CFG_SHADOW_EN)
#endif
#ifndef QMA6100P_PARAM_INT_SHADOW
#  define QMA6100P_PARAM_INT_SHADOW (QMA6100P_INT_CFG_SHADOW_DIS)
#endif

/**
 * @def QMA6100P_PARAM_INT_PIN_NUM
 * @brief QMA6100P interrupt output pin routed to the MCU
 */
#if IS_ACTIVE(CONFIG_QMA6100P_INT1)
#  define QMA6100P_PARAM_INT_PIN_NUM (QMA6100P_INT1)
#elif IS_ACTIVE(CONFIG_QMA6100P_INT2)
#  define QMA6100P_PARAM_INT_PIN_NUM (QMA6100P_INT2)
#endif
#ifndef QMA6100P_PARAM_INT_PIN_NUM
#  define QMA6100P_PARAM_INT_PIN_NUM (QMA6100P_INT1)
#endif

/**
 * @def QMA6100P_PARAMS
 * @brief Default configuration parameters structure for QMA6100P devices
 */
#ifndef QMA6100P_PARAMS
#  define QMA6100P_PARAMS { .i2c = QMA6100P_PARAM_I2C,       \
                            .addr = QMA6100P_PARAM_I2C_ADDR, \
                            .rate = QMA6100P_PARAM_RATE,     \
                            .range = QMA6100P_PARAM_RANGE,   \
                            .mclk = QMA6100P_PARAM_MCLK,     \
                            .mode = QMA6100P_PARAM_MODE }
#endif

/**
 * @def QMA6100P_INT_PARAMS
 * @brief Default interrupt configuration parameters structure for QMA6100P devices
 */
#ifndef QMA6100P_INT_PARAMS
#  define QMA6100P_INT_PARAMS { .interrupt_pin = QMA6100P_PARAM_INT_PIN,             \
                                .active_level_int = QMA6100P_PARAM_INT_ACTIVE_LEVEL, \
                                .pin_mode_int = QMA6100P_PARAM_INT_PIN_MODE,         \
                                .interrupt_shadow = QMA6100P_PARAM_INT_SHADOW,       \
                                .interrupt_pin_num = QMA6100P_PARAM_INT_PIN_NUM }
#endif

/**
 * @def QMA6100P_SAUL_INFO
 * @brief Additional SAUL registry information
 */
#ifndef QMA6100P_SAUL_INFO
#  define QMA6100P_SAUL_INFO { .name = "qma6100p" }
#endif
/** @} */

/**
 * @brief   QMA6100P defaults params if not defined for a board or application
 */
static const qma6100p_params_t qma6100p_params[] = {
    QMA6100P_PARAMS
};

/**
 * @brief   QMA6100P defaults interrupt params if not defined for a board or application
 */
static const qma6100p_int_params_t qma6100p_int_params[] = {
    QMA6100P_INT_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t qma6100p_saul_info[] = {
    QMA6100P_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
