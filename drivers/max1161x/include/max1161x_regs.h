/*
 * SPDX-FileCopyrightText: 2026 Mathis LECRIVAIN <lecrivain.mathis@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_max1161x
 * @{
 *
 * @file
 * @brief       Register definition for MAX1161X devices
 *
 * @author      Mathis Lécrivain <lecrivain.mathis@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Setup byte bits
 * @{
 */
#define MAX1161X_SETUP_REG           (1 << 7)                           /**< Setup byte identifier (always 1) */
#define MAX1161X_SETUP_SEL_POS       (4)                                /**< Position of the SEL[2:0] field */
#define MAX1161X_SETUP_SEL_MASK      (0x07 << MAX1161X_SETUP_SEL_POS)   /**< Reference selection */
#define MAX1161X_SETUP_CLK_EXTERNAL  (1 << 3)                           /**< External clock (internal clock if cleared) */
#define MAX1161X_SETUP_BIPOLAR       (1 << 2)                           /**< Bipolar mode (unipolar mode if cleared) */
#define MAX1161X_SETUP_NO_RESET      (1 << 1)                           /**< No reset (resets the configuration byte if cleared) */
/** @} */

/**
 * @name    Configuration byte bits
 * @{
 */
#define MAX1161X_CONFIG_REG          (0 << 7)                           /**< Configuration byte identifier (always 0) */
#define MAX1161X_CONFIG_SCAN_POS     (5)                                /**< Position of the SCAN[1:0] field */
#define MAX1161X_CONFIG_SCAN_MASK    (0x03 << MAX1161X_CONFIG_SCAN_POS) /**< Scan mode selection */
#define MAX1161X_CONFIG_CS_POS       (1)                                /**< Position of the CS[3:0] field */
#define MAX1161X_CONFIG_CS_MASK      (0x0F << MAX1161X_CONFIG_CS_POS)   /**< Channel selection */
#define MAX1161X_CONFIG_SINGLE_ENDED (1 << 0)                           /**< Single-ended mode (differential mode if cleared) */
/** @} */

/**
 * @name    Data format
 * @{
 */
#define MAX1161X_DATA_MASK           (0x0FFF)   /**< 12-bit conversion result mask */
#define MAX1161X_DATA_SIGN_BIT       (0x0800)   /**< Sign bit of a bipolar conversion result */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
