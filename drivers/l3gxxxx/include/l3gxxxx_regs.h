/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_l3gxxxx
 * @brief       Register definitions for ST L3Gxxxx 3-axis gyroscope sensor family
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses
 * @{
 */
#define L3GXXXX_REG_WHO_AM_I      (0x0f)    /**< Register address WHO_AM_I */
#define L3GXXXX_REG_CTRL1         (0x20)    /**< Register address CTRL1 */
#define L3GXXXX_REG_CTRL2         (0x21)    /**< Register address CTRL2 */
#define L3GXXXX_REG_CTRL3         (0x22)    /**< Register address CTRL3 */
#define L3GXXXX_REG_CTRL4         (0x23)    /**< Register address CTRL4 */
#define L3GXXXX_REG_CTRL5         (0x24)    /**< Register address CTRL5 */
#define L3GXXXX_REG_REFERENCE     (0x25)    /**< Register address REFERENCE */
#define L3GXXXX_REG_OUT_TEMP      (0x26)    /**< Register address OUT_TEMP */
#define L3GXXXX_REG_STATUS        (0x27)    /**< Register address STATUS */
#define L3GXXXX_REG_OUT_X_L       (0x28)    /**< Register address OUT_X_L */
#define L3GXXXX_REG_OUT_X_H       (0x29)    /**< Register address OUT_X_H */
#define L3GXXXX_REG_OUT_Y_L       (0x2a)    /**< Register address OUT_Y_L */
#define L3GXXXX_REG_OUT_Y_H       (0x2b)    /**< Register address OUT_Y_H */
#define L3GXXXX_REG_OUT_Z_L       (0x2c)    /**< Register address OUT_Z_L */
#define L3GXXXX_REG_OUT_Z_H       (0x2d)    /**< Register address OUT_Z_H */
#define L3GXXXX_REG_FIFO_CTRL     (0x2e)    /**< Register address FIFO_CTRL */
#define L3GXXXX_REG_FIFO_SRC      (0x2f)    /**< Register address FIFO_SRC */
#define L3GXXXX_REG_IG_CFG        (0x30)    /**< Register address IG_CFG */
#define L3GXXXX_REG_IG_SRC        (0x31)    /**< Register address IG_SRC */
#define L3GXXXX_REG_IG_THS_XH     (0x32)    /**< Register address IG_THS_XH */
#define L3GXXXX_REG_IG_THS_XL     (0x33)    /**< Register address IG_THS_XL */
#define L3GXXXX_REG_IG_THS_YH     (0x34)    /**< Register address IG_THS_YH */
#define L3GXXXX_REG_IG_THS_YL     (0x35)    /**< Register address IG_THS_YL */
#define L3GXXXX_REG_IG_THS_ZH     (0x36)    /**< Register address IG_THS_ZH */
#define L3GXXXX_REG_IG_THS_ZL     (0x37)    /**< Register address IG_THS_ZL */
#define L3GXXXX_REG_IG_DURATION   (0x38)    /**< Register address IG_DURATION */
#define L3GXXXX_REG_LOW_ODR       (0x39)    /**< Register address LOW_ODR */
/** @} */

/**
 * @name    Register structure definitions
 * @{
 */
#define L3GXXXX_ZYXOR             (0x80)  /**< L3GXXXX_REG_STATUS<7> */
#define L3GXXXX_ZOR               (0x40)  /**< L3GXXXX_REG_STATUS<6> */
#define L3GXXXX_YOR               (0x20)  /**< L3GXXXX_REG_STATUS<5> */
#define L3GXXXX_XOR               (0x10)  /**< L3GXXXX_REG_STATUS<4> */
#define L3GXXXX_ZYXDA             (0x08)  /**< L3GXXXX_REG_STATUS<3> */
#define L3GXXXX_ZDA               (0x04)  /**< L3GXXXX_REG_STATUS<2> */
#define L3GXXXX_YDA               (0x02)  /**< L3GXXXX_REG_STATUS<1> */
#define L3GXXXX_XDA               (0x01)  /**< L3GXXXX_REG_STATUS<0> */

#define L3GXXXX_ANY_DATA_READY    (0x07)  /**< L3GXXXX_REG_STATUS<2:0> */
#define L3GXXXX_ANY_DATA_READY_S  (0)     /**< L3GXXXX_REG_STATUS<2:0> */

#define L3GXXXX_ODR               (0xc0)  /**< L3GXXXX_REG_CTRL1<7:6> */
#define L3GXXXX_BW                (0x30)  /**< L3GXXXX_REG_CTRL1<5:4> */
#define L3GXXXX_ODR_BW            (0xf0)  /**< L3GXXXX_REG_CTRL1<7:4> */
#define L3GXXXX_POWER_MODE        (0x08)  /**< L3GXXXX_REG_CTRL1<3> */
#define L3GXXXX_Z_ENABLED         (0x04)  /**< L3GXXXX_REG_CTRL1<2> */
#define L3GXXXX_Y_ENABLED         (0x02)  /**< L3GXXXX_REG_CTRL1<1> */
#define L3GXXXX_X_ENABLED         (0x01)  /**< L3GXXXX_REG_CTRL1<0> */
#define L3GXXXX_XYZ_ENABLED       (0x07)  /**< L3GXXXX_REG_CTRL1<2:0> */

#define L3GXXXX_ODR_S             (6)     /**< L3GXXXX_REG_CTRL1<7:6> */
#define L3GXXXX_BW_S              (4)     /**< L3GXXXX_REG_CTRL1<5:4> */
#define L3GXXXX_ODR_BW_S          (4)     /**< L3GXXXX_REG_CTRL1<7:4> */
#define L3GXXXX_POWER_MODE_S      (3)     /**< L3GXXXX_REG_CTRL1<3> */
#define L3GXXXX_Z_ENABLED_S       (2)     /**< L3GXXXX_REG_CTRL1<2> */
#define L3GXXXX_Y_ENABLED_S       (1)     /**< L3GXXXX_REG_CTRL1<1> */
#define L3GXXXX_X_ENABLED_S       (1)     /**< L3GXXXX_REG_CTRL1<0> */
#define L3GXXXX_XYZ_ENABLED_S     (0)     /**< L3GXXXX_REG_CTRL1<2:0> */

#define L3GXXXX_EXTR_EN           (0x80)  /**< L3GXXXX_REG_CTRL2<7> */
#define L3GXXXX_LVL_EN            (0x40)  /**< L3GXXXX_REG_CTRL2<6> */
#define L3GXXXX_HPF_MODE          (0x30)  /**< L3GXXXX_REG_CTRL2<5:4> */
#define L3GXXXX_HPF_CUTOFF        (0x0f)  /**< L3GXXXX_REG_CTRL2<3:0> */

#define L3GXXXX_EXTR_EN_S         (7)     /**< L3GXXXX_REG_CTRL2<7> */
#define L3GXXXX_LVL_EN_S          (6)     /**< L3GXXXX_REG_CTRL2<6> */
#define L3GXXXX_HPF_MODE_S        (4)     /**< L3GXXXX_REG_CTRL2<5:4> */

#define L3GXXXX_INT1_IG           (0x80)  /**< L3GXXXX_REG_CTRL3<7> */
#define L3GXXXX_INT1_BOOT         (0x40)  /**< L3GXXXX_REG_CTRL3<6> */
#define L3GXXXX_HL_ACTIVE         (0x20)  /**< L3GXXXX_REG_CTRL3<5> */
#define L3GXXXX_PP_OD             (0x10)  /**< L3GXXXX_REG_CTRL3<4> */
#define L3GXXXX_INT2_DRDY         (0x08)  /**< L3GXXXX_REG_CTRL3<3> */
#define L3GXXXX_INT2_WTM          (0x04)  /**< L3GXXXX_REG_CTRL3<2> */
#define L3GXXXX_INT2_ORUN         (0x02)  /**< L3GXXXX_REG_CTRL3<1> */
#define L3GXXXX_INT2_EMPTY        (0x01)  /**< L3GXXXX_REG_CTRL3<0> */

#define L3GXXXX_INT1_IG_S         (7)     /**< L3GXXXX_REG_CTRL3<7> */
#define L3GXXXX_INT1_BOOT_S       (6)     /**< L3GXXXX_REG_CTRL3<6> */
#define L3GXXXX_HL_ACTIVE_S       (5)     /**< L3GXXXX_REG_CTRL3<5> */
#define L3GXXXX_PP_OD_S           (4)     /**< L3GXXXX_REG_CTRL3<4> */
#define L3GXXXX_INT2_DRDY_S       (3)     /**< L3GXXXX_REG_CTRL3<3> */
#define L3GXXXX_INT2_WTM_S        (2)     /**< L3GXXXX_REG_CTRL3<2> */
#define L3GXXXX_INT2_ORUN_S       (1)     /**< L3GXXXX_REG_CTRL3<1> */
#define L3GXXXX_INT2_EMPTY_S      (0)     /**< L3GXXXX_REG_CTRL3<0> */

#define L3GXXXX_BLOCK_DATA_UPDATE (0x80)  /**< L3GXXXX_REG_CTRL4<7> */
#define L3GXXXX_BIG_LITTLE_ENDIAN (0x40)  /**< L3GXXXX_REG_CTRL4<6> */
#define L3GXXXX_FULL_SCALE        (0x30)  /**< L3GXXXX_REG_CTRL4<5:4> */

#define L3GXXXX_FULL_SCALE_S      (4)     /**< L3GXXXX_REG_CTRL4<5:4> */

#define L3GXXXX_BOOT              (0x80)  /**< L3GXXXX_REG_CTRL5<7> */
#define L3GXXXX_FIFO_EN           (0x40)  /**< L3GXXXX_REG_CTRL5<6> */
#define L3GXXXX_STOP_ON_WTM       (0x20)  /**< L3GXXXX_REG_CTRL5<5> */
#define L3GXXXX_HP_ENABLED        (0x10)  /**< L3GXXXX_REG_CTRL5<4> */
#define L3GXXXX_IG_SEL            (0x0c)  /**< L3GXXXX_REG_CTRL5<3:2> */
#define L3GXXXX_OUT_SEL           (0x03)  /**< L3GXXXX_REG_CTRL5<1:0> */

#define L3GXXXX_BOOT_S            (7)     /**< L3GXXXX_REG_CTRL5<7> */
#define L3GXXXX_FIFO_EN_S         (6)     /**< L3GXXXX_REG_CTRL5<6> */
#define L3GXXXX_STOP_ON_WTM_S     (5)     /**< L3GXXXX_REG_CTRL5<5> */
#define L3GXXXX_HP_ENABLED_S      (4)     /**< L3GXXXX_REG_CTRL5<4> */
#define L3GXXXX_IG_SEL_S          (2)     /**< L3GXXXX_REG_CTRL5<3:2> */
#define L3GXXXX_OUT_SEL_S         (0)     /**< L3GXXXX_REG_CTRL5<1:0> */

#define L3GXXXX_FIFO_MODE         (0xe0)  /**< L3GXXXX_REG_FIFO_CTRL<7:5> */
#define L3GXXXX_FIFO_WATERMARK    (0x1f)  /**< L3GXXXX_REG_FIFO_CTRL<4:0> */

#define L3GXXXX_FIFO_MODE_S       (5)     /**< L3GXXXX_REG_FIFO_CTRL<7:5> */
#define L3GXXXX_FIFO_WATERMARK_S  (0)     /**< L3GXXXX_REG_FIFO_CTRL<4:0> */

#define L3GXXXX_FIFO_WTM          (0x80)  /**< L3GXXXX_REG_FIFO_SRC<7> */
#define L3GXXXX_FIFO_OVR          (0x40)  /**< L3GXXXX_REG_FIFO_SRC<6> */
#define L3GXXXX_FIFO_EMPTY        (0x20)  /**< L3GXXXX_REG_FIFO_SRC<5> */
#define L3GXXXX_FIFO_FFS          (0x1f)  /**< L3GXXXX_REG_FIFO_SRC<4:0> */

#define L3GXXXX_FIFO_WTM_S        (7)     /**< L3GXXXX_REG_FIFO_SRC<7> */
#define L3GXXXX_FIFO_OVR_S        (6)     /**< L3GXXXX_REG_FIFO_SRC<6> */
#define L3GXXXX_FIFO_EMPTY_S      (5)     /**< L3GXXXX_REG_FIFO_SRC<5> */
#define L3GXXXX_FIFO_FFS_S        (0)     /**< L3GXXXX_REG_FIFO_SRC<4:0> */

#define L3GXXXX_INT1_AND_OR       (0x80)  /**< L3GXXXX_REG_IG_CFG<7> */
#define L3GXXXX_INT1_LATCH        (0x40)  /**< L3GXXXX_REG_IG_CFG<6> */
#define L3GXXXX_INT1_Z_HIGH       (0x20)  /**< L3GXXXX_REG_IG_CFG<5>, L3GXXXX_REG_IG_SRC<5> */
#define L3GXXXX_INT1_Z_LOW        (0x10)  /**< L3GXXXX_REG_IG_CFG<4>, L3GXXXX_REG_IG_SRC<4> */
#define L3GXXXX_INT1_Y_HIGH       (0x08)  /**< L3GXXXX_REG_IG_CFG<3>, L3GXXXX_REG_IG_SRC<3> */
#define L3GXXXX_INT1_Y_LOW        (0x04)  /**< L3GXXXX_REG_IG_CFG<2>, L3GXXXX_REG_IG_SRC<2> */
#define L3GXXXX_INT1_X_HIGH       (0x02)  /**< L3GXXXX_REG_IG_CFG<1>, L3GXXXX_REG_IG_SRC<1> */
#define L3GXXXX_INT1_X_LOW        (0x01)  /**< L3GXXXX_REG_IG_CFG<0>, L3GXXXX_REG_IG_SRC<0> */

#define L3GXXXX_INT1_AND_OR_S     (7)     /**< L3GXXXX_REG_IG_CFG<7> */
#define L3GXXXX_INT1_LATCH_S      (6)     /**< L3GXXXX_REG_IG_CFG<6> */
#define L3GXXXX_INT1_Z_HIGH_S     (5)     /**< L3GXXXX_REG_IG_CFG<5>, L3GXXXX_REG_IG_SRC<5> */
#define L3GXXXX_INT1_Z_LOW_S      (4)     /**< L3GXXXX_REG_IG_CFG<4>, L3GXXXX_REG_IG_SRC<4> */
#define L3GXXXX_INT1_Y_HIGH_S     (3)     /**< L3GXXXX_REG_IG_CFG<3>, L3GXXXX_REG_IG_SRC<3> */
#define L3GXXXX_INT1_Y_LOW_S      (2)     /**< L3GXXXX_REG_IG_CFG<2>, L3GXXXX_REG_IG_SRC<2> */
#define L3GXXXX_INT1_X_HIGH_S     (1)     /**< L3GXXXX_REG_IG_CFG<1>, L3GXXXX_REG_IG_SRC<1> */
#define L3GXXXX_INT1_X_LOW_S      (0)     /**< L3GXXXX_REG_IG_CFG<0>, L3GXXXX_REG_IG_SRC<0> */

#define L3GXXXX_INT1_ACTIVE       (0x40)  /**< L3GXXXX_REG_IG_SRC<6> */

#define L3GXXXX_INT1_WAIT         (0x80)  /**< L3GXXXX_REG_IG_DURATION<7> */
#define L3GXXXX_INT1_DURATION     (0x3f)  /**< L3GXXXX_REG_IG_DURATION<6:0> */

#define L3GXXXX_INT1_WAIT_S       (7)     /**< L3GXXXX_REG_IG_DURATION<7> */
#define L3GXXXX_INT1_DURATION_S   (0)     /**< L3GXXXX_REG_IG_DURATION<6:0> */

#define L3GXXXX_DRDY_HL           (0x20)  /**< L3GXXXX_REG_LOW_ODR<5> */
#define L3GXXXX_SW_RESET          (0x04)  /**< L3GXXXX_REG_LOW_ODR<2> */
#define L3GXXXX_LOW_ODR           (0x01)  /**< L3GXXXX_REG_LOW_ODR<0> */

#define L3GXXXX_DRDY_HL_S         (5)     /**< L3GXXXX_REG_LOW_ODR<5> */
#define L3GXXXX_SW_RESET_S        (2)     /**< L3GXXXX_REG_LOW_ODR<2> */
#define L3GXXXX_LOW_ODR_S         (0)     /**< L3GXXXX_REG_LOW_ODR<0> */

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
