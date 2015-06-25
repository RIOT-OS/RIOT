/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lsm6ds3
 *
 * @{
 *
 * @file
 * @brief       Definitions for the LSM6DS3 3D accelerometer/gyroscope
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 */

#ifndef LSM6DS3_INTERNAL_H
#define LSM6DS3_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LSM6DS3 Identification number of the chip
 * @{
 */
#define LSM6DS_CHIP_ID                    (0x69)
/** @} */

#define LSM6DS3_FUNC_CFG_ACCESS           (0x01)
#define LSM6DS3_SENSOR_SYNC_TIME_FRAME    (0x04)

/**
 * @name LSM6DS3 fifo control registers
 * @{
 */
#define LSM6DS3_FIFO_CTRL1                (0x06)
#define LSM6DS3_FIFO_CTRL2                (0x07)
#define LSM6DS3_FIFO_CTRL3                (0x08)
#define LSM6DS3_FIFO_CTRL4                (0x09)
#define LSM6DS3_FIFO_CTRL5                (0x0A)
/** @} */

/**
 * @name Masks for the LSM6DS3 FIFO_CTRL5 register
 * @{
 */
#define LSM6DS3_FIFO_CTRL5_CONTINOUS_MODE (0x06)
/** @} */

#define LSM6DS3_ORIENT_CFG_G              (0x0B)
#define LSM6DS3_INT1_CTRL                 (0x0D)
#define LSM6DS3_INT2_CTRL                 (0x0E)
#define LSM6DS3_WHO_AM_I_REG              (0x0F)

/**
 * @name LSM6DS3 accelerometer/gyroscope registers
 * @{
 */
#define LSM6DS3_CTRL1_XL                  (0x10)
#define LSM6DS3_CTRL2_G                   (0x11)
#define LSM6DS3_CTRL3_C                   (0x12)
#define LSM6DS3_CTRL4_C                   (0x13)
#define LSM6DS3_CTRL5_C                   (0x14)
#define LSM6DS3_CTRL6_C                   (0x15)
#define LSM6DS3_CTRL7_G                   (0x16)
#define LSM6DS3_CTRL8_XL                  (0x17)
#define LSM6DS3_CTRL9_XL                  (0x18)
#define LSM6DS3_CTRL10_C                  (0x19)
/** @} */


/**
 * @name Masks for the LSM6DS3 CTRL1_XL register
 * @{
 */
#define LSM6DS3_MASK_CTRL1_XL_FS          (0x0C)
#define LSM6DS3_MASK_CTRL1_XL_ODR         (0xF0)
#define LSM6DS3_MASK_CTRL1_XL_ODR_PDOWN   (0x00)
#define LSM6DS3_MASK_CTRL1_XL_ODR_13HZ    (0x10)
#define LSM6DS3_MASK_CTRL1_XL_ODR_26HZ    (0x20)
#define LSM6DS3_MASK_CTRL1_XL_ODR_52HZ    (0x30)
#define LSM6DS3_MASK_CTRL1_XL_ODR_104HZ   (0x40)
#define LSM6DS3_MASK_CTRL1_XL_ODR_208HZ   (0x50)
#define LSM6DS3_MASK_CTRL1_XL_ODR_416HZ   (0x60)
#define LSM6DS3_MASK_CTRL1_XL_ODR_833HZ   (0x70)
#define LSM6DS3_MASK_CTRL1_XL_ODR_1660HZ  (0x80)
#define LSM6DS3_MASK_CTRL1_XL_ODR_3330HZ  (0x90)
#define LSM6DS3_MASK_CTRL1_XL_ODR_6660HZ  (0xA0)
#define LSM6DS3_MASK_CTRL1_XL_FS_2G       (0x00)
#define LSM6DS3_MASK_CTRL1_XL_FS_16G      (0x01)
#define LSM6DS3_MASK_CTRL1_XL_FS_4G       (0x02)
#define LSM6DS3_MASK_CTRL1_XL_FS_8G       (0x03)
#define LSM6DS3_MASK_CTRL1_XL_BW_400HZ    (0x00)
#define LSM6DS3_MASK_CTRL1_XL_BW_200HZ    (0x01)
#define LSM6DS3_MASK_CTRL1_XL_BW_100HZ    (0x02)
#define LSM6DS3_MASK_CTRL1_XL_BW_50HZ     (0x03)
/** @} */

/**
 * @name Masks for the LSM6DS3 CTRL2_G registers
 * @{
 */
#define LSM6DS3_MASK_CTRL2_G_FS           (0x0E)
#define LSM6DS3_MASK_CTRL2_G_ODR          (0xF0)
#define LSM6DS3_MASK_CTRL2_G_ODR_PDOWN    (0xF0)
#define LSM6DS3_MASK_CTRL2_G_ODR_13HZ     (0x10)
#define LSM6DS3_MASK_CTRL2_G_ODR_26HZ     (0x20)
#define LSM6DS3_MASK_CTRL2_G_ODR_52HZ     (0x30)
#define LSM6DS3_MASK_CTRL2_G_ODR_104HZ    (0x40)
#define LSM6DS3_MASK_CTRL2_G_ODR_208HZ    (0x50)
#define LSM6DS3_MASK_CTRL2_G_ODR_416HZ    (0x60)
#define LSM6DS3_MASK_CTRL2_G_ODR_833HZ    (0x70)
#define LSM6DS3_MASK_CTRL2_G_ODR_1660HZ   (0x80)
#define LSM6DS3_MASK_CTRL2_G_FS_125DPS    (0x02)
#define LSM6DS3_MASK_CTRL2_G_FS_245DPS    (0x00)
#define LSM6DS3_MASK_CTRL2_G_FS_500DPS    (0x04)
#define LSM6DS3_MASK_CTRL2_G_FS_1000DPS   (0x08)
#define LSM6DS3_MASK_CTRL2_G_FS_2000DPS   (0x0C)
/** @} */

/**
 * @name Masks for the LSM6DS3 CTRL3_C register
 * @{
 */
#define LSM6DS3_CTRL3_C_BOOT              (0x80)
#define LSM6DS3_CTRL3_C_BDU               (0x40)
/** @} */

/**
 * @name Masks for the LSM6DS3 CTRL4_C register
 * @{
 */
#define LSM6DS3_CTRL4_C_SLEEP_G           (0x40)
#define LSM6DS3_CTRL4_C_TEMP_EN           (0x10)
/** @} */

/**
 * @name Masks for the LSM6DS3 CTRL9_XL register
 * @{
 */
#define LSM6DS3_CTRL9_XL_Z_EN             (0x20)
#define LSM6DS3_CTRL9_XL_Y_EN             (0x10)
#define LSM6DS3_CTRL9_XL_X_EN             (0x08)
#define LSM6DS3_CTRL9_XL_PEDO_RST_STEP    (0x02)
/** @}*/

/**
 * @name Masks for the LSM6DS3 CTRL9_XL register
 * @{
 */
#define LSM6DS3_CTRL10_G_Z_EN             (0x20)
#define LSM6DS3_CTRL10_G_Y_EN             (0x10)
#define LSM6DS3_CTRL10_G_X_EN             (0x08)
#define LSM6DS3_CTRL10_G_PEDO_RST_STEP    (0x02)
/** @}*/

/**
 * @name LSM6DS3 master configuration register
 * @{
 */
#define LSM6DS3_MASTER_CONFIG             (0x1A)
/** @} */

#define LSM6DS3_WAKE_UP_SRC               (0x1B)
#define LSM6DS3_TAP_SRC                   (0x1C)
#define LSM6DS3_D6D_SRC                   (0x1D)

/**
 * @name LSM6DS3 status register
 * @{
 */
#define LSM6DS3_STATUS_REG                (0x1E)
/** @} */

/**
 * @name LSM6DS3 masks for status register
 * @{
 */
#define LSM6DS3_MASK_STATUS_REG_EV_BOOT   (0x08)
#define LSM6DS3_MASK_STATUS_REG_XLDA      (0x01)
#define LSM6DS3_MASK_STATUS_REG_GDA       (0x02)
#define LSM6DS3_MASK_STATUS_REG_TDA       (0x04)
/** }@ */

/**
 * @name LSM6DS3 temperature registers
 * @{
 */
#define LSM6DS3_OUT_TEMP_L                (0x20)
#define LSM6DS3_OUT_TEMP_H                (0x21)
/** @} */

/**
 * @name LSM6DS3 ouput register of the accelerometer
 * @{
 */
#define LSM6DS3_OUT_X_L_XL                (0x28)
#define LSM6DS3_OUT_X_H_XL                (0x29)
#define LSM6DS3_OUT_Y_L_XL                (0x2A)
#define LSM6DS3_OUT_Y_H_XL                (0x2B)
#define LSM6DS3_OUT_Z_L_XL                (0x2C)
#define LSM6DS3_OUT_Z_H_XL                (0x2D)
/** @} */

/**
 * @name LSM6DS3 output register of the gyroscope
 * @{
 */
#define LSM6DS3_OUT_X_L_G                 (0x22)
#define LSM6DS3_OUT_X_H_G                 (0x23)
#define LSM6DS3_OUT_Y_L_G                 (0x24)
#define LSM6DS3_OUT_Y_H_G                 (0x25)
#define LSM6DS3_OUT_Z_L_G                 (0x26)
#define LSM6DS3_OUT_Z_H_G                 (0x27)
/** @} */

#define LSM6DS3_SENSORHUB1_REG            (0x2E)
#define LSM6DS3_SENSORHUB2_REG            (0x2F)
#define LSM6DS3_SENSORHUB3_REG            (0x30)
#define LSM6DS3_SENSORHUB4_REG            (0x31)
#define LSM6DS3_SENSORHUB5_REG            (0x32)
#define LSM6DS3_SENSORHUB6_REG            (0x33)
#define LSM6DS3_SENSORHUB7_REG            (0x34)
#define LSM6DS3_SENSORHUB8_REG            (0x35)
#define LSM6DS3_SENSORHUB9_REG            (0x36)
#define LSM6DS3_SENSORHUB10_REG           (0x37)
#define LSM6DS3_SENSORHUB11_REG           (0x38)
#define LSM6DS3_SENSORHUB12_REG           (0x39)

/**
 * @name LSM6DS3 fifo status registers
 * @{
 */
#define LSM6DS3_FIFO_STATUS1              (0x3A)
#define LSM6DS3_FIFO_STATUS2              (0x3B)
#define LSM6DS3_FIFO_STATUS3              (0x3C)
#define LSM6DS3_FIFO_STATUS4              (0x3D)
/** @}*/

/**
 * @name LSM6DS3 fifo out registers
 * @{
 */
#define LSM6DS3_FIFO_OUT_L                (0x3E)
#define LSM6DS3_FIFO_OUT_H                (0x3F)
/** @} */

#define LSM6DS3_TIMESTAMP0_REG            (0x40)
#define LSM6DS3_TIMESTAMP1_REG            (0x41)
#define LSM6DS3_TIMESTAMP2_REG            (0x42)

#define LSM6DS3_STEP_TIMESTAMP_L          (0x49)
#define LSM6DS3_STEP_TIMESTAMP_H          (0x4A)

#define LSM6DS3_STEP_COUNTER_L            (0x4B)
#define LSM6DS3_STEP_COUNTER_H            (0x4C)

#define LSM6DS3_SENSORHUB13_REG           (0x4D)
#define LSM6DS3_SENSORHUB14_REG           (0x4E)
#define LSM6DS3_SENSORHUB15_REG           (0x4F)
#define LSM6DS3_SENSORHUB16_REG           (0x50)
#define LSM6DS3_SENSORHUB17_REG           (0x51)
#define LSM6DS3_SENSORHUB18_REG           (0x52)

#define LSM6DS3_FUNC_SRC                  (0x53)

#define LSM6DS3_TAP_CFG                   (0x58)
#define LSM6DS3_TAP_THS_6D                (0x59)

#define LSM6DS3_INT_DUR2                  (0x5A)

#define LSM6DS3_WAKE_UP_THS               (0x5B)
#define LSM6DS3_WAKE_UP_DUR               (0x5C)

#define LSM6DS3_FREE_FALL                 (0x5D)

#define LSM6DS3_MD1_CFG                   (0x5E)
#define LSM6DS3_MD2_CFG                   (0x5F)

#define LSM6DS3_OUT_MAG_RAW_X_L           (0x66)
#define LSM6DS3_OUT_MAG_RAW_X_H           (0x67)
#define LSM6DS3_OUT_MAG_RAW_Y_L           (0x68)
#define LSM6DS3_OUT_MAG_RAW_Y_H           (0x69)
#define LSM6DS3_OUT_MAG_RAW_Z_L           (0x6A)
#define LSM6DS3_OUT_MAG_RAW_Z_H           (0x6B)

#ifdef __cplusplus
}
#endif

#endif /* LSM6DS3_INTERNAL_H */
/** @} */
