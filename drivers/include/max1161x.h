/*
 * SPDX-FileCopyrightText: 2026 Mathis LECRIVAIN <lecrivain.mathis@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup   drivers_max1161x MAX1161X ADC device driver
 * @ingroup    drivers_sensors
 * @ingroup    drivers_saul
 * @brief      I2C Analog-to-Digital Converter device driver
 *
 * This driver works with max11612, max11613, max11614, max11615, max11616 and max11617 versions.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief      MAX1161X ADC device driver
 *
 * @author     Mathis Lécrivain <lecrivain.mathis@gmail.com>
 */

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   MAX1161X default I2C address for device variants
 */
#if defined(MODULE_MAX11612) || defined(MODULE_MAX11613)
#  define MAX1161X_I2C_ADDRESS (0x34)
#elif defined(MODULE_MAX11614) || defined(MODULE_MAX11615)
#  define MAX1161X_I2C_ADDRESS (0x33)
#elif defined(MODULE_MAX11616) || defined(MODULE_MAX11617)
#  define MAX1161X_I2C_ADDRESS (0x35)
#else
#  define MAX1161X_I2C_ADDRESS (-1)
#  error "MAX1161X: Failed to select default address: unknown MAX1161X device variant!"
#endif

/**
 * @brief   MAX1161X channel number for device variants
 */
#if defined(MODULE_MAX11612) || defined(MODULE_MAX11613)
#  define MAX1161X_NUM_CHANNELS (4)
#elif defined(MODULE_MAX11614) || defined(MODULE_MAX11615)
#  define MAX1161X_NUM_CHANNELS (8)
#elif defined(MODULE_MAX11616) || defined(MODULE_MAX11617)
#  define MAX1161X_NUM_CHANNELS (12)
#else
#  define MAX1161X_NUM_CHANNELS (-1)
#  error "MAX1161X: Failed to select channel number: unknown MAX1161X device variant!"
#endif

/**
 * @brief   Reference voltage selection (Table 6. Reference Voltage, AIN_/REF, and REF Format)
 */
typedef enum {
    MAX1161X_REF_VDD_AIN_NC_OFF = 0x0,                  /**< VDD, AIN_/REF is an analog input, internal ref off */
    MAX1161X_REF_EXT_RIN_IN_OFF = 0x2,                  /**< External, AIN_/REF is a reference input, internal ref off */
    MAX1161X_REF_INT_AIN_NC_OFF = 0x4,                  /**< Internal, AIN_/REF is an analog input, internal ref off */
    MAX1161X_REF_INT_AIN_NC_ON = 0x5,                   /**< Internal, AIN_/REF is an analog input, internal ref on */
    MAX1161X_REF_INT_ROUT_OUT_OFF = 0x6,                /**< Internal, AIN_/REF is a reference output, internal ref off */
    MAX1161X_REF_INT_ROUT_OUT_ON = 0x7,                 /**< Internal, AIN_/REF is a reference output, internal ref on */
    MAX1161X_REF_DEFAULT = MAX1161X_REF_VDD_AIN_NC_OFF, /**< Default reference */
} max1161x_ref_t;

/**
 * @brief   Clock mode
 */
typedef enum {
    MAX1161X_CLOCK_INT = 0,                      /**< Internal clock */
    MAX1161X_CLOCK_EXT = 1,                      /**< External clock */
    MAX1161X_CLOCK_DEFAULT = MAX1161X_CLOCK_INT, /**< Default clock */
} max1161x_clock_t;

/**
 * @brief   Unipolar/bipolar selection
 */
typedef enum {
    MAX1161X_UNIPOLAR = 0,                           /**< Unipolar mode */
    MAX1161X_BIPOLAR = 1,                            /**< Bipolar mode (differential conversions only) */
    MAX1161X_UNIBIPOLAR_DEFAULT = MAX1161X_UNIPOLAR, /**< Default uni/bipolar mode */
} max1161x_unibipolar_t;

/**
 * @brief   Scan mode (Table 5. Scanning Configuration)
 */
typedef enum {
    MAX1161X_SCAN_FROM_ZERO = 0x0,              /**< Scan from channel 0 up to the selected channel */
    MAX1161X_SCAN_EIGHT_TIMES = 0x1,            /**< Convert the selected channel eight times */
    MAX1161X_SCAN_UPPER = 0x2,                  /**< Scan from the selected channel up to the last channel */
    MAX1161X_SCAN_NONE = 0x3,                   /**< Convert the selected channel once */
    MAX1161X_SCAN_DEFAULT = MAX1161X_SCAN_NONE, /**< Default scan mode */
} max1161x_scan_t;

/**
 * @brief   Channel selection
 *
 * @note    Both single-ended and differential modes share the same enum.
 *          CS[3:0] values are the same for both modes, but have different meanings.
 *          The correct meaning is selected by the SGL/DIF bit in the config byte.
 */
