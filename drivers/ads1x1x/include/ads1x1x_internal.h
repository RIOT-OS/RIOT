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
 * @brief       Internal definitions for ADS101x/111x devices
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADS101x/111x register addresses
 * @{
 */
#define ADS1X1X_CONV_RES_ADDR      (0) /**< Conversion register */
#define ADS1X1X_CONF_ADDR          (1) /**< Configuration register */
#define ADS1X1X_LOW_LIMIT_ADDR     (2) /**< Low limit register */
#define ADS1X1X_HIGH_LIMIT_ADDR    (3) /**< High limit register */
/** @} */

/**
 * @name    ADS101x/111x Config flags
 *
 * Comparator flags have no effect on ADS1013 and ADS1113.
 *
 * @{
 */
#define ADS1X1X_CONF_OS_CONV         (1 << 7)
#define ADS1X1X_CONF_COMP_MODE_WIND  (1 << 4)
#define ADS1X1X_CONF_COMP_DIS        ((1 << 1) | (1 << 0))
/** @} */

/**
 * @name    ADS101x/111x mux settings
 *
 * Supports both single mode and differential.
 * This has no effect on ADS1013-4 and ADS1113-4.
 *
 * @{
 */
#define ADS1X1X_MUX_MASK           ((1 << 6) | (1 << 5) | (1 << 4))
#define ADS1X1X_AIN0_DIFFM_AIN1    ((0 << 6) | (0 << 5) | (0 << 4))
#define ADS1X1X_AIN0_DIFFM_AIN3    ((0 << 6) | (0 << 5) | (1 << 4))
#define ADS1X1X_AIN1_DIFFM_AIN3    ((0 << 6) | (1 << 5) | (0 << 4))
#define ADS1X1X_AIN2_DIFFM_AIN3    ((0 << 6) | (1 << 5) | (1 << 4))
#define ADS1X1X_AIN0_SINGM         ((1 << 6) | (0 << 5) | (0 << 4))
#define ADS1X1X_AIN1_SINGM         ((1 << 6) | (0 << 5) | (1 << 4))
#define ADS1X1X_AIN2_SINGM         ((1 << 6) | (1 << 5) | (0 << 4))
#define ADS1X1X_AIN3_SINGM         ((1 << 6) | (1 << 5) | (1 << 4))
/** @} */

/**
 * @name    ADS101x/111x programmable gain
 *
 * Sets the full-scale range (max voltage value).
 * This has no effect on ADS1013 and ADS1113 (both use 2.048V FSR).
 *
 * @{
 */
#define ADS1X1X_PGA_MASK         ((1 << 3) | (1 << 2) | (1 << 1))
#define ADS1X1X_PGA_FSR_6V144    ((0 << 3) | (0 << 2) | (0 << 1))
#define ADS1X1X_PGA_FSR_4V096    ((0 << 3) | (0 << 2) | (1 << 1))
#define ADS1X1X_PGA_FSR_2V048    ((0 << 3) | (1 << 2) | (0 << 1))
#define ADS1X1X_PGA_FSR_1V024    ((0 << 3) | (1 << 2) | (1 << 1))
#define ADS1X1X_PGA_FSR_0V512    ((1 << 3) | (0 << 2) | (0 << 1))
#define ADS1X1X_PGA_FSR_0V256    ((1 << 3) | (0 << 2) | (1 << 1))
/** @} */


/**
 * @def ADS1X1X_DATAR
 * @brief Data rate configuration
 */
#ifdef ADS101X
#   define ADS1X1X_DATAR_MASK    ((1 << 7) | (1 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_128     ((0 << 7) | (0 << 6) | (0 << 5))
#   define ADS1X1X_DATAR_250     ((0 << 7) | (0 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_490     ((0 << 7) | (1 << 6) | (0 << 5))
#   define ADS1X1X_DATAR_920     ((0 << 7) | (1 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_1600    ((1 << 7) | (0 << 6) | (0 << 5))
#   define ADS1X1X_DATAR_2400    ((1 << 7) | (0 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_3300    ((1 << 7) | (1 << 6) | (0 << 5))
#else /**<ADS111X */
#   define ADS1X1X_DATAR_MASK    ((1 << 7) | (1 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_8       ((0 << 7) | (0 << 6) | (0 << 5))
#   define ADS1X1X_DATAR_16      ((0 << 7) | (0 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_64      ((0 << 7) | (1 << 6) | (0 << 5))
#   define ADS1X1X_DATAR_128     ((0 << 7) | (1 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_250     ((1 << 7) | (0 << 6) | (0 << 5))
#   define ADS1X1X_DATAR_475     ((1 << 7) | (0 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_860     ((1 << 7) | (1 << 6) | (0 << 5))
#endif

/**
 * @def ADS1X1X_MODE
 * @brief Operating mode configuration
 */
#define ADS1X1X_MODE_MASK         (1 << 0)
#define ADS1X1X_MODE_SINGLE       (0 << 0)
#define ADS1X1X_MODE_CONTINUOUS   (1 << 0)

/**
 * @def ADS1X1X_COMP_MODE
 * @brief Comparator mode configuration
 */
#define ADS1X1X_COMP_MODE_MASK     (1 << 4)
#define ADS1X1X_COMP_MODE_TRADITIONAL (0 << 4)
#define ADS1X1X_COMP_MODE_WINDOW    (1 << 4)

/**
 * @def ADS1X1X_COMP_POLARITY
 * @brief Comparator polarity configuration
 */
#define ADS1X1X_COMP_POLARITY_MASK (1 << 3)
#define ADS1X1X_COMP_POLARITY_LOW  (0 << 3)
#define ADS1X1X_COMP_POLARITY_HIGH (1 << 3)

/**
 * @def ADS1X1X_COMP_LATCH
 * @brief Comparator latch configuration
 */
#define ADS1X1X_COMP_LATCH_MASK    (1 << 2)
#define ADS1X1X_COMP_LATCH_DISABLE (0 << 2)
#define ADS1X1X_COMP_LATCH_ENABLE  (1 << 2)

/**
 * @def ADS1X1X_COMP_QUEUE
 * @brief Comparator queue configuration
 */
#define ADS1X1X_COMP_QUEUE_MASK    (1 << 1) | (1 << 0)
#define ADS1X1X_COMP_QUEUE_1        (0 << 0) | (1 << 0) /**< Assert after one conversion */
#define ADS1X1X_COMP_QUEUE_2        (1 << 0) | (1 << 1) /**< Assert after two conversions */
#define ADS1X1X_COMP_QUEUE_4        (1 << 0) | (1 << 1) | (1 << 2) /**< Assert after four conversions */
#define ADS1X1X_COMP_QUEUE_DISABLE  (0 << 0) | (0 << 1) | (0 << 2) /**< Disable comparator queue */

#ifdef __cplusplus
}
#endif

/** @} */


