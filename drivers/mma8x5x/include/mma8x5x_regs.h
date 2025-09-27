/*
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mma8x5x
 * @{
 *
 * @file
 * @brief       Register definition for the MMA8x5x accelerometer driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses
 * @{
 */
#define MMA8X5X_STATUS              0x00 /**< Data or FIFO Status */
#define MMA8X5X_OUT_X_MSB           0x01 /**< [7:0] are 8 MSBs of X data */
#define MMA8X5X_OUT_X_LSB           0x02 /**< [7:4] are 4 LSBs of X data */
#define MMA8X5X_OUT_Y_MSB           0x03 /**< [7:0] are 8 MSBs of Y data */
#define MMA8X5X_OUT_Y_LSB           0x04 /**< [7:4] are 4 LSBs of Y data */
#define MMA8X5X_OUT_Z_MSB           0x05 /**< [7:0] are 8 MSBs of Z data */
#define MMA8X5X_OUT_Z_LSB           0x06 /**< [7:4] are 8 LSBs of Z data */
#define MMA8X5X_F_SETUP             0x09 /**< FIFO setup */
#define MMA8X5X_TRIG_CFG            0x0A /**< Map of FIFO data capture events */
#define MMA8X5X_SYSMOD              0x0B /**< Current System mode */
#define MMA8X5X_INT_SOURCE          0x0C /**< Interrupt status */
#define MMA8X5X_WHO_AM_I            0x0D /**< Device ID */
#define MMA8X5X_XYZ_DATA_CFG        0x0E /**< Dynamic Range Settings */
#define MMA8X5X_HP_FILTER_CUTOFF    0x0F /**< High-Pass Filter Selection */
#define MMA8X5X_PL_STATUS           0x10 /**< Landscape/Portrait orientation status */
#define MMA8X5X_PL_CFG              0x11 /**< Landscape/Portrait configuration */
#define MMA8X5X_PL_COUNT            0x12 /**< Landscape/Portrait debounce counter */
#define MMA8X5X_PL_BF_ZCOMP         0x13 /**< Back/Front, Z-Lock Trip threshold */
#define MMA8X5X_P_L_THS_REG         0x14 /**< Portrait/Landscape Threshold and Hysteresis */
#define MMA8X5X_FF_MT_CFG           0x15 /**< Freefall/Motion functional block configuration */
#define MMA8X5X_FF_MT_SRC           0x16 /**< Freefall/Motion event source register */
#define MMA8X5X_FF_MT_THS           0x17 /**< Freefall/Motion threshold register */
#define MMA8X5X_FF_MT_COUNT         0x18 /**< Freefall/Motion debounce counter */
#define MMA8X5X_TRANSIENT_CFG       0x1D /**< Transient functional block configuration */
#define MMA8X5X_TRANSIENT_SRC       0x1E /**< Transient event status register */
#define MMA8X5X_TRANSIENT_THS       0x1F /**< Transient event threshold */
#define MMA8X5X_TRANSIENT_COUNT     0x20 /**< Transient debounce counter */
#define MMA8X5X_PULSE_CFG           0x21 /**< Pulse enable configuration */
#define MMA8X5X_PULSE_SRC           0x22 /**< Pulse detection source */
#define MMA8X5X_PULSE_THSX          0x23 /**< X pulse threshold */
#define MMA8X5X_PULSE_THSY          0x24 /**< Y pulse threshold */
#define MMA8X5X_PULSE_THSZ          0x25 /**< Z pulse threshold */
#define MMA8X5X_PULSE_TMLT          0x26 /**< Time limit for pulse */
#define MMA8X5X_PULSE_LTCY          0x27 /**< Latency time for 2nd pulse */
#define MMA8X5X_PULSE_WIND          0x28 /**< Window time for 2nd pulse */
#define MMA8X5X_ASLP_COUNT          0x29 /**< Counter setting for Auto-SLEEP */
#define MMA8X5X_CTRL_REG1           0x2A /**< Data rates and modes setting */
#define MMA8X5X_CTRL_REG2           0x2B /**< Sleep Enable, OS modes, RST, ST */
#define MMA8X5X_CTRL_REG3           0x2C /**< Wake from Sleep, IPOL, PP_OD */
#define MMA8X5X_CTRL_REG4           0x2D /**< Interrupt enable register */
#define MMA8X5X_CTRL_REG5           0x2E /**< Interrupt pin (INT1/INT2) map */
#define MMA8X5X_OFF_X               0x2F /**< X-axis offset adjust */
#define MMA8X5X_OFF_Y               0x30 /**< Y-axis offset adjust */
#define MMA8X5X_OFF_Z               0x31 /**< Z-axis offset adjust */
/** @} */

