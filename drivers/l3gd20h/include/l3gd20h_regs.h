/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l3gd20h
 * @brief       Register definitions for the L3GD20H 3-axis digital gyroscope
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef L3GD20H_REGS_H
#define L3GD20H_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses
 * @{
 */
#define L3GD20H_REG_WHO_AM_I      (0x0f)
#define L3GD20H_REG_CTRL1         (0x20)
#define L3GD20H_REG_CTRL2         (0x21)
#define L3GD20H_REG_CTRL3         (0x22)
#define L3GD20H_REG_CTRL4         (0x23)
#define L3GD20H_REG_CTRL5         (0x24)
#define L3GD20H_REG_REFERENCE     (0x25)
#define L3GD20H_REG_OUT_TEMP      (0x26)
#define L3GD20H_REG_STATUS        (0x27)
#define L3GD20H_REG_OUT_X_L       (0x28)
#define L3GD20H_REG_OUT_X_H       (0x29)
#define L3GD20H_REG_OUT_Y_L       (0x2a)
#define L3GD20H_REG_OUT_Y_H       (0x2b)
#define L3GD20H_REG_OUT_Z_L       (0x2c)
#define L3GD20H_REG_OUT_Z_H       (0x2d)
#define L3GD20H_REG_FIFO_CTRL     (0x2e)
#define L3GD20H_REG_FIFO_SRC      (0x2f)
#define L3GD20H_REG_IG_CFG        (0x30)
#define L3GD20H_REG_IG_SRC        (0x31)
#define L3GD20H_REG_IG_THS_XH     (0x32)
#define L3GD20H_REG_IG_THS_XL     (0x33)
#define L3GD20H_REG_IG_THS_YH     (0x34)
#define L3GD20H_REG_IG_THS_YL     (0x35)
#define L3GD20H_REG_IG_THS_ZH     (0x36)
#define L3GD20H_REG_IG_THS_ZL     (0x37)
#define L3GD20H_REG_IG_DURATION   (0x38)
#define L3GD20H_REG_LOW_ODR       (0x39)
/** @} */

/**
 * @name    Register structure definitions
 * @{
 */
#define L3GD20H_ZYXOR             (0x80)  /**< L3GD20H_REG_STATUS<7> */
#define L3GD20H_ZOR               (0x40)  /**< L3GD20H_REG_STATUS<6> */
#define L3GD20H_YOR               (0x20)  /**< L3GD20H_REG_STATUS<5> */
#define L3GD20H_XOR               (0x10)  /**< L3GD20H_REG_STATUS<4> */
#define L3GD20H_ZYXDA             (0x08)  /**< L3GD20H_REG_STATUS<3> */
#define L3GD20H_ZDA               (0x04)  /**< L3GD20H_REG_STATUS<2> */
#define L3GD20H_YDA               (0x02)  /**< L3GD20H_REG_STATUS<1> */
#define L3GD20H_XDA               (0x01)  /**< L3GD20H_REG_STATUS<0> */

#define L3GD20H_ANY_DATA_READY    (0x07)  /**< L3GD20H_REG_STATUS<2:0> */

#define L3GD20H_ODR               (0xc0)  /**< L3GD20H_REG_CTRL1<7:6> */
#define L3GD20H_BW                (0x30)  /**< L3GD20H_REG_CTRL1<5:4> */
#define L3GD20H_ODR_BW            (0xf0)  /**< L3GD20H_REG_CTRL1<7:4> */
#define L3GD20H_POWER_MODE        (0x08)  /**< L3GD20H_REG_CTRL1<3> */
#define L3GD20H_Z_ENABLED         (0x04)  /**< L3GD20H_REG_CTRL1<2> */
#define L3GD20H_Y_ENABLED         (0x02)  /**< L3GD20H_REG_CTRL1<1> */
#define L3GD20H_X_ENABLED         (0x01)  /**< L3GD20H_REG_CTRL1<0> */
#define L3GD20H_XYZ_ENABLED       (0x07)  /**< L3GD20H_REG_CTRL1<2:0> */

#define L3GD20H_HPF_MODE          (0x30)  /**< L3GD20H_REG_CTRL2<5:4> */
#define L3GD20H_HPF_CUTOFF        (0x0f)  /**< L3GD20H_REG_CTRL2<3:0> */

#define L3GD20H_INT1_IG           (0x80)  /**< L3GD20H_REG_CTRL3<7> */
#define L3GD20H_INT1_BOOT         (0x40)  /**< L3GD20H_REG_CTRL3<6> */
#define L3GD20H_HL_ACTIVE         (0x20)  /**< L3GD20H_REG_CTRL3<5> */
#define L3GD20H_PP_OD             (0x10)  /**< L3GD20H_REG_CTRL3<4> */
#define L3GD20H_INT2_DRDY         (0x08)  /**< L3GD20H_REG_CTRL3<3> */
#define L3GD20H_INT2_FTH          (0x04)  /**< L3GD20H_REG_CTRL3<2> */
#define L3GD20H_INT2_ORUN         (0x02)  /**< L3GD20H_REG_CTRL3<1> */
#define L3GD20H_INT2_EMPTY        (0x01)  /**< L3GD20H_REG_CTRL3<0> */

#define L3GD20H_BLOCK_DATA_UPDATE (0x80)  /**< L3GD20H_REG_CTRL4<7> */
#define L3GD20H_BIG_LITTLE_ENDIAN (0x40)  /**< L3GD20H_REG_CTRL4<6> */
#define L3GD20H_FULL_SCALE        (0x30)  /**< L3GD20H_REG_CTRL4<5:4> */

#define L3GD20H_BOOT              (0x80)  /**< L3GD20H_REG_CTRL5<7> */
#define L3GD20H_FIFO_EN           (0x40)  /**< L3GD20H_REG_CTRL5<6> */
#define L3GD20H_STOP_ON_FTH       (0x20)  /**< L3GD20H_REG_CTRL5<5> */
#define L3GD20H_HP_ENABLED        (0x10)  /**< L3GD20H_REG_CTRL5<4> */
#define L3GD20H_IG_SEL            (0x0c)  /**< L3GD20H_REG_CTRL5<3:2> */
#define L3GD20H_OUT_SEL           (0x03)  /**< L3GD20H_REG_CTRL5<1:0> */

#define L3GD20H_FIFO_MODE         (0xe0)  /**< L3GD20H_REG_FIFO_CTRL<7:5> */
#define L3GD20H_FIFO_THRESH       (0x1f)  /**< L3GD20H_REG_FIFO_CTRL<4:0> */

#define L3GD20H_FIFO_THS          (0x80)  /**< L3GD20H_REG_FIFO_SRC<7> */
#define L3GD20H_FIFO_OVR          (0x40)  /**< L3GD20H_REG_FIFO_SRC<6> */
#define L3GD20H_FIFO_EMPTY        (0x20)  /**< L3GD20H_REG_FIFO_SRC<5> */
#define L3GD20H_FIFO_FFS          (0x1f)  /**< L3GD20H_REG_FIFO_SRC<4:0> */

#define L3GD20H_INT1_AND_OR       (0x80)  /**< L3GD20H_REG_IG_CFG<7> */
#define L3GD20H_INT1_LATCH        (0x40)  /**< L3GD20H_REG_IG_CFG<6> */
#define L3GD20H_INT1_Z_HIGH       (0x20)  /**< L3GD20H_REG_IG_CFG<5>, L3GD20H_REG_IG_SRC<5> */
#define L3GD20H_INT1_Z_LOW        (0x10)  /**< L3GD20H_REG_IG_CFG<4>, L3GD20H_REG_IG_SRC<4> */
#define L3GD20H_INT1_Y_HIGH       (0x08)  /**< L3GD20H_REG_IG_CFG<3>, L3GD20H_REG_IG_SRC<3> */
#define L3GD20H_INT1_Y_LOW        (0x04)  /**< L3GD20H_REG_IG_CFG<2>, L3GD20H_REG_IG_SRC<2> */
#define L3GD20H_INT1_X_HIGH       (0x02)  /**< L3GD20H_REG_IG_CFG<1>, L3GD20H_REG_IG_SRC<1> */
#define L3GD20H_INT1_X_LOW        (0x01)  /**< L3GD20H_REG_IG_CFG<0>, L3GD20H_REG_IG_SRC<0> */

#define L3GD20H_INT1_ACTIVE       (0x40)  /**< L3GD20H_REG_IG_SRC<7> */

#define L3GD20H_INT1_WAIT         (0x80)  /**< L3GD20H_REG_IG_DURATION<7> */
#define L3GD20H_INT1_DURATION     (0x3f)  /**< L3GD20H_REG_IG_DURATION<6:0> */

#define L3GD20H_DRDY_HL           (0x20)  /**< L3GD20H_REG_LOW_ODR<5> */
#define L3GD20H_SW_RESET          (0x04)  /**< L3GD20H_REG_LOW_ODR<2> */
#define L3GD20H_LOW_ODR           (0x01)  /**< L3GD20H_REG_LOW_ODR<0> */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* L3GD20H_REGS_H */