typedef enum {
    /* Table 3. Channel Selection in Single-Ended Mode (SGL/DIF = 1). */
    MAX1161X_CHANNEL_CH0 = 0x0,  /**< CH0, REF */
    MAX1161X_CHANNEL_CH1 = 0x1,  /**< CH1, REF */
    MAX1161X_CHANNEL_CH2 = 0x2,  /**< CH2, REF */
    MAX1161X_CHANNEL_CH3 = 0x3,  /**< CH3, REF */
    MAX1161X_CHANNEL_CH4 = 0x4,  /**< CH4, REF */
    MAX1161X_CHANNEL_CH5 = 0x5,  /**< CH5, REF */
    MAX1161X_CHANNEL_CH6 = 0x6,  /**< CH6, REF */
    MAX1161X_CHANNEL_CH7 = 0x7,  /**< CH7, REF */
    MAX1161X_CHANNEL_CH8 = 0x8,  /**< CH8, REF */
    MAX1161X_CHANNEL_CH9 = 0x9,  /**< CH9, REF */
    MAX1161X_CHANNEL_CH10 = 0xA, /**< CH10, REF */
    MAX1161X_CHANNEL_CH11 = 0xB, /**< CH11, REF */

    /* Table 4. Channel Selection in Differential Mode (SGL/DIF = 0). */
    MAX1161X_CHANNEL_CH0_CH1 = 0x0,   /**< +CH0, -CH1 */
    MAX1161X_CHANNEL_CH1_CH0 = 0x1,   /**< +CH1, -CH0 */
    MAX1161X_CHANNEL_CH2_CH3 = 0x2,   /**< +CH2, -CH3 */
    MAX1161X_CHANNEL_CH3_CH2 = 0x3,   /**< +CH3, -CH2 */
    MAX1161X_CHANNEL_CH4_CH5 = 0x4,   /**< +CH4, -CH5 */
    MAX1161X_CHANNEL_CH5_CH4 = 0x5,   /**< +CH5, -CH4 */
    MAX1161X_CHANNEL_CH6_CH7 = 0x6,   /**< +CH6, -CH7 */
    MAX1161X_CHANNEL_CH7_CH6 = 0x7,   /**< +CH7, -CH6 */
    MAX1161X_CHANNEL_CH8_CH9 = 0x8,   /**< +CH8, -CH9 */
    MAX1161X_CHANNEL_CH9_CH8 = 0x9,   /**< +CH9, -CH8 */
    MAX1161X_CHANNEL_CH10_CH11 = 0xA, /**< +CH10, -CH11 */
    MAX1161X_CHANNEL_CH11_CH10 = 0xB, /**< +CH11, -CH10 */

    MAX1161X_CHANNEL_DEFAULT = MAX1161X_CHANNEL_CH0, /**< Default channel */
} max1161x_channel_t;

/**
 * @brief   Single-ended/differential selection
 */
typedef enum {
    MAX1161X_DIFFERENTIAL = 0,                        /**< Differential mode */
    MAX1161X_SINGLE_ENDED = 1,                        /**< Single-ended mode */
    MAX1161X_SGLDIFF_DEFAULT = MAX1161X_SINGLE_ENDED, /**< Default sgl/diff mode */
} max1161x_sgldiff_t;

/**
 * @brief   MAX1161X device parameters
 */
typedef struct {
    i2c_t i2c;                        /**< I2C device */
    uint8_t addr;                     /**< I2C address */
    max1161x_ref_t ref;               /**< reference selection */
    max1161x_clock_t clock;           /**< clock source selection */
    max1161x_unibipolar_t unibipolar; /**< unipolar/bipolar selection */
    max1161x_scan_t scan;             /**< scan mode selection */
    max1161x_channel_t channel;       /**< selected channel */
    max1161x_sgldiff_t sgldiff;       /**< single-ended/differential selection */
} max1161x_params_t;

/**
 * @brief   MAX1161X device descriptor
 */
typedef struct {
    max1161x_params_t params; /**< device driver configuration */
    uint8_t setup;            /**< current setup byte */
    uint8_t config;           /**< current configuration byte */
} max1161x_t;

/**
 * @brief   Initialize a MAX1161X ADC device
 *
 * @param[in,out] dev       device descriptor
 * @param[in]     params    device configuration
 *
 * @retval 0            on success
 * @retval -ENODEV      if the device did not acknowledge its configuration
 * @retval <0           negative errno code from the I2C bus on communication error
 */
int max1161x_init(max1161x_t *dev, const max1161x_params_t *params);

/**
 * @brief   Reset a MAX1161X ADC device to its power-up configuration
 *
 * @param[in,out] dev       device descriptor
 *
 * @retval 0            on success
 * @retval <0           negative errno code from the I2C bus on communication error
 */
int max1161x_reset(max1161x_t *dev);

/**
 * @brief   Read a raw ADC value as configured in driver parameters
 *
 * @param[in]  dev          device descriptor
 * @param[out] raw          read value
 *
 * @retval 0            on success
 * @retval <0           negative errno code from the I2C bus on communication error
 */
int max1161x_read_raw(const max1161x_t *dev, int16_t *raw);

/**
 * @brief   Read a raw ADC value from a specific channel
 *
 * @warning The selected channel remains selected after the read operation.
 *
 * @param[in,out] dev       device descriptor
 * @param[in]     chan      channel to read
 * @param[out]    raw       read value
 *
 * @retval 0            on success
 * @retval <0           negative errno code from the I2C bus on communication error
 */
int max1161x_read_channel_raw(max1161x_t *dev, max1161x_channel_t chan, int16_t *raw);

#ifdef __cplusplus
}
#endif

/** @} */
