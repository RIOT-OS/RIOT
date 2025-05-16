/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#pragma once

/**
 * @ingroup     drivers_mpl3115a2
 * @{
 *
 * @file
 * @brief       Register definition for the MPL3115A2 sensor driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 */


#ifdef __cplusplus
extern "C"
{
#endif

#define MPL3115A2_STATUS            0x00 /**< Sensor Status Register */
#define MPL3115A2_OUT_P_MSB         0x01 /**< Pressure Data Out MSB */
#define MPL3115A2_OUT_P_CSB         0x02 /**< Pressure Data Out CSB */
#define MPL3115A2_OUT_P_LSB         0x03 /**< Pressure Data Out LSB */
#define MPL3115A2_OUT_T_MSB         0x04 /**< Temperature Data Out MSB */
#define MPL3115A2_OUT_T_LSB         0x05 /**< Temperature Data Out LSB */
#define MPL3115A2_DR_STATUS         0x06 /**< Sensor Status Register */
#define MPL3115A2_OUT_P_DELTA MSB   0x07 /**< Pressure Data Out Delta MSB */
#define MPL3115A2_OUT_P_DELTA_CSB   0x08 /**< Pressure Data Out Delta CSB */
#define MPL3115A2_OUT_P_DELTA_LSB   0x09 /**< Pressure Data Out Delta LSB */
#define MPL3115A2_OUT_T_DELTA_MSB   0x0A /**< Temperature Data Out Delta MSB */
#define MPL3115A2_OUT_T_DELTA_LSB   0x0B /**< Temperature Data Out Delta LSB */
#define MPL3115A2_WHO_AM_I          0x0C /**< Device Identification Register */
#define MPL3115A2_F_STATUS          0x0D /**< FIFO Status Register */
#define MPL3115A2_F_DATA            0x0E /**< FIFO 8-bit Data Access */
#define MPL3115A2_F_SETUP           0x0F /**< FIFO Setup Register */
#define MPL3115A2_TIME_DLY          0x10 /**< Time Delay Register */
#define MPL3115A2_SYSMOD            0x11 /**< System Mode Register */
#define MPL3115A2_INT_SOURCE        0x12 /**< Interrupt Source Register */
#define MPL3115A2_PT_DATA_CFG       0x13 /**< PT Data Configuration Register */
#define MPL3115A2_BAR_IN_MSB        0x14 /**< BAR Input in MSB */
#define MPL3115A2_BAR_IN_LSB        0x15 /**< BAR Input in LSB */
#define MPL3115A2_P_TGT_MSB         0x16 /**< Pressure Target MSB */
#define MPL3115A2_P_TGT_LSB         0x17 /**< Pressure Target LSB */
#define MPL3115A2_T_TGT             0x18 /**< Temperature Target */
#define MPL3115A2_P_WND_MSB         0x19 /**< Pressure/Altitude Window MSB */
#define MPL3115A2_P_WND_LSB         0x1A /**< Pressure/Altitude Window LSB */
#define MPL3115A2_T_WND             0x1B /**< Temperature Window */
#define MPL3115A2_P_MIN_MSB         0x1C /**< Minimum Pressure Data Out MSB */
#define MPL3115A2_P_MIN_CSB         0x1D /**< Minimum Pressure Data Out CSB */
#define MPL3115A2_P_MIN_LSB         0x1E /**< Minimum Pressure Data Out LSB */
#define MPL3115A2_T_MIN_MSB         0x1F /**< Minimum Temperature Data Out MSB */
#define MPL3115A2_T_MIN_LSB         0x20 /**< Minimum Temperature Data Out LSB */
#define MPL3115A2_P_MAX_MSB         0x21 /**< Maximum Pressure Data Out MSB */
#define MPL3115A2_P_MAX_CSB         0x22 /**< Maximum Pressure Data Out CSB */
#define MPL3115A2_P_MAX_LSB         0x23 /**< Maximum Pressure Data Out LSB */
#define MPL3115A2_T_MAX_MSB         0x24 /**< Maximum Temperature Data Out MSB */
#define MPL3115A2_T_MAX_LSB         0x25 /**< Maximum Temperature Data Out LSB */
#define MPL3115A2_CTRL_REG1         0x26 /**< Control Register 1 */
#define MPL3115A2_CTRL_REG2         0x27 /**< Control Register 2 */
#define MPL3115A2_CTRL_REG3         0x28 /**< Control Register 3 */
#define MPL3115A2_CTRL_REG4         0x29 /**< Control Register 4 */
#define MPL3115A2_CTRL_REG5         0x2A /**< Control Register 5 */
#define MPL3115A2_OFF_P             0x2B /**< Pressure Data User Offset Register */
#define MPL3115A2_OFF_T             0x2C /**< Temperature Data User Offset Register */
#define MPL3115A2_OFF_H             0x2D /**< Altitude Data User Offset Register */

#define MPL3115A2_STATUS_TDR            (1 << 1)
#define MPL3115A2_STATUS_PDR            (1 << 2)
#define MPL3115A2_STATUS_PTDR           (1 << 3)
#define MPL3115A2_STATUS_TOW            (1 << 5)
#define MPL3115A2_STATUS_POW            (1 << 6)
#define MPL3115A2_STATUS_PTOW           (1 << 7)

#define MPL3115A2_PT_DATA_CFG_TDEFE     (1 << 0)
#define MPL3115A2_PT_DATA_CFG_PDEFE     (1 << 1)
#define MPL3115A2_PT_DATA_CFG_DREM      (1 << 2)

#define MPL3115A2_CTRL_REG1_SBYB        (1 << 0)
#define MPL3115A2_CTRL_REG1_OST         (1 << 1)
#define MPL3115A2_CTRL_REG1_RST         (1 << 2)
#define MPL3115A2_CTRL_REG1_OS_SHIFT    3
#define MPL3115A2_CTRL_REG1_OS_MASK     0x38
#define MPL3115A2_CTRL_REG1_OS(x)       (((uint8_t)(((uint8_t)(x))<<MPL3115A2_CTRL_REG1_OS_SHIFT))\
                                        &MPL3115A2_CTRL_REG1_OS_MASK)
