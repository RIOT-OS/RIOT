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

#if !DOXYGEN
/**
 * @name    Register addresses
 *
 * @{
 */
#  define VL53L1X_IDENTIFICATION__REVISION_ID   (0x0111)
#  define VL53L1X_IDENTIFICATION__MODULE_TYPE   (0x0110)
#  define VL53L1X_IDENTIFICATION__MODULE_ID     (0x0112)
#  define VL53L1X_PAD_I2C_HV__EXTSUP_CONFIG     (0x002e)
#  define VL53L1X_SOFT_RESET                    (0x0b00)
/** @} */
#endif /* !DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