/**
 * @brief   MMA8x5x register bitfields
 * @{
 */
#define MMA8X5X_STATUS_XDR                  (1 << 0)
#define MMA8X5X_STATUS_YDR                  (1 << 1)
#define MMA8X5X_STATUS_ZDR                  (1 << 2)
#define MMA8X5X_STATUS_ZYXDR                (1 << 3)
#define MMA8X5X_STATUS_XOW                  (1 << 4)
#define MMA8X5X_STATUS_YOW                  (1 << 5)
#define MMA8X5X_STATUS_ZOW                  (1 << 6)
#define MMA8X5X_STATUS_ZYXOW                (1 << 7)

#define MMA8X5X_F_STATUS_F_CNT_MASK         0x3F
#define MMA8X5X_F_STATUS_F_WMRK_FLAG        (1 << 6)
#define MMA8X5X_F_STATUS_F_OVF              (1 << 7)

#define MMA8X5X_F_SETUP_MODE_MASK           0xC0
#define MMA8X5X_F_SETUP_MODE_DISABLED       0
#define MMA8X5X_F_SETUP_MODE_CIRCULAR       1
#define MMA8X5X_F_SETUP_MODE_STOP           2
#define MMA8X5X_F_SETUP_MODE_TRIGGER        3
#define MMA8X5X_F_SETUP_F_WMRK_MASK         0x3F

#define MMA8X5X_TRIG_CFG_FF_MT              (1 << 2)
#define MMA8X5X_TRIG_CFG_PULSE              (1 << 3)
#define MMA8X5X_TRIG_CFG_LNDPRT             (1 << 4)
#define MMA8X5X_TRIG_CFG_TRANS              (1 << 5)

#define MMA8X5X_SYSMOD_MASK                 0x3
#define MMA8X5X_SYSMOD_STANDBY              0
#define MMA8X5X_SYSMOD_WAKE                 1
#define MMA8X5X_SYSMOD_SLEEP                2
#define MMA8X5X_SYSMOD_FGT_MASK             0x7C
#define MMA8X5X_SYSMOD_FGERR                (1 << 7)

#define MMA8X5X_INT_SOURCE_DRDY             (1 << 0)
#define MMA8X5X_INT_SOURCE_FF_MT            (1 << 2)
#define MMA8X5X_INT_SOURCE_PULSE            (1 << 3)
#define MMA8X5X_INT_SOURCE_LNDPRT           (1 << 4)
#define MMA8X5X_INT_SOURCE_TRANS            (1 << 5)
#define MMA8X5X_INT_SOURCE_FIFO             (1 << 6)
#define MMA8X5X_INT_SOURCE_ASLP             (1 << 7)

#define MMA8X5X_XYZ_DATA_CFG_FS_MASK        0x3
#define MMA8X5X_XYZ_DATA_CFG_HPF_OUT        (1 << 4)

#define MMA8X5X_HP_FILTER_SEL_MASK          0x03
#define MMA8X5X_HP_FILTER_LPF_EN            (1 << 4)
#define MMA8X5X_HP_FILTER_HPF_BYP           (1 << 5)

#define MMA8X5X_PL_STATUS_BAFRO             (1 << 0)
#define MMA8X5X_PL_STATUS_LAPO_MASK         0x6
#define MMA8X5X_PL_STATUS_LAPO_P_UP         0
#define MMA8X5X_PL_STATUS_LAPO_P_DOWN       1
#define MMA8X5X_PL_STATUS_LAPO_L_RIGHT      2
#define MMA8X5X_PL_STATUS_LAPO_L_LEFT       3
#define MMA8X5X_PL_STATUS_LO                (1 << 6)
#define MMA8X5X_PL_STATUS_NEWLP             (1 << 7)

