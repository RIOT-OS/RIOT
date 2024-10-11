/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9x50
 *
 * @{
 * @file
 * @brief       Internal config for the MPU-9X50 (MPU9150 and MPU9250)
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */
#ifndef MPU9X50_INTERNAL_H
#define MPU9X50_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power Management 1 register macros
 * @{
 */
#define MPU9X50_PWR_WAKEUP          (0x00)
#define MPU9X50_PWR_PLL             (0x01)
#define MPU9X50_PWR_RESET           (0x80)
/** @} */

/**
 * @name    Power Management 2 register macros
 * @{
 */
#define MPU9X50_PWR_GYRO            (0x07)
#define MPU9X50_PWR_ACCEL           (0x38)
/** @} */

/**
 * @name    Sleep times in microseconds
 * @{
 */
#define MPU9X50_COMP_MODE_SLEEP_MS  (1)     /**< 1ms sleep for comp mode */
#define MPU9X50_BYPASS_SLEEP_MS     (3)     /**< 3ms sleep for bypass */
#define MPU9X50_PWR_CHANGE_SLEEP_MS (50)    /**< 50ms sleep to change power */
#define MPU9X50_RESET_SLEEP_MS      (100)   /**< 100ms sleep during driver reset */
/** @} */

/**
 * @name    MPU-9X50 compass operating modes and reg values
 * @{
 */
#define MPU9X50_COMP_POWER_DOWN     (0x00)
#define MPU9X50_COMP_SINGLE_MEASURE (0x01)
#define MPU9X50_COMP_SELF_TEST      (0x08)
#define MPU9X50_COMP_FUSE_ROM       (0x0F)
#define MPU9X50_COMP_WHOAMI_ANSWER  (0x48)  /**< MPU9X50 WHO_AM_I answer register */
/** @} */

/**
 * @name    MPU-9X50 temperature calibration values
 * @{
 */
#ifdef MODULE_MPU9150
#define MPU9X50_TEMP_SENSITIVITY    340
#define MPU9X50_TEMP_OFFSET         35
#elif defined(MODULE_MPU9250)
#define MPU9X50_TEMP_SENSITIVITY    333.87
#define MPU9X50_TEMP_OFFSET         21
#else
#error "MPU9X50 DRIVER not selected or supported"
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MPU9X50_INTERNAL_H */
