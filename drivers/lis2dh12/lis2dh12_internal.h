/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_lis2dh12
 * @{
 *
 * @file
 * @brief       Command definition for the LIS2DH12 accelerometer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LIS2DH12 registers
 * @{
 */
#define REG_STATUS_REG_AUX          (0x07)
#define REG_OUT_TEMP_L              (0x0C)
#define REG_OUT_TEMP_H              (0x0D)
#define REG_WHO_AM_I                (0x0F)
#define REG_CTRL_REG0               (0x1E)
#define REG_TEMP_CFG_REG            (0x1F)
#define REG_CTRL_REG1               (0x20)
#define REG_CTRL_REG2               (0x21)
#define REG_CTRL_REG3               (0x22)
#define REG_CTRL_REG4               (0x23)
#define REG_CTRL_REG5               (0x24)
#define REG_CTRL_REG6               (0x25)
#define REG_REFERENCE               (0x26)
#define REG_STATUS_REG              (0x27)
#define REG_OUT_X_L                 (0x28)
#define REG_OUT_X_H                 (0x29)
#define REG_OUT_Y_L                 (0x2A)
#define REG_OUT_Y_H                 (0x2B)
#define REG_OUT_Z_L                 (0x2C)
#define REG_OUT_Z_H                 (0x2D)
#define REG_FIFO_CTRL_REG           (0x2E)
#define REG_FIFO_SRC_REG            (0x2F)
#define REG_INT1_CFG                (0x30)
#define REG_INT1_SRC                (0x31)
#define REG_INT1_THS                (0x32)
#define REG_INT1_DURATION           (0x33)
#define REG_INT2_CFG                (0x34)
#define REG_INT2_SRC                (0x35)
#define REG_INT2_THS                (0x36)
#define REG_INT2_DURATION           (0x37)
#define REG_CLICK_CFG               (0x38)
#define REG_CLICK_SRC               (0x39)
#define REG_CLICK_THS               (0x3A)
#define REG_TIME_LIMIT              (0x3B)
#define REG_TIME_LATENCY            (0x3C)
#define REG_TIME_WINDOW             (0x3D)
#define REG_ACT_THS                 (0x3E)
#define REG_ACT_DUR                 (0x3F)
/** @} */

/**
 * @name    Selected register values
 * @{
 */
#define WHO_AM_I_VAL                (0x33)
#define CTRL_REG0_DEFAULT           (0x10)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
