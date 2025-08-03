/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_vl53l1x
 * @brief       Register definitions for ST VL53L1X Time-of-Flight (ToF) ranging sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/** Register addresses are imported from ST VL53L1X Register Map definitions */
#include "vl53l1x_register_map.h"

#if !DOXYGEN
/**
 * @name    Register addresses
 *
 * Register addresses are included from ST VL53L1X Register Map definitions.
 * Only additionial addresses are defined here.
 *
 * @{
 */
#  define VL53L1X_MM_CONFIG__TIMEOUT_MACROP_A     (0x005a)
#  define VL53L1X_MM_CONFIG__TIMEOUT_MACROP_B     (0x005c)
#  define VL53L1X_RANGE_CONFIG__TIMEOUT_MACROP_A  (0x005e)
#  define VL53L1X_RANGE_CONFIG__TIMEOUT_MACROP_B  (0x0061)
/** @} */
#endif /* !DOXYGEN */

#define VL53L1X_DEVICE_ID   (0xea)  /**< VL53L1X Model ID */
#define VL53L1X_I2C_ADDRESS (0x29)  /**< VL53L1X I2C address */

#ifdef __cplusplus
}
#endif

/** @} */
