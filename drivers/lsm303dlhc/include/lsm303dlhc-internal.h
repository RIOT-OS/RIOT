/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lsm303dlhc
 *
 * @{
 *
 * @file
 * @brief       Definitions for the LSM303DLHC 3D accelerometer/magnetometer
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef LSM303DLHC_INTERNAL_H
#define LSM303DLHC_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LSM303DLHC accelerometer registers
 * @{
 */
#define LSM303DLHC_REG_CTRL1_A              (0x20)
#define LSM303DLHC_REG_CTRL2_A              (0x21)
#define LSM303DLHC_REG_CTRL3_A              (0x22)
#define LSM303DLHC_REG_CTRL4_A              (0x23)
#define LSM303DLHC_REG_CTRL5_A              (0x24)
#define LSM303DLHC_REG_CTRL6_A              (0x25)
#define LSM303DLHC_REG_REFERENCE_A          (0x26)
#define LSM303DLHC_REG_STATUS_A             (0x27)
#define LSM303DLHC_REG_OUT_X_L_A            (0x28)
#define LSM303DLHC_REG_OUT_X_H_A            (0x29)
#define LSM303DLHC_REG_OUT_Y_L_A            (0x2a)
#define LSM303DLHC_REG_OUT_Y_H_A            (0x2b)
#define LSM303DLHC_REG_OUT_Z_L_A            (0x2c)
#define LSM303DLHC_REG_OUT_Z_H_A            (0x2d)
/** @} */

/**
 * @name Masks for the LSM303DLHC CTRL1_A register
 * @{
 */
#define LSM303DLHC_CTRL1_A_XEN              (0x01)
#define LSM303DLHC_CTRL1_A_YEN              (0x02)
#define LSM303DLHC_CTRL1_A_ZEN              (0x04)
#define LSM303DLHC_CTRL1_A_LOW_POWER        (0x08)
#define LSM303DLHC_CTRL1_A_POWEROFF         (0x00)
#define LSM303DLHC_CTRL1_A_1HZ              (0x10)
#define LSM303DLHC_CTRL1_A_10HZ             (0x20)
#define LSM303DLHC_CTRL1_A_25HZ             (0x30)
#define LSM303DLHC_CTRL1_A_50HZ             (0x40)
#define LSM303DLHC_CTRL1_A_100HZ            (0x50)
#define LSM303DLHC_CTRL1_A_200HZ            (0x60)
#define LSM303DLHC_CTRL1_A_400HZ            (0x70)
#define LSM303DLHC_CTRL1_A_1620HZ           (0x80)
#define LSM303DLHC_CTRL1_A_N1344HZ_L5376HZ  (0x90)
/** @} */

/**
 * @name Masks for the LSM303DLHC CTRL3_A register
 * @{
 */
#define LSM303DLHC_CTRL3_A_I1_CLICK         (0x80)
#define LSM303DLHC_CTRL3_A_I1_AOI1          (0x40)
#define LSM303DLHC_CTRL3_A_I1_AOI2          (0x20)
#define LSM303DLHC_CTRL3_A_I1_DRDY1         (0x10)
#define LSM303DLHC_CTRL3_A_I1_DRDY2         (0x80)
#define LSM303DLHC_CTRL3_A_I1_WTM           (0x40)
#define LSM303DLHC_CTRL3_A_I1_OVERRUN       (0x20)
#define LSM303DLHC_CTRL3_A_I1_NONE          (0x00)
/** @} */

/**
 * @name Masks for the LSM303DLHC CTRL4_A register
 * @{
 */
#define LSM303DLHC_CTRL4_A_BDU              (0x80)
#define LSM303DLHC_CTRL4_A_BLE              (0x40)
#define LSM303DLHC_CTRL4_A_SCALE_2G         (0x00)
#define LSM303DLHC_CTRL4_A_SCALE_4G         (0x10)
#define LSM303DLHC_CTRL4_A_SCALE_8G         (0x20)
#define LSM303DLHC_CTRL4_A_SCALE_16G        (0x30)
#define LSM303DLHC_CTRL4_A_HR               (0x04)
/** @} */

/**
 * @name Masks for the LSM303DLHC STATUS_A register
 * @{
 */
#define LSM303DLHC_STATUS_ZYXOR             (0x80)
#define LSM303DLHC_STATUS_ZOR               (0x40)
#define LSM303DLHC_STATUS_YOR               (0x20)
#define LSM303DLHC_STATUS_XOR               (0x10)
#define LSM303DLHC_STATUS_ZYXDA             (0x08)
#define LSM303DLHC_STATUS_ZDA               (0x04)
#define LSM303DLHC_STATUS_YDA               (0x02)
#define LSM303DLHC_STATUS_XDA               (0x01)
/** @} */

/**
 * @name Masks for the LSM303DLHC CTRL4_A register
 * @{
 */
#define LSM303DLHC_REG_CTRL5_A_BOOT     (0x80)
#define LSM303DLHC_REG_CTRL5_A_FIFO_EN  (0x40)
/** @} */

/**
 * @name LSM303DLHC magnetometer registers
 * @{
 */
#define LSM303DLHC_REG_CRA_M            (0x00)
#define LSM303DLHC_REG_CRB_M            (0x01)
#define LSM303DLHC_REG_MR_M             (0x02)
#define LSM303DLHC_REG_OUT_X_H_M        (0x03)
#define LSM303DLHC_REG_OUT_X_L_M        (0x04)
#define LSM303DLHC_REG_OUT_Y_H_M        (0x05)
#define LSM303DLHC_REG_OUT_Y_L_M        (0x06)
#define LSM303DLHC_REG_OUT_Z_H_M        (0x07)
#define LSM303DLHC_REG_OUT_Z_L_M        (0x08)
#define LSM303DLHC_REG_SR_M             (0x09)
#define LSM303DLHC_REG_TEMP_OUT_L       (0x32)
#define LSM303DLHC_REG_TEMP_OUT_H       (0x31)
/** @} */

/**
 * @name Masks for the LSM303DLHC CRA_REG_M register
 * @{
 */
#define LSM303DLHC_TEMP_EN              (0x80)
#define LSM303DLHC_TEMP_DIS             (0x00)

#define LSM303DLHC_TEMP_SAMPLE_0_75HZ   (0x00)
#define LSM303DLHC_TEMP_SAMPLE_1_5HZ    (0x04)
#define LSM303DLHC_TEMP_SAMPLE_3HZ      (0x08)
#define LSM303DLHC_TEMP_SAMPLE_7_5HZ    (0x0c)
#define LSM303DLHC_TEMP_SAMPLE_15HZ     (0x10)
#define LSM303DLHC_TEMP_SAMPLE_30HZ     (0x14)
#define LSM303DLHC_TEMP_SAMPLE_75HZ     (0x18)
#define LSM303DLHC_TEMP_SAMPLE_220HZ    (0x1c)
/** @} */

/**
 * @name Masks for the LSM303DLHC CRB_REG_M register
 * @{
 */
#define LSM303DLHC_GAIN_1               (0x20)
#define LSM303DLHC_GAIN_2               (0x40)
#define LSM303DLHC_GAIN_3               (0x60)
#define LSM303DLHC_GAIN_4               (0x80)
#define LSM303DLHC_GAIN_5               (0xa0)
#define LSM303DLHC_GAIN_6               (0xc0)
#define LSM303DLHC_GAIN_7               (0xe0)
/** @} */

/**
 * @name Masks for the LSM303DLHC MR_REG_M register
 * @{
 */
#define LSM303DLHC_MAG_MODE_CONTINUOUS  (0x00)
#define LSM303DLHC_MAG_MODE_SINGLE      (0x01)
#define LSM303DLHC_MAG_MODE_SLEEP       (0x02)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LSM303DLHC_INTERNAL_H */