#define MMA8X5X_PL_CFG_PL_EN                (1 << 6)
#define MMA8X5X_PL_CFG_DBCNTM               (1 << 7)

#define MMA8X5X_PL_BF_ZCOMP_ZLOCK_MASK      0x07
#define MMA8X5X_PL_BF_ZCOMP_BKFR_MASK       0xC0

#define MMA8X5X_P_L_HYS_MASK                0x07
#define MMA8X5X_P_L_THS_MASK                0xF8

#define MMA8X5X_FF_MT_CFG_XEFE              (1 << 3)
#define MMA8X5X_FF_MT_CFG_YEFE              (1 << 4)
#define MMA8X5X_FF_MT_CFG_ZEFE              (1 << 5)
#define MMA8X5X_FF_MT_CFG_OAE               (1 << 6)
#define MMA8X5X_FF_MT_CFG_ELE               (1 << 7)

#define MMA8X5X_FF_MT_SRC_XHP               (1 << 0)
#define MMA8X5X_FF_MT_SRC_XHE               (1 << 1)
#define MMA8X5X_FF_MT_SRC_YHP               (1 << 2)
#define MMA8X5X_FF_MT_SRC_YHE               (1 << 3)
#define MMA8X5X_FF_MT_SRC_ZHP               (1 << 4)
#define MMA8X5X_FF_MT_SRC_ZHE               (1 << 5)
#define MMA8X5X_FF_MT_SRC_EA                (1 << 7)

#define MMA8X5X_FF_MT_THS_MASK              0x7F
#define MMA8X5X_FF_MT_THS_DBCNTM            (1 << 7)

#define MMA8X5X_TRANSIENT_CFG_HPF_BYP       (1 << 0)
#define MMA8X5X_TRANSIENT_CFG_XTEFE         (1 << 1)
#define MMA8X5X_TRANSIENT_CFG_YTEFE         (1 << 2)
#define MMA8X5X_TRANSIENT_CFG_ZTEFE         (1 << 3)
#define MMA8X5X_TRANSIENT_CFG_ELE           (1 << 4)

#define MMA8X5X_TRANSIENT_SRC_XTPOL         (1 << 0)
#define MMA8X5X_TRANSIENT_SRC_XTEVENT       (1 << 1)
#define MMA8X5X_TRANSIENT_SRC_YTPOL         (1 << 2)
#define MMA8X5X_TRANSIENT_SRC_YTEVENT       (1 << 3)
#define MMA8X5X_TRANSIENT_SRC_ZTPOL         (1 << 4)
#define MMA8X5X_TRANSIENT_SRC_ZTEVENT       (1 << 5)
#define MMA8X5X_TRANSIENT_SRC_EA            (1 << 6)

#define MMA8X5X_TRANSIENT_THS_MASK          0x7F
#define MMA8X5X_TRANSIENT_THS_DBCNTM        (1<< 7)

#define MMA8X5X_PULSE_CFG_XSPEFE            (1 << 0)
#define MMA8X5X_PULSE_CFG_XDPEFE            (1 << 1)
#define MMA8X5X_PULSE_CFG_YSPEFE            (1 << 2)
#define MMA8X5X_PULSE_CFG_YDPEFE            (1 << 3)
#define MMA8X5X_PULSE_CFG_ZSPEFE            (1 << 4)
#define MMA8X5X_PULSE_CFG_ZDPEFE            (1 << 5)
#define MMA8X5X_PULSE_CFG_ELE               (1 << 6)
#define MMA8X5X_PULSE_CFG_DPA               (1 << 7)

#define MMA8X5X_PULSE_SRC_POLX              (1 << 0)
#define MMA8X5X_PULSE_SRC_POLY              (1 << 1)
#define MMA8X5X_PULSE_SRC_POLZ              (1 << 2)
#define MMA8X5X_PULSE_SRC_DPE               (1 << 3)
#define MMA8X5X_PULSE_SRC_AXX               (1 << 4)
#define MMA8X5X_PULSE_SRC_AXY               (1 << 5)
#define MMA8X5X_PULSE_SRC_AXZ               (1 << 6)
#define MMA8X5X_PULSE_SRC_EA                (1 << 7)

