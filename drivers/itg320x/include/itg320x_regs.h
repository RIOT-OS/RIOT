/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_itg320x
 * @brief       Register definitions for InvenSense ITG320X 3-axis gyroscope
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef ITG320X_REGS_H
#define ITG320X_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses
 * @{
 */
#define ITG320X_REG_WHO_AM_I        (0x00)
#define ITG320X_REG_SMPLRT_DIV      (0x15)
#define ITG320X_REG_DLPFS           (0x16)
#define ITG320X_REG_INT_CFG         (0x17)
#define ITG320X_REG_INT_STATUS      (0x1a)
#define ITG320X_REG_TEMP_OUT_H      (0x1b)
#define ITG320X_REG_TEMP_OUT_L      (0x1c)
#define ITG320X_REG_GYRO_XOUT_H     (0x1d)
#define ITG320X_REG_GYRO_XOUT_L     (0x1e)
#define ITG320X_REG_GYRO_YOUT_H     (0x1f)
#define ITG320X_REG_GYRO_YOUT_L     (0x20)
#define ITG320X_REG_GYRO_ZOUT_H     (0x21)
#define ITG320X_REG_GYRO_ZOUT_L     (0x22)
#define ITG320X_REG_PWR_MGM         (0x3e)
/** @} */

/**
 * @name    Register structure definitions
 * @{
 */
#define ITG320X_REG_DLPFS_FS_SEL        (0x18)  /**< ITG320X_REG_DLPFS<4:3> */
#define ITG320X_REG_DLPFS_FS_SEL_VAL    (0x18)  /**< ITG320X_REG_DLPFS<4:3> = 3 */
#define ITG320X_REG_DLPFS_DLPF_CFG      (0x07)  /**< ITG320X_REG_DLPFS<2:0> */

#define ITG320X_REG_INT_CFG_ACTL        (0x80)  /**< ITG320X_REG_INT_CFG<7> */
#define ITG320X_REG_INT_CFG_OPEN        (0x40)  /**< ITG320X_REG_INT_CFG<6> */
#define ITG320X_REG_INT_CFG_LATCH_INT   (0x20)  /**< ITG320X_REG_INT_CFG<5> */
#define ITG320X_REG_INT_CFG_ANY_RDY_CLR (0x10)  /**< ITG320X_REG_INT_CFG<4> */
#define ITG320X_REG_INT_CFG_ITG_RDY_EN  (0x04)  /**< ITG320X_REG_INT_CFG<2> */
#define ITG320X_REG_INT_CFG_RAW_RDY_EN  (0x01)  /**< ITG320X_REG_INT_CFG<0> */

#define ITG320X_REG_INT_STATUS_ITG_RDY  (0x04)  /**< ITG320X_REG_INT_STATUS<2> */
#define ITG320X_REG_INT_STATUS_RAW_RDY  (0x01)  /**< ITG320X_REG_INT_STATUS<0> */

#define ITG320X_REG_PWR_MGM_H_RESET     (0x80)  /**< ITG320X_REG_PWR_MGM<7> */
#define ITG320X_REG_PWR_MGM_SLEEP       (0x40)  /**< ITG320X_REG_PWR_MGM<6> */
#define ITG320X_REG_PWR_MGM_STBY_XG     (0x20)  /**< ITG320X_REG_PWR_MGM<5> */
#define ITG320X_REG_PWR_MGM_STBY_YG     (0x10)  /**< ITG320X_REG_PWR_MGM<4> */
#define ITG320X_REG_PWR_MGM_STBY_ZG     (0x08)  /**< ITG320X_REG_PWR_MGM<3> */
#define ITG320X_REG_PWR_MGM_CLK_SEL     (0x07)  /**< ITG320X_REG_PWR_MGM<2:0> */
/** @} */
#ifdef __cplusplus
}
#endif

#endif /* ITG320X_REGS_H */
