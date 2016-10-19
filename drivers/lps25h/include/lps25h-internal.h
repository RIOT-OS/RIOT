/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lps25h
 * @{
 *
 * @file
 * @brief       Definitions for the LPS25H pressure sensor
 *
 * @author      Rene Herthel <rene-herthel@outlook.de>
 */

#ifndef LPS25H_INTERNAL_H
#define LPS25H_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LPS25H register table
 * @{
 */
/** RESERVED 0x00-0x07                */
#define LPS25H_REG_REF_P_XL             (0x08)
#define LPS25H_REG_REF_P_L              (0x09)
#define LPS25H_REG_REF_P_H              (0x0A)
/** RESERVED 0x0D-0x0E                */
#define LPS25H_REG_WHO_AM_I             (0x0F)
#define LPS25H_REG_RES_CONF             (0x10)
/** RESERVED 0x11-0x1F                */
#define LPS25H_REG_CTRL_REG1            (0x20)
#define LPS25H_REG_CTRL_REG2            (0x21)
#define LPS25H_REG_CTRL_REG3            (0x22)
#define LPS25H_REG_CTRL_REG4            (0x23)
#define LPS25H_REG_INT_CFG              (0x24)
#define LPS25H_REG_INT_SOURCE           (0x25)
/** RESERVED 0x26                     */
#define LPS25H_REG_STATUS_REG           (0x27)
#define LPS25H_REG_PRESS_OUT_XL         (0x28)
#define LPS25H_REG_PRESS_OUT_L          (0x29)
#define LPS25H_REG_PRESS_OUT_H          (0x2A)
#define LPS25H_REG_TEMP_OUT_L           (0x2B)
#define LPS25H_REG_TEMP_OUT_H           (0x2C)
/** RESERVED 0x2D                     */
#define LPS25H_REG_FIFO_CTRL            (0x2E)
#define LPS25H_REG_FIFO_STATUS          (0x2F)
#define LPS25H_REG_THS_P_L              (0x30)
#define LPS25H_REG_THS_P_H              (0x31)
/** RESERVED 0x32-0x38                */
#define LPS25H_REG_RPDS_L               (0x39)
#define LPS25H_REG_RPDS_H               (0x3A)
/** @} */

/**
 * @name LPS25H masks for CTRL_REG1
 * @{
 */
#define LPS25H_MASK_CTRL_REG1_PD        (0x80)
#define LPS25H_MASK_CTRL_REG1_ODR       (0x70)
#define LPS25H_MASK_CTRL_REG1_DIFF_EN   (0x08)
#define LPS25H_MASK_CTRL_REG1_BDU       (0x04)
#define LPS25H_MASK_CTRL_REG1_RESET_AZ  (0x02)
#define LPS25H_MASK_CTRL_REG1_SIM       (0x01)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LPS331AP_INTERNAL_H */
/** @} */
