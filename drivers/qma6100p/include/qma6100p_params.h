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
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 */

#include "periph/gpio.h"
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
#define QMA6100P_I2C_ADDR_LOW  (0x12) /**< AD0 connected to GND */
#define QMA6100P_I2C_ADDR_HIGH (0x13) /**< AD0 connected to VDD */
/** @} */

/**
 * @def QMA6100P_PARAM_I2C
 * @brief Default I2C bus
 */
#ifndef QMA6100P_PARAM_I2C
#  define QMA6100P_PARAM_I2C (I2C_DEV(0))
#endif

/**
 * @def QMA6100P_PARAM_I2C_ADDR
 * @brief Default I2C address: must be defined by the board depending on AD0 pin wiring.
 *        Use @ref QMA6100P_I2C_ADDR_LOW (0x12) when AD0 is connected to GND,
 *        or @ref QMA6100P_I2C_ADDR_HIGH (0x13) when AD0 is connected to VDD.
 */
#ifndef QMA6100P_PARAM_I2C_ADDR
#  define QMA6100P_PARAM_I2C_ADDR (QMA6100P_I2C_ADDR_LOW)
#endif

/**
 * @def QMA6100P_PARAM_RATE
 * @brief Default output data rate
 */
#ifndef QMA6100P_PARAM_RATE
#  define QMA6100P_PARAM_RATE (QMA6100P_ODR_100HZ)
#endif

/**
 * @def QMA6100P_PARAM_RANGE
 * @brief Default full-scale range
 */
#ifndef QMA6100P_PARAM_RANGE
#  define QMA6100P_PARAM_RANGE (QMA6100P_RANGE_2G)
#endif

/**
 * @def QMA6100P_PARAM_OFFSET
 * @brief Default per-axis calibration offset (X, Y, Z) in raw LSB units
 */
#ifndef QMA6100P_PARAM_OFFSET
#  define QMA6100P_PARAM_OFFSET { 0, 0, 0 }
#endif

/**
 * @def QMA6100P_PARAM_MODE
 * @brief Default mode
 */
#ifndef QMA6100P_PARAM_MODE
#  define QMA6100P_PARAM_MODE (QMA6100P_MODE_ACTIVE)
#endif

/**
 * @def QMA6100P_PARAM_INT_PIN
 * @brief Default interrupt pin. Set to GPIO_UNDEF to disable interrupt-driven
 *        operation and use polling instead.
 */
#ifndef QMA6100P_PARAM_INT_PIN
#  define QMA6100P_PARAM_INT_PIN_UNDEF (GPIO_UNDEF) /**< fallback when board has no INT pin */
#  define QMA6100P_PARAM_INT_PIN       (QMA6100P_PARAM_INT_PIN_UNDEF)
#endif

/**
 * @def QMA6100P_PARAM_INT_ACTIVE_LEVEL
 * @brief Default interrupt pin active level.
 */
#ifndef QMA6100P_PARAM_INT_ACTIVE_LEVEL
#  define QMA6100P_PARAM_INT_ACTIVE_LEVEL (QMA6100P_INTPIN_ACTIVE_HIGH)
#endif

/**
 * @def QMA6100P_PARAM_INT_PIN_MODE
 * @brief Default interrupt pin output mode (push-pull or open-drain).
 */
#ifndef QMA6100P_PARAM_INT_PIN_MODE
#  define QMA6100P_PARAM_INT_PIN_MODE (QMA6100P_INTPIN_PUSH_PULL)
#endif

/**
 * @def QMA6100P_PARAM_INT_LATCH
 * @brief Default interrupt latch mode. Non-latched clears automatically;
 *        latched holds until the status register is read.
 */
#ifndef QMA6100P_PARAM_INT_LATCH
#  define QMA6100P_PARAM_INT_LATCH (QMA6100P_INT_CFG_NON_LATCH)
#endif

/**
 * @def QMA6100P_PARAM_INT_CLEAR
 * @brief Default interrupt clear behavior. Controls whether any read of
 *        the status registers (0x09–0x0D) clears all pending interrupts.
 */
#ifndef QMA6100P_PARAM_INT_CLEAR
#  define QMA6100P_PARAM_INT_CLEAR (QMA6100P_INT_CFG_CLR_ON_ANY_READ)
#endif

/**
 * @def QMA6100P_PARAM_INT_SHADOW
 * @brief Default shadow mode for acceleration data registers
 */
#ifndef QMA6100P_PARAM_INT_SHADOW
#  define QMA6100P_PARAM_INT_SHADOW (QMA6100P_INT_CFG_SHADOW_DIS)
#endif

/**
 * @def QMA6100P_PARAM_INT_PIN_NUM
 * @brief QMA6100P interrupt output pin routed to the MCU */
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
                            .offset = QMA6100P_PARAM_OFFSET, \
                            .mode = QMA6100P_PARAM_MODE }
#endif

/**
 * @def QMA6100P_INT_PARAMS
 * @brief Default interrupt configuration parameters structure for QMA6100P devices
 */
#ifndef QMA6100P_INT_PARAMS
#  define QMA6100P_INT_PARAMS { .interrupt_pin = QMA6100P_PARAM_INT_PIN,              \
                                .active_level_int = QMA6100P_PARAM_INT_ACTIVE_LEVEL,  \
                                .pin_mode_int = QMA6100P_PARAM_INT_PIN_MODE,          \
                                .interrupt_latch = QMA6100P_PARAM_INT_LATCH,          \
                                .interrupt_clear_behavior = QMA6100P_PARAM_INT_CLEAR, \
                                .interrupt_shadow = QMA6100P_PARAM_INT_SHADOW,        \
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
 * @brief   QMA6100P defaults if not defined for a board or application
 */
static const qma6100p_params_t qma6100p_params[] = {
    QMA6100P_PARAMS
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
