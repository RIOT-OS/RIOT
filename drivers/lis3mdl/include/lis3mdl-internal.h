/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lis3mdl
 *
 * @{
 *
 * @file
 * @brief       Definitions for the LIS3MDL 3-axis magnetometer
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 */

#ifndef LIS3MDL_INTERNAL_H
#define LIS3MDL_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LIS3DML chip identification
 * @{
 */
#define LIS3MDL_CHIP_ID                   (0x3D)
#define LIS3MDL_CHIP_ADDR                 (0x1C)
/** @} */

/**
 * @name LIS3DML device identification register
 * @{
 */
#define LIS3DML_WHO_AM_I_REG              (0x0F)
/**  @} */

/**
 * @name LIS3DML controll register
 * @{
 */
#define LIS3MDL_CTRL_REG1                 (0x20)
#define LIS3MDL_CTRL_REG2                 (0x21)
#define LIS3MDL_CTRL_REG3                 (0x22)
#define LIS3MDL_CTRL_REG4                 (0x23)
#define LIS3MDL_CTRL_REG5                 (0x24)
/**  @} */

/**
 * @name LIS3DML masks for CTRL_REG1
 * @{
 */
#define LIS3MDL_MASK_REG1_TEMP_EN         (0x80)
/**  @} */

/**
 * @name LIS3DML masks for CTRL_REG2
 * @{
 */
#define LIS3MDL_MASK_REG2_REBOOT          (0x06)
#define LIS3MDL_MASK_REG2_SOFT_RST        (0x04)
/**  @} */

/**
 * @name LIS3DML masks for CTRL_REG3
 * @{
 */
#define LIS3MDL_MASK_REG3_LOW_POWER_EN    (0x02)
#define LIS3MDL_MASK_REG3_CONT_CONV_MODE  (0x00)
#define LIS3MDL_MASK_REG3_Z_LOW_POWER     (0x00)
#define LIS3MDL_MASK_REG3_Z_MEDIUM_POWER  (0x04)
/**  @} */

/**
 * @name LIS3DML masks for CTRL_REG5
 * @{
 */
#define LIS3MDL_MASK_REG5_BDU             (0x40)
#define LIS3MDL_MASK_REG5_BDU_OFF         (0x00)
/**  @} */

/**
 * @name LIS3DML status register
 * @{
 */
#define LIS3MDL_STATUS_REG                (0x27)
/**  @} */

/**
 * @name LIS3DML magnometer output register
 * @{
 */
#define LIS3MDL_OUT_X_L_REG               (0x28)
#define LIS3MDL_OUT_X_H_REG               (0x29)
#define LIS3MDL_OUT_Y_L_REG               (0x2A)
#define LIS3MDL_OUT_Y_H_REG               (0x2B)
#define LIS3MDL_OUT_Z_L_REG               (0x2C)
#define LIS3MDL_OUT_Z_H_REG               (0x2D)
/**  @} */

/**
 * @name LIS3DML temperature output register
 * @{
 */
#define LIS3MDL_TEMP_OUT_L_REG            (0x2E)
#define LIS3MDL_TEMP_OUT_H_REG            (0x2F)
/**  @} */

/**
 * @name LIS3DML interrupt register
 * @{
 */
#define LIS3MDL_INT_CFG_REG               (0x30)
#define LIS3MDL_INT_SRC_REG               (0x31)
#define LIS3MDL_INT_THS_L_REG             (0x32)
#define LIS3MDL_INT_THS_H_REG             (0x33)
/**  @} */

/**
 * @name LIS3MDL masks for interrupt cfg register
 * @{
 */
#define LIS3MDL_MASK_INT_CFG_XIEN         (0x80)
#define LIS3MDL_MASK_INT_CFG_YIEN         (0x40)
#define LIS3MDL_MASK_INT_CFG_ZIEN         (0x20)
#define LIS3MDL_MASK_INT_CFG_IEA          (0x04)
#define LIS3MDL_MASK_INT_CFG_LIR          (0x02)
#define LIS3MDL_MASK_INT_CFG_IEN          (0x01)
/** }@ */

/**
 * @name LIS3MDL masks for interrupt src register
 * @{
 */
#define LIS3MDL_MASK_INT_SRC_PTH_X        (0x80)
#define LIS3MDL_MASK_INT_SRC_PTH_Y        (0x40)
#define LIS3MDL_MASK_INT_SRC_PTH_Z        (0x20)
#define LIS3MDL_MASK_INT_SRC_NTH_X        (0x10)
#define LIS3MDL_MASK_INT_SRC_NTH_Y        (0x08)
#define LIS3MDL_MASK_INT_SRC_NTH_Z        (0x04)
#define LIS3MDL_MASK_INT_SRC_MROI         (0x02)
#define LIS3MDL_MASK_INT_SRC_INT          (0x01)

#ifdef __cplusplus
}
#endif

#endif /* LIS3MDL_INTERNAL_H */
/** @} */
