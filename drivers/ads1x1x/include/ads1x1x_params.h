/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
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
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
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
#  define ADS1X1X_PARAM_I2C       (I2C_DEV(0))
#endif

/**
 * @def CONFIG_ADS1X1X_I2C_ADDR
 * @brief Default I2C address
 */
#ifndef CONFIG_ADS1X1X_I2C_ADDR
#  define CONFIG_ADS1X1X_I2C_ADDR       (0x48)
#endif

/**
 * @def ADS1X1X_PARAM_MUX
 * @brief Default multiplexer configuration
 */
#if IS_ACTIVE(CONFIG_ADS1X1X_MUX_AIN0_DIFFM_AIN1)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN0_DIFFM_AIN1)
#elif IS_ACTIVE(CONFIG_ADS1X1X_MUX_AIN0_DIFFM_AIN3)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN0_DIFFM_AIN3)
#elif IS_ACTIVE(CONFIG_ADS1X1X_MUX_AIN1_DIFFM_AIN3)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN1_DIFFM_AIN3)
#elif IS_ACTIVE(CONFIG_ADS1X1X_MUX_AIN2_DIFFM_AIN3)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN2_DIFFM_AIN3)
#elif IS_ACTIVE(CONFIG_ADS1X1X1_MUX_AIN0_SINGM)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN0_SINGM)
#elif IS_ACTIVE(CONFIG_ADS1X1X_MUX_AIN1_SINGM)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN1_SINGM)
#elif IS_ACTIVE(CONFIG_ADS1X1X_MUX_AIN2_SINGM)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN2_SINGM)
#elif IS_ACTIVE(CONFIG_ADS1X1X_MUX_AIN3_SINGM)
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN3_SINGM)
#endif

#ifndef ADS1X1X_PARAM_MUX
#  define ADS1X1X_PARAM_MUX       (ADS1X1X_AIN0_DIFFM_AIN1)
#endif

/**
 * @def ADS1X1X_PARAM_ALERT_PIN
 * @brief Default alert pin configuration
 */
#ifndef ADS1X1X_PARAM_ALERT_PIN
#  define ADS1X1X_PARAM_ALERT_PIN  (GPIO_UNDEF)
#endif

/**
 * @def CONFIG_ADS1X1X_LOW_LIMIT
 * @brief Default low limit for alert
 */
#ifndef CONFIG_ADS1X1X_LOW_LIMIT
#  define CONFIG_ADS1X1X_LOW_LIMIT  (10000U)
#endif

/**
 * @def CONFIG_ADS1X1X_HIGH_LIMIT
 * @brief Default high limit for alert
 */
#ifndef CONFIG_ADS1X1X_HIGH_LIMIT
#  define CONFIG_ADS1X1X_HIGH_LIMIT (20000U)
#endif

/**
 * @def ADS1X1X_PARAM_PGA
 * @brief Default programmable gain amplifier configuration
 */
#if IS_ACTIVE(CONFIG_ADS1X1X_PGA_FSR_6V144)
#  define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_6V144)
#elif IS_ACTIVE(CONFIG_ADS1X1X_PGA_FSR_4V096)
#  define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_4V096)
#elif IS_ACTIVE(CONFIG_ADS1X1X_PGA_FSR_2V048)
#  define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_2V048)
#elif IS_ACTIVE(CONFIG_ADS1X1X_PGA_FSR_1V024)
#  define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_1V024)
#elif IS_ACTIVE(CONFIG_ADS1X1X_PGA_FSR_0V512)
#  define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_0V512)
#elif IS_ACTIVE(CONFIG_ADS1X1X_PGA_FSR_0V256)
#  define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_0V256)
#endif

#ifndef ADS1X1X_PARAM_PGA
#  define ADS1X1X_PARAM_PGA       (ADS1X1X_PGA_FSR_2V048)
#endif

/**
 * @def ADS1X1X_PARAM_DATAR
 * @brief Default data rate configuration set to undefined
 */
