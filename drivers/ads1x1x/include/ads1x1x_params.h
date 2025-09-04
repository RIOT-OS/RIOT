/*
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ads1x1x
 * @{
 *
 * @file
 * @brief       Default configuration for ADS101x/111x devices
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 */

#include "periph/i2c.h"
#include "saul_reg.h"
#include "ads1x1x.h"
#include "ads1x1x_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ADS101x/111x driver
 * @{
 */

/**
 * @def ADS1X1X_PARAM_I2C
 * @brief Default I2C bus
 */
#ifndef ADS1X1X_PARAM_I2C
#   define ADS1X1X_PARAM_I2C       (I2C_DEV(0))
#endif

/**
 * @def ADS1X1X_PARAM_ADDR
 * @brief Default I2C address
 */
#ifndef ADS1X1X_PARAM_ADDR
#   define ADS1X1X_PARAM_ADDR       (0x48)
#endif

/**
 * @def ADS1X1X_PARAM_MUX
 * @brief Default multiplexer configuration
 */
#ifndef ADS1X1X_PARAM_MUX
#   define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN0_DIFFM_AIN1)
#endif

/**
 * @def ADS1X1X_PARAM_ALERT_PIN
 * @brief Default alert pin configuration
 */
#ifndef ADS1X1X_PARAM_ALERT_PIN
#   define ADS1X1X_PARAM_ALERT_PIN  (GPIO_UNDEF)
#endif

/**
 * @def ADS1X1X_PARAM_LOW_LIMIT
 * @brief Default low limit for alert
 */
#ifndef ADS1X1X_PARAM_LOW_LIMIT
#   define ADS1X1X_PARAM_LOW_LIMIT  (10000U)
#endif

/**
 * @def ADS1X1X_PARAM_HIGH_LIMIT
 * @brief Default high limit for alert
 */
#ifndef ADS1X1X_PARAM_HIGH_LIMIT
#   define ADS1X1X_PARAM_HIGH_LIMIT (20000U)
#endif

/**
 * @def ADS1X1X_BITS_RES
 * @brief Default bit resolution
 */
#if MODULE_ADS101X
#   define ADS1X1X_BITS_RES  (12)
#else /**<ADS111X */
#   define ADS1X1X_BITS_RES  (16)
#endif


/**
 * @def ADS1X1X_PARAM_PGA
 * @brief Default programmable gain amplifier configuration
 */
#ifndef ADS1X1X_PARAM_PGA
#   define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_2V048)
#endif

/**
 * @def ADS1X1X_PARAM_DR
 * @brief Default data rate configuration
 */
#if MODULE_ADS101X
#   define ADS1X1X_PARAM_DATAR   (ADS1X1X_DATAR_1600)
#else /**<ADS111X */
#   define ADS1X1X_PARAM_DATAR  (ADS1X1X_DATAR_128)
#endif



/**
 * @def ADS1X1X_PARAM_MODE
 * @brief Default operating mode
 */
#ifndef ADS1X1X_PARAM_MODE
#  define ADS1X1X_PARAM_MODE  (ADS1X1X_MODE_SINGLE)
#endif

/**
 * @def ADS1X1X_PARAM_COMP_MODE
 * @brief Default comparator mode (No effect on ADS1113/1013)
 */
#ifndef ADS1X1X_PARAM_COMP_MODE
#  define ADS1X1X_PARAM_COMP_MODE  (ADS1X1X_COMP_MODE_TRADITIONAL)
#endif


/**
 * @def ADS1X1X_PARAM_COMP_POLARITY
 * @brief Default comparator polarity (No effect on ADS1113/1013)
 */
#ifndef ADS1X1X_PARAM_COMP_POLARITY
#  define ADS1X1X_PARAM_COMP_POLARITY (ADS1X1X_COMP_POLARITY_LOW)
#endif

/**
 * @def ADS1X1X_PARAM_COMP_LATCH
 * @brief Default comparator latch (No effect on ADS1113/1013)
 */
#ifndef ADS1X1X_PARAM_COMP_LATCH
#  define ADS1X1X_PARAM_COMP_LATCH (ADS1X1X_COMP_LATCH_DISABLE)
#endif

/**
 * @def ADS1X1X_PARAM_COMP_QUEUE
 * @brief Default comparator queue (No effect on ADS1113/1013)
 */
#ifndef ADS1X1X_PARAM_COMP_QUEUE
#  define ADS1X1X_PARAM_COMP_QUEUE (ADS1X1X_COMP_QUEUE_DISABLE)
#endif


#ifndef ADS1X1X_PARAMS
#define ADS1X1X_PARAMS          { .i2c        = ADS1X1X_PARAM_I2C,        \
                                  .addr       = ADS1X1X_PARAM_ADDR,       \
                                  .mux        = ADS1X1X_PARAM_MUX,        \
                                  .pga        = ADS1X1X_PARAM_PGA,        \
                                  .mode       = ADS1X1X_PARAM_MODE,       \
                                  .dr         = ADS1X1X_PARAM_DATAR        }

#endif

#ifndef ADS1X1X_ALERT_PARAMS
#define ADS1X1X_ALERT_PARAMS    { .i2c        = ADS1X1X_PARAM_I2C,        \
                                  .addr       = ADS1X1X_PARAM_ADDR,       \
                                  .comp_mode  = ADS1X1X_PARAM_COMP_MODE,  \
                                  .comp_polarity = ADS1X1X_PARAM_COMP_POLARITY, \
                                  .comp_latch = ADS1X1X_PARAM_COMP_LATCH,  \
                                  .comp_queue = ADS1X1X_PARAM_COMP_QUEUE,  \
                                  .alert_pin  = ADS1X1X_PARAM_ALERT_PIN,  \
                                  .low_limit  = ADS1X1X_PARAM_LOW_LIMIT,  \
                                  .high_limit = ADS1X1X_PARAM_HIGH_LIMIT }
#endif
#ifndef ADS1X1X_SAUL_INFO
#define ADS1X1X_SAUL_INFO       { .name = "ads1x1x" }
#endif
/** @} */

/**
 * @brief   ADS1X1X defaults if not defined for a board or application
 */
static const ads1x1x_params_t ads1x1x_params[] =
{
    ADS1X1X_PARAMS
};

/**
 * @brief   ADS1X1X alert defaults if not defined for a board or application
 */
static const ads1x1x_alert_params_t ads1x1x_alert_params[] =
{
    ADS1X1X_ALERT_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t ads1x1x_saul_info[] =
{
    ADS1X1X_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

/** @} */
