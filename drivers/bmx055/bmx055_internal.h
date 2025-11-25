/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmx055
 *
 * @{
 * @file
 * @brief       Definitions for the bmx055 device
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef BMX055_INTERNAL_H
#define BMX055_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    BMX055 magnetometer registers
 * @{
 */
#define REG_MAG_CHIPID      (0x40U)
#define REG_MAG_CHIPID_VAL  (0x32U)
#define REG_MAG_OPMODE      (0x4CU)
#define REG_MAG_PWRCTRL     (0x4BU)
#define REG_MAG_DATA        (0x42U)
/** @} */

/**
 * @name    BMX055 magnetometer bitfields and values
 * @{
 */
#define BIT_MAG_PWRCTRL_VAL (0x01U)
#define BIT_MAG_DATARDY     (0x01U)
/** @} */

/**
 * @name    BMX055 accelerometer registers and values
 * @{
 */
#define REG_ACC_CHIPID      (0x00U)
#define REG_ACC_CHIPID_VAL  (0xFAU)
#define REG_ACC_SHDW        (0x13U)
#define REG_ACC_SHDW_ENABLE (0x00U)
#define REG_ACC_DATA        (0x02U)
/** @} */

/**
 * @name    BMX055 accelerometer range register and values
 * @{
 */
#define REG_ACC_RANGE       (0x0FU)
#define BIT_ACC_RANGE_2G    (0x03U)
#define BIT_ACC_RANGE_4G    (0x05U)
#define BIT_ACC_RANGE_8G    (0x08U)
#define BIT_ACC_RANGE_16G   (0x0CU)
/** @} */

/**
 * @name    BMX055 gyroscope registers and values
 * @{
 */
#define REG_GYRO_CHIPID     (0x00U)
#define REG_GYRO_CHIPID_VAL (0x0FU)
#define REG_GYRO_SCALE      (0x0FU)
#define REG_GYRO_SHDW       (0x13U)
#define REG_GYRO_SHDW_EN    (0x00U)
#define REG_GYRO_PWRMD      (0x11U)
#define REG_GYRO_PWRMD_NORM (0x00U)
#define REG_GYRO_DATA       (0x02U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BMX055_INTERNAL_H */
/** @} */
