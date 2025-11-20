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
 * @ingroup     drivers_mag3110
 * @{
 *
 * @file
 * @brief       Register definition for the MAG3110 magnetometer driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

#define MAG3110_DR_STATUS            0x00 /**< Data ready status per axis */
#define MAG3110_OUT_X_MSB            0x01 /**< Bits [15:8] of X measurement */
#define MAG3110_OUT_X_LSB            0x02 /**< Bits [7:0] of X measurement */
#define MAG3110_OUT_Y_MSB            0x03 /**< Bits [15:8] of Y measurement */
#define MAG3110_OUT_Y_LSB            0x04 /**< Bits [7:0] of Y measurement */
#define MAG3110_OUT_Z_MSB            0x05 /**< Bits [15:8] of Z measurement */
#define MAG3110_OUT_Z_LSB            0x06 /**< Bits [7:0] of Z measurement */
#define MAG3110_WHO_AM_I             0x07 /**< Device Identification Register */
#define MAG3110_SYSMOD               0x08 /**< FIFO Status Register */
#define MAG3110_OFF_X_MSB            0x09 /**< Bits [15:8] of user X offset */
#define MAG3110_OFF_X_LSB            0x0A /**< Bits [7:0] of user X offset */
#define MAG3110_OFF_Y_MSB            0x0B /**< Bits [15:8] of user Y offset */
#define MAG3110_OFF_Y_LSB            0x0C /**< Bits [7:0] of user Y offset */
#define MAG3110_OFF_Z_MSB            0x0D /**< Bits [15:8] of user Z offset */
#define MAG3110_OFF_Z_LSB            0x0E /**< Bits [7:0] of user Z offset */
#define MAG3110_DIE_TEMP             0x0F /**< Temperature, signed 8 bits */
#define MAG3110_CTRL_REG1            0x10 /**< Operation modes */
#define MAG3110_CTRL_REG2            0x11 /**< Operation modes */

#define MAG3110_DR_STATUS_ZYXOW           (1 << 7)
#define MAG3110_DR_STATUS_ZOW             (1 << 6)
#define MAG3110_DR_STATUS_YOW             (1 << 5)
#define MAG3110_DR_STATUS_XOW             (1 << 4)
#define MAG3110_DR_STATUS_ZYXDR           (1 << 3)
#define MAG3110_DR_STATUS_ZDR             (1 << 2)
#define MAG3110_DR_STATUS_YDR             (1 << 1)
#define MAG3110_DR_STATUS_XDR             (1 << 0)

#define MAG3110_ID                        0xC4 /**< Device ID */

#define MAG3110_SYSMOD_STANDBY            0
#define MAG3110_SYSMOD_ACTIVE_RAW         1
#define MAG3110_SYSMOD_ACTIVE             2

#define MAG3110_CTRL_REG1_DROS_SHIFT      3
#define MAG3110_CTRL_REG1_DROS_MASK       0xF8
#define MAG3110_CTRL_REG1_DROS(x)         (((uint8_t)(((uint8_t)(x))<<MAG3110_CTRL_REG1_DROS_SHIFT))\
                                          &MAG3110_CTRL_REG1_DROS_MASK)
#define MAG3110_CTRL_REG1_FR              (1 << 2)
#define MAG3110_CTRL_REG1_TM              (1 << 1)
#define MAG3110_CTRL_REG1_AC              (1 << 0)

#define MAG3110_CTRL_REG2_AUTO_MRST_EN    (1 << 7)
#define MAG3110_CTRL_REG2_RAW             (1 << 5)
#define MAG3110_CTRL_REG2_MAG_RST         (1 << 4)

#ifdef __cplusplus
}
#endif

/** @} */