#ifndef ADS1X1X_PARAM_DATAR
#  define ADS1X1X_PARAM_DATAR  ADS1X1X_DATAR_UNDEF
#endif

/**
 * @def ADS1X1X_PARAM_MODE
 * @brief Default operating mode
 */
#if IS_ACTIVE(CONFIG_ADS1X1X_MODE_SINGLE)
#  define ADS1X1X_PARAM_MODE  (ADS1X1X_MODE_SINGLE)
#elif IS_ACTIVE(CONFIG_ADS1X1X_MODE_CONTINUOUS)
#  define ADS1X1X_PARAM_MODE  (ADS1X1X_MODE_CONTINUOUS)
#endif

#ifndef ADS1X1X_PARAM_MODE
#  define ADS1X1X_PARAM_MODE  (ADS1X1X_MODE_SINGLE)
#endif

/**
 * @def ADS1X1X_PARAM_COMP_MODE
 * @brief Default comparator mode (No effect on ADS1113/1013)
 */
#if IS_ACTIVE(CONFIG_ADS1X1X_COMP_MODE_TRADITIONAL)
#  define ADS1X1X_PARAM_COMP_MODE  (ADS1X1X_COMP_MODE_TRADITIONAL)
#elif IS_ACTIVE(CONFIG_ADS1X1X_COMP_MODE_WINDOW)
#  define ADS1X1X_PARAM_COMP_MODE  (ADS1X1X_COMP_MODE_WINDOW)
#endif

#ifndef ADS1X1X_PARAM_COMP_MODE
#  define ADS1X1X_PARAM_COMP_MODE  (ADS1X1X_COMP_MODE_TRADITIONAL)
#endif

/**
 * @def ADS1X1X_PARAM_COMP_POLARITY
 * @brief Default comparator polarity (No effect on ADS1113/1013)
 */
#if IS_ACTIVE(CONFIG_ADS1X1X_COMP_POLARITY_LOW)
#  define ADS1X1X_PARAM_COMP_POLARITY (ADS1X1X_COMP_POLARITY_LOW)
#elif IS_ACTIVE(CONFIG_ADS1X1X_COMP_POLARITY_HIGH)
#  define ADS1X1X_PARAM_COMP_POLARITY (ADS1X1X_COMP_POLARITY_HIGH)
#endif

#ifndef ADS1X1X_PARAM_COMP_POLARITY
#  define ADS1X1X_PARAM_COMP_POLARITY (ADS1X1X_COMP_POLARITY_LOW)
#endif

/**
 * @def ADS1X1X_PARAM_COMP_LATCH
 * @brief Default comparator latch (No effect on ADS1113/1013)
 */
#if IS_ACTIVE(CONFIG_ADS1X1X_COMP_LATCH_DISABLE)
#  define ADS1X1X_PARAM_COMP_LATCH (ADS1X1X_COMP_LATCH_DISABLE)
#elif IS_ACTIVE(CONFIG_ADS1X1X_COMP_LATCH_ENABLE)
#  define ADS1X1X_PARAM_COMP_LATCH (ADS1X1X_COMP_LATCH_ENABLE)
#endif

#ifndef ADS1X1X_PARAM_COMP_LATCH
#  define ADS1X1X_PARAM_COMP_LATCH (ADS1X1X_COMP_LATCH_DISABLE)
#endif

/**
 * @def ADS1X1X_PARAM_COMP_QUEUE
 * @brief Default comparator queue (No effect on ADS1113/1013)
 */
