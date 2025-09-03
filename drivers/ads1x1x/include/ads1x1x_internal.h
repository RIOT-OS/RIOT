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
 * @name    ADS101x/111x Operational Status
 *
 * This bit indicates the status of the conversion.
 *
 * @{
 */
#define ADS1X1X_CONF_OS_CONV_MASK         (1 << 15)

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
 * @name ADS101x/111x operating modes
 *
 * MODE bit in the config register:
 * - @ref ADS1X1X_MODE_SINGLE : Single-shot (default), powers down after each conversion.
 * - @ref ADS1X1X_MODE_CONTINUOUS : Continuous conversions.
 *
 * @{
 */
#define ADS1X1X_MODE_MASK         (1 << 0)
#define ADS1X1X_MODE_SINGLE       (0 << 0)  /**< Single-shot / power-down */
#define ADS1X1X_MODE_CONTINUOUS   (1 << 0)  /**< Continuous conversion */
/** @} */

/**
 * @name ADS101x/111x data rate settings
 *
 * Configures the data rate (samples per second).
 * Values differ between ADS101x and ADS111x families.
 *
 * @{
 */
#ifdef ADS101X
#   define ADS1X1X_DATAR_MASK    ((1 << 7) | (1 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_128     ((0 << 7) | (0 << 6) | (0 << 5))  /**< 128 SPS */
#   define ADS1X1X_DATAR_250     ((0 << 7) | (0 << 6) | (1 << 5))  /**< 250 SPS */
#   define ADS1X1X_DATAR_490     ((0 << 7) | (1 << 6) | (0 << 5))  /**< 490 SPS */
#   define ADS1X1X_DATAR_920     ((0 << 7) | (1 << 6) | (1 << 5))  /**< 920 SPS */
#   define ADS1X1X_DATAR_1600    ((1 << 7) | (0 << 6) | (0 << 5))  /**< 1600 SPS (default) */
#   define ADS1X1X_DATAR_2400    ((1 << 7) | (0 << 6) | (1 << 5))  /**< 2400 SPS */
#   define ADS1X1X_DATAR_3300    ((1 << 7) | (1 << 6) | (0 << 5))  /**< 3300 SPS */
#else /* ADS111X */
#   define ADS1X1X_DATAR_MASK    ((1 << 7) | (1 << 6) | (1 << 5))
#   define ADS1X1X_DATAR_8       ((0 << 7) | (0 << 6) | (0 << 5))  /**< 8 SPS */
#   define ADS1X1X_DATAR_16      ((0 << 7) | (0 << 6) | (1 << 5))  /**< 16 SPS */
#   define ADS1X1X_DATAR_64      ((0 << 7) | (1 << 6) | (0 << 5))  /**< 64 SPS */
#   define ADS1X1X_DATAR_128     ((0 << 7) | (1 << 6) | (1 << 5))  /**< 128 SPS */
#   define ADS1X1X_DATAR_250     ((1 << 7) | (0 << 6) | (0 << 5))  /**< 250 SPS */
#   define ADS1X1X_DATAR_475     ((1 << 7) | (0 << 6) | (1 << 5))  /**< 475 SPS */
#   define ADS1X1X_DATAR_860     ((1 << 7) | (1 << 6) | (0 << 5))  /**< 860 SPS (default) */
#endif
/** @} */

/**
 * @name Comparator mode
 * 
 * Selects comparator operation
 * 
 * @{
 */
#define ADS1X1X_COMP_MODE_MASK        (1 << 4)
#define ADS1X1X_COMP_MODE_TRADITIONAL (0 << 4) /**< Traditional comparator */
#define ADS1X1X_COMP_MODE_WINDOW      (1 << 4) /**< Window comparator */
/** @} */

/**
 * @name Comparator polarity
 * 
 * Controls ALERT/RDY pin polarity
 * 
 * @{
 */
#define ADS1X1X_COMP_POLARITY_MASK (1 << 3)
#define ADS1X1X_COMP_POLARITY_LOW  (0 << 3) /**< Active low (default) */
#define ADS1X1X_COMP_POLARITY_HIGH (1 << 3) /**< Active high */
/** @} */

/**
 * @name Comparator latch
 * 
 * Latching comparator output
 * 
 * @{
 */
#define ADS1X1X_COMP_LATCH_MASK    (1 << 2)
#define ADS1X1X_COMP_LATCH_DISABLE (0 << 2) /**< Non-latching (default) */
#define ADS1X1X_COMP_LATCH_ENABLE  (1 << 2) /**< Latching until read */
/** @} */

/**
 * @name Comparator queue
 * 
 * Configures comparator trigger queue
 * 
 * @{
 */
#define ADS1X1X_COMP_QUEUE_MASK       ((1 << 1) | (1 << 0))
#define ADS1X1X_COMP_QUEUE_1          ((0 << 1) | (0 << 0))              /**< Assert after 1 conversion */
#define ADS1X1X_COMP_QUEUE_2          ((0 << 1) | (1 << 0))              /**< Assert after 2 conversions */
#define ADS1X1X_COMP_QUEUE_4          ((1 << 1) | (0 << 0))              /**< Assert after 4 conversions */
#define ADS1X1X_COMP_QUEUE_DISABLE    ((1 << 1) | (1 << 0))              /**< Disable comparator */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */


