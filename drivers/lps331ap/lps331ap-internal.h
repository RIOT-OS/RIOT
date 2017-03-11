/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lps331ap
 * @{
 *
 * @file
 * @brief       Definitions for the LPS331AP pressure sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LPS331AP_INTERNAL_H
#define LPS331AP_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LPS331AP registers
 * @{
 */
#define LPS331AP_AUTO_INC               0x80
#define LPS331AP_REG_REF_P_XL           0x08
#define LPS331AP_REG_REF_P_L            0x09
#define LPS331AP_REG_REF_P_H            0x0a
#define LPS331AP_REG_WHO_AM_I           0x0f
#define LPS331AP_REG_RES_CONF           0x10
#define LPS331AP_REG_CTRL_REG1          0x20
#define LPS331AP_REG_CTRL_REG2          0x21
#define LPS331AP_REG_CTRL_REG3          0x22
#define LPS331AP_REG_INT_CFG_REG        0x23
#define LPS331AP_REG_INT_SOURCE_REG     0x24
#define LPS331AP_REG_THS_P_LOW_REG      0x25
#define LPS331AP_REG_THS_P_HIGH_REG     0x26
#define LPS331AP_REG_STATUS_REG         0x27
#define LPS331AP_REG_PRESS_OUT_XL       0x28
#define LPS331AP_REG_PRESS_OUT_L        0x29
#define LPS331AP_REG_PRESS_OUT_H        0x2a
#define LPS331AP_REG_TEMP_OUT_L         0x2b
#define LPS331AP_REG_TEMP_OUT_H         0x2c
#define LPS331AP_REG_AMP_CTRL           0x30
/** @} */

/**
 * @name LPS331AP CTRL_REG1 bitfields
 * @{
 */
#define LPS331AP_CTRL_REG1_PD           0x80
#define LPS331AP_CTRL_REG1_ODR          0x70
#define LPS331AP_CTRL_REG1_ODR_POS      4
#define LPS331AP_CTRL_REG1_DIFF_EN      0x08
#define LPS331AP_CTRL_REG1_DBDU         0x04
#define LPS331AP_CTRL_REG1_DELTA_EN     0x02
#define LPS331AP_CTRL_REG1_SIM          0x01
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LPS331AP_INTERNAL_H */
/** @} */