#define MPL3115A2_CTRL_REG1_RAW         (1 << 6)
#define MPL3115A2_CTRL_REG1_ALT         (1 << 7)

#define MPL3115A2_CTRL_REG2_ST_SHIFT    0
#define MPL3115A2_CTRL_REG2_ST_MASK     0xF
#define MPL3115A2_CTRL_REG2_ST(x)       (((uint8_t)(((uint8_t)(x))<<MPL3115A2_CTRL_REG2_ST_SHIFT))\
                                        &MPL3115A2_CTRL_REG2_ST_MASK)
#define MPL3115A2_CTRL_REG2_ALARM_SEL   (1 << 4)
#define MPL3115A2_CTRL_REG2_LOAD_OPUT   (1 << 5)

#define MPL3115A2_CTRL_REG3_PP_OD2      (1 << 0)
#define MPL3115A2_CTRL_REG3_IPOL2       (1 << 1)
#define MPL3115A2_CTRL_REG3_PP_OD1      (1 << 4)
#define MPL3115A2_CTRL_REG3_IPOL1       (1 << 5)

#define MPL3115A2_CTRL_REG4_INT_EN_TCHG (1 << 0)
#define MPL3115A2_CTRL_REG4_INT_EN_PCHG (1 << 1)
#define MPL3115A2_CTRL_REG4_INT_EN_TTH  (1 << 2)
#define MPL3115A2_CTRL_REG4_INT_EN_PTH  (1 << 3)
#define MPL3115A2_CTRL_REG4_INT_EN_TW   (1 << 4)
#define MPL3115A2_CTRL_REG4_INT_EN_PW   (1 << 5)
#define MPL3115A2_CTRL_REG4_INT_EN_FIFO (1 << 6)
#define MPL3115A2_CTRL_REG4_INT_EN_DRDY (1 << 7)

#define MPL3115A2_CTRL_REG5_INT_TCHG    (1 << 0)
#define MPL3115A2_CTRL_REG5_INT_PCHG    (1 << 1)
#define MPL3115A2_CTRL_REG5_INT_TTH     (1 << 2)
#define MPL3115A2_CTRL_REG5_INT_PTH     (1 << 3)
#define MPL3115A2_CTRL_REG5_INT_TW      (1 << 4)
#define MPL3115A2_CTRL_REG5_INT_PW      (1 << 5)
#define MPL3115A2_CTRL_REG5_INT_FIFO    (1 << 6)
#define MPL3115A2_CTRL_REG5_INT_DRDY    (1 << 7)

#define MPL3115A2_ID                    0xC4 /**< Device ID */

#ifdef __cplusplus
}
#endif

/** @} */
