/*
 * Copyright (C) 2019 Wouter Horlings
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bno055
 *
 * @{
 * @file
 * @brief       Definitions for the bno055 device
 *
 * @author      Wouter Horlings <wouter@horlin.gs>
 */

#ifndef BNO055_INTERNAL_H
#define BNO055_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    BNO055 chip id registers and who am I values
 * @{
 */
#define BNO055_REG_CHIPID          (0x00U)  /**> chip id register */
#define BNO055_VAL_MCU_WHO_AM_I    (0xA0U)  /**> BNO055 MCU chip ID */
#define BNO055_VAL_ACC_WHO_AM_I    (0xFBU)  /**> Accelerometer chip ID */
#define BNO055_VAL_MAG_WHO_AM_I    (0x32U)  /**> Magnetometer chip ID */
#define BNO055_VAL_GYR_WHO_AM_I    (0x0FU)  /**> Gyroscope chip ID */
/** @} */

/**
 * @name    BNO055 operation mode registers and values
 * @{
 */
#define BNO055_REG_PAGEID          (0x07U)  /**> Register page register */
#define BNO055_REG_TEM_DAT         (0x34U)  /**> Temperature data [1] */
#define BNO055_REG_OPR_MOD         (0x3DU)  /**> Operation mode [1] */
#define BNO055_REG_PWR_MOD         (0x3EU)  /**> Power mode [1] */
#define BNO055_BIT_PAGEID_DEFAULT  (0x00U)  /**> Default value of pageid [1] */
/** @} */

/**
 * @name    BNO055 No-Fusion Data Register
 * @{
 */
#define BNO055_REG_ACC_DAT (0x08U)  /**> Acceleration Data [6] */
#define BNO055_REG_MAG_DAT (0x0EU)  /**> Magnetic field Data [6] */
#define BNO055_REG_GYR_DAT (0x14U)  /**> Rotation Data [6] */
/** @}*/

/**
 * @name    BNO055 No-Fusion configuration registers that are on register page 1
 * @{
 */
#define BNO055_REG_ACC_CNF (0x08U)  /**> Accelerometer Config [1] */
#define BNO055_REG_MAG_CNF (0x09U)  /**> Magnetometer Config [1] */
#define BNO055_REG_GYR_CNF (0x0AU)  /**> Gyroscope Config [2] */
/** @} */

/**
 * @name    BNO055 Fusion Data Registers
 * @{
 */
#define BNO055_REG_EUL_DAT (0x1AU)  /**> Euler representation data [6] */
#define BNO055_REG_QUA_DAT (0x20U)  /**> Quaternion representation data [8] */
#define BNO055_REG_LIA_DAT (0x28U)  /**> Linear Acceleration Data [6] */
#define BNO055_REG_GRV_DAT (0x2EU)  /**> Gravity Vector Data [6] */
#define BNO055_REG_CAL_DAT (0x35U)  /**> Calibration Status [1] */
/** @} */

/**
 * @name    BNO055 Unit select Registers and bit values
 * @{
 */
#define BNO055_REG_UNIT_SEL (0x3BU) /**> Register for output unit selection */
#define BNO055_BIT_ACC_MSS (0 << 0) /**> Unit acceleration data m/s2 */
#define BNO055_BIT_ACC_G   (1 << 0) /**> Unit acceleration data g (9.81m/s2) */
#define BNO055_BIT_GYR_DPS (0 << 1) /**> Unit gyroscope data deg/s  */
#define BNO055_BIT_GYR_RPS (1 << 1) /**> Unit gyroscope data rad/s */
#define BNO055_BIT_EUL_DEG (0 << 2) /**> Unit euler data deg  */
#define BNO055_BIT_EUL_RAD (1 << 2) /**> Unit euler data rad */
#define BNO055_BIT_TEMP_DC (0 << 4) /**> Unit Temperature data Celsius */
#define BNO055_BIT_TEMP_DF (1 << 4) /**> Unit Temperature data Fahrenheid */
/** @} */

/**
 * @name    BNO055 Scaling values for different unit options. Should divide by this value.
 * @{
 */
#define BNO055_DIV_SCALE_ACC_MSS (100.0)    /**> Acceleration Raw to m/s2 scaling */
#define BNO055_DIV_SCALE_ACC_G (1.0)        /**> Acceleration Raw to g (9.81m/s2) scaling */
#define BNO055_DIV_SCALE_GYR_RPS (900.0)    /**> Gyroscope Raw to rad/s scaling */
#define BNO055_DIV_SCALE_GYR_DPS (16.0)     /**> Gyroscope Raw to deg/s scaling */
#define BNO055_DIV_SCALE_EUL_RAD (900.0)    /**> Euler Data Raw to rad scaling */
#define BNO055_DIV_SCALE_EUL_DEG (16)       /**> Euler Data Raw to deg scaling */
#define BNO055_DIV_SCALE_QUAT_UN (16384.0)  /**> Quaternion data Raw to 0..1 (unit less) scaling */
#define BNO055_DIV_SCALE_TEMP_DC (1.0)      /**> Tempature Data Raw to Celsius scaling */
#define BNO055_DIV_SCALE_TEMP_DF (0.5)      /**> Tempature Data Raw to Fahrenheid scaling */
#define BNO055_DIV_SCALE_MAG (16.0)         /**> Magnetometer Data Raw to microtesla scaling */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BNO055_INTERNAL_H */
/** @} */