#if IS_ACTIVE(CONFIG_ADS1X1X_COMP_QUEUE_1)
#  define ADS1X1X_PARAM_COMP_QUEUE (ADS1X1X_COMP_QUEUE_1)
#elif IS_ACTIVE(CONFIG_ADS1X1X_COMP_QUEUE_2)
#  define ADS1X1X_PARAM_COMP_QUEUE (ADS1X1X_COMP_QUEUE_2)
#elif IS_ACTIVE(CONFIG_ADS1X1X_COMP_QUEUE_4)
#  define ADS1X1X_PARAM_COMP_QUEUE (ADS1X1X_COMP_QUEUE_4)
#elif IS_ACTIVE(CONFIG_ADS1X1X_COMP_QUEUE_DISABLE)
#  define ADS1X1X_PARAM_COMP_QUEUE (ADS1X1X_COMP_QUEUE_DISABLE)
#endif

#ifndef ADS1X1X_PARAM_COMP_QUEUE
#  define ADS1X1X_PARAM_COMP_QUEUE (ADS1X1X_COMP_QUEUE_DISABLE)
#endif

/**
 * @def ADS1X1X_PARAM_BITS_RES
 * @brief Default bit resolution set to undefined
 */
#ifndef ADS1X1X_PARAM_BITS_RES
#  define ADS1X1X_PARAM_BITS_RES  (ADS1X1X_BITS_RES_UNDEF)
#endif

/**
 * @def ADS1X1X_PARAMS
 * @brief Default configuration parameters structure for ADS1X1X ADC devices
 *
 * This macro defines the default parameter structure used to initialize ADS1X1X
 * analog-to-digital converter devices. The structure contains all necessary
 * configuration options including resolution, data rate, gain, and operational modes.
 *
 * @warning The following parameters are mandatory and must be explicitly configured:
 *          - @ref ads1x1x_params_t::bits_res - ADC resolution, must be set to either:
 *            * ADS101X_BITS_RES for 12-bit resolution (ADS101x series)
 *            * ADS111X_BITS_RES for 16-bit resolution (ADS111x series)
 *          - @ref ads1x1x_params_t::dr - Data rate configuration, must be set to a
 *            valid sampling rate value appropriate for the selected device variant
 *
 * @note Other parameters in the structure have sensible defaults but can be
 *       customized based on application requirements such as input multiplexer
 *       configuration, programmable gain amplifier settings, and comparator modes.
 *
 * @see ads1x1x_params_t for detailed parameter structure definition
 * @see ads1x1x_init() for device initialization using these parameters
 */
#ifndef ADS1X1X_PARAMS
#  define ADS1X1X_PARAMS          { .i2c = ADS1X1X_PARAM_I2C,        \
                                  .addr = CONFIG_ADS1X1X_I2C_ADDR,       \
                                  .mux = ADS1X1X_PARAM_MUX,        \
                                  .pga = ADS1X1X_PARAM_PGA,        \
                                  .mode = ADS1X1X_PARAM_MODE,       \
                                  .dr = ADS1X1X_PARAM_DATAR,      \
                                  .bits_res = ADS1X1X_PARAM_BITS_RES }
#endif

/**
 * @def ADS1X1X_ALERT_PARAMS
 * @brief Default ADS1X1X alert parameters structure
 */
#ifndef ADS1X1X_ALERT_PARAMS
#  define ADS1X1X_ALERT_PARAMS    { .i2c = ADS1X1X_PARAM_I2C,        \
                                  .addr = CONFIG_ADS1X1X_I2C_ADDR,       \
                                  .comp_mode = ADS1X1X_PARAM_COMP_MODE,  \
                                  .comp_polarity = ADS1X1X_PARAM_COMP_POLARITY, \
                                  .comp_latch = ADS1X1X_PARAM_COMP_LATCH,  \
                                  .comp_queue = ADS1X1X_PARAM_COMP_QUEUE,  \
                                  .alert_pin = ADS1X1X_PARAM_ALERT_PIN,  \
                                  .low_limit = CONFIG_ADS1X1X_LOW_LIMIT,  \
                                  .high_limit = CONFIG_ADS1X1X_HIGH_LIMIT }
#endif

/**
 * @def ADS1X1X_SAUL_INFO
 * @brief Additional SAUL registry information
 */
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
