/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l3g4200d
 * @{
 *
 * @file
 * @brief       Definitions for the L3G4200D gyroscope
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef L3G4200D_REGS_H
#define L3G4200D_REGS_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Flag for reading multiple bytes
 */
#define L3G4200D_AUTOINC                0x80

/**
 * @name    L3G4200D register definitions
 * @{
 */
#define L3G4200D_REG_WHO_AM_I           0x0f
#define L3G4200D_REG_CTRL1              0x20
#define L3G4200D_REG_CTRL2              0x21
#define L3G4200D_REG_CTRL3              0x22
#define L3G4200D_REG_CTRL4              0x23
#define L3G4200D_REG_CTRL5              0x24
#define L3G4200D_REG_REF                0x25
#define L3G4200D_REG_OUT_TEMP           0x26
#define L3G4200D_REG_STATUS             0x27
#define L3G4200D_REG_OUT_X_L            0x28
#define L3G4200D_REG_OUT_X_H            0x29
#define L3G4200D_REG_OUT_Y_L            0x2a
#define L3G4200D_REG_OUT_Y_H            0x2b
#define L3G4200D_REG_OUT_Z_L            0x2c
#define L3G4200D_REG_OUT_Z_H            0x2d
#define L3G4200D_REG_FIFO_CTRL          0x2e
#define L3G4200D_REG_FIFO_SRC           0x2f
#define L3G4200D_REG_INT1_CFG           0x30
#define L3G4200D_REG_INT1_SRC           0x31
#define L3G4200D_REG_INT1_THS_XH        0x32
#define L3G4200D_REG_INT1_THS_XL        0x33
#define L3G4200D_REG_INT1_THS_YH        0x34
#define L3G4200D_REG_INT1_THS_YL        0x35
#define L3G4200D_REG_INT1_THS_ZL        0x36
#define L3G4200D_REG_INT1_THS_ZH        0x37
#define L3G4200D_REG_INT1_DURATION      0x38
/** @} */

/**
 * @name    CTRL1 bitfields
 * @{
 */
#define L3G4200D_CTRL1_PD               0x08
#define L3G4200D_CTRL1_ZEN              0x04
#define L3G4200D_CTRL1_YEN              0x02
#define L3G4200D_CTRL1_XEN              0x01
#define L3G4200D_CTRL1_ALLON            0x0f
#define L3G4200D_CTRL1_MODE_POS         (4)
/** @} */

/**
 * @name    CTRL4 bitfields
 * @{
 */
#define L3G4200D_CTRL4_BDU              0x80
#define L3G4200D_CTRL4_BLE              0x40
#define L3G4200D_CTRL4_FS1              0x20
#define L3G4200D_CTRL4_FS2              0x10
#define L3G4200D_CTRL4_ST1              0x04
#define L3G4200D_CTRL4_ST2              0x02
#define L3G4200D_CTRL4_SIM              0x01
#define L3G4200D_CTRL4_FS_POS           (4)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* L3G4200D_REGS_H */