#define MMA8X5X_PULSE_THSX_MASK             0x7F
#define MMA8X5X_PULSE_THSY_MASK             0x7F
#define MMA8X5X_PULSE_THSZ_MASK             0x7F

#define MMA8X5X_CTRL_REG1_ACTIVE            (1 << 0)
#define MMA8X5X_CTRL_REG1_F_READ            (1 << 1)
#define MMA8X5X_CTRL_REG1_DR_MASK           0x38
#define MMA8X5X_CTRL_REG1_DR_SHIFT          3
#define MMA8X5X_CTRL_REG1_DR(x)             (((uint8_t)(((uint8_t)(x))<<MMA8X5X_CTRL_REG1_DR_SHIFT))\
                                            &MMA8X5X_CTRL_REG1_DR_MASK)
#define MMA8X5X_CTRL_REG1_ASR_MASK          0xC0
#define MMA8X5X_CTRL_REG1_ASR_50HZ          0
#define MMA8X5X_CTRL_REG1_ASR_12HZ5         1
#define MMA8X5X_CTRL_REG1_ASR_6HZ25         2
#define MMA8X5X_CTRL_REG1_ASR_1HZ56         3

#define MMA8X5X_CTRL_REG2_MODS_MASK         0x3
#define MMA8X5X_CTRL_REG2_MODS_NORMAL           0
#define MMA8X5X_CTRL_REG2_MODS_LNLP         1
#define MMA8X5X_CTRL_REG2_MODS_HR           2
#define MMA8X5X_CTRL_REG2_MODS_LP           3
#define MMA8X5X_CTRL_REG2_SLPE              (1 << 2)
#define MMA8X5X_CTRL_REG2_SMODS_MASK        0x18
#define MMA8X5X_CTRL_REG2_SMODS_NORMAL      0
#define MMA8X5X_CTRL_REG2_SMODS_LNLP        1
#define MMA8X5X_CTRL_REG2_SMODS_HR          2
#define MMA8X5X_CTRL_REG2_SMODS_LP          3
#define MMA8X5X_CTRL_REG2_RST               (1 << 6)
#define MMA8X5X_CTRL_REG2_ST                (1 << 7)

#define MMA8X5X_CTRL_REG3_PP_OD             (1 << 0)
#define MMA8X5X_CTRL_REG3_IPOL              (1 << 1)
#define MMA8X5X_CTRL_REG3_WAKE_FF_MT        (1 << 3)
#define MMA8X5X_CTRL_REG3_WAKE_PULSE        (1 << 4)
#define MMA8X5X_CTRL_REG3_WAKE_LNDPRT       (1 << 5)
#define MMA8X5X_CTRL_REG3_WAKE_TRANS        (1 << 6)
#define MMA8X5X_CTRL_REG3_FIFO_GATE         (1 << 7)

#define MMA8X5X_CTRL_REG4_INT_EN_DRDY       (1 << 0)
#define MMA8X5X_CTRL_REG4_INT_EN_FF_MT      (1 << 2)
#define MMA8X5X_CTRL_REG4_INT_EN_PULSE      (1 << 3)
#define MMA8X5X_CTRL_REG4_INT_EN_LNDPRT     (1 << 4)
#define MMA8X5X_CTRL_REG4_INT_EN_TRANS      (1 << 5)
#define MMA8X5X_CTRL_REG4_INT_EN_FIFO       (1 << 6)
#define MMA8X5X_CTRL_REG4_INT_EN_ASLP       (1 << 7)

#define MMA8X5X_CTRL_REG5_INT_CFG_DRDY      (1 << 0)
#define MMA8X5X_CTRL_REG5_INT_CFG_FF_MT     (1 << 2)
#define MMA8X5X_CTRL_REG5_INT_CFG_PULSE     (1 << 3)
#define MMA8X5X_CTRL_REG5_INT_CFG_LNDPRT    (1 << 4)
#define MMA8X5X_CTRL_REG5_INT_CFG_TRANS     (1 << 5)
#define MMA8X5X_CTRL_REG5_INT_CFG_FIFO      (1 << 6)
#define MMA8X5X_CTRL_REG5_INT_CFG_ASLP      (1 << 7)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
