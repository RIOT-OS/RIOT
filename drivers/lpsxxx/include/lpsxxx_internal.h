/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_lpsxxx
 * @{
 *
 * @file
 * @brief       Definitions for the LPSXXX family of pressure sensors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Common LPS331AP/LPS25HB registers
 * @{
 */
#define LPSXXX_REG_WHO_AM_I             (0x0f)
#define LPSXXX_REG_STATUS               (0x27)
#define LPSXXX_REG_PRESS_OUT_XL         (0x28)
#define LPSXXX_REG_PRESS_OUT_L          (0x29)
#define LPSXXX_REG_PRESS_OUT_H          (0x2a)
#define LPSXXX_REG_TEMP_OUT_L           (0x2b)
#define LPSXXX_REG_TEMP_OUT_H           (0x2c)
/** @} */

/**
 * @name    Common LPS331AP/LPS25HB CTRL_REG1 bitfields
 * @{
 */
#define LPSXXX_CTRL_REG1_ODR            (0x70)
#define LPSXXX_CTRL_REG1_ODR_POS        (4)
#define LPSXXX_CTRL_REG1_SIM            (0x01)
/** @} */

#if MODULE_LPS331AP

/**
 * @name    LPS331AP registers
 * @{
 */
#define LPSXXX_REG_REF_P_XL             (0x08)
#define LPSXXX_REG_REF_P_L              (0x09)
#define LPSXXX_REG_REF_P_H              (0x0a)
#define LPSXXX_REG_RES_CONF             (0x10)
#define LPSXXX_REG_CTRL_REG1            (0x20)
#define LPSXXX_REG_CTRL_REG2            (0x21)
#define LPSXXX_REG_CTRL_REG3            (0x22)
#define LPSXXX_REG_INT_CFG              (0x23)
#define LPSXXX_REG_INT_SOURCE           (0x24)
#define LPSXXX_REG_THS_P_LOW            (0x25)
#define LPSXXX_REG_THS_P_HIGH           (0x26)
#define LPSXXX_REG_AMP_CTRL             (0x30)
/** @} */

/**
 * @name    LPS331AP CTRL_REG1 bitfields
 * @{
 */
#define LPSXXX_CTRL_REG1_PD             (0x80)
#define LPSXXX_CTRL_REG1_DIFF_EN        (0x08)
#define LPSXXX_CTRL_REG1_DBDU           (0x04)
#define LPSXXX_CTRL_REG1_DELTA_EN       (0x02)
/** @} */

/**
 * @brief   LPS331AP WHO_AM_I register value
 */
#define LPSXXX_WHO_AM_I                 (0xbb)

#elif MODULE_LPS25HB

/**
 * @name    LPS25HB registers
 * @{
 */
#define LPSXXX_REG_REF_P_XL             (0x08)
#define LPSXXX_REG_REF_P_L              (0x09)
#define LPSXXX_REG_REF_P_H              (0x0a)
#define LPSXXX_REG_RES_CONF             (0x10)
#define LPSXXX_REG_CTRL_REG1            (0x20)
#define LPSXXX_REG_CTRL_REG2            (0x21)
#define LPSXXX_REG_CTRL_REG3            (0x22)
#define LPSXXX_REG_CTRL_REG4            (0x23)
#define LPSXXX_REG_INT_CFG              (0x24)
#define LPSXXX_REG_INT_SOURCE           (0x25)
#define LPSXXX_REG_FIFO_CTRL            (0x2e)
#define LPSXXX_REG_FIFO_STATUS          (0x2f)
#define LPSXXX_REG_THS_P_LOW            (0x30)
#define LPSXXX_REG_THS_P_HIGH           (0x31)
#define LPSXXX_REG_RPDS_L               (0x39)
#define LPSXXX_REG_RPDS_H               (0x3a)
/** @} */

/**
 * @name    LPS25HB CTRL_REG1 bitfields
 * @{
 */
#define LPSXXX_CTRL_REG1_PD             (0x80)
#define LPSXXX_CTRL_REG1_DIFF_EN        (0x08)
#define LPSXXX_CTRL_REG1_BDU            (0x04)
#define LPSXXX_CTRL_REG1_RESET_AZ       (0x02)
/** @} */

/**
 * @brief   LPS25HB WHO_AM_I register value
 */
#define LPSXXX_WHO_AM_I                 (0xbd)

#elif MODULE_LPS22HB

/**
 * @name    LPS22HB registers
 * @{
 */
#define LPSXXX_REG_INT_CFG              (0x0b)
#define LPSXXX_REG_THS_P_L              (0x0c)
#define LPSXXX_REG_THS_P_H              (0x0d)
#define LPSXXX_REG_REF_P_XL             (0x15)
#define LPSXXX_REG_RES_CONF             (0x1a)
#define LPSXXX_REG_CTRL_REG1            (0x10)
#define LPSXXX_REG_CTRL_REG2            (0x11)
#define LPSXXX_REG_CTRL_REG3            (0x13)
#define LPSXXX_REG_FIFO_CTRL            (0x2e)
#define LPSXXX_REG_REF_P_XL             (0x15)
#define LPSXXX_REG_REF_P_L              (0x16)
#define LPSXXX_REG_REF_P_H              (0x17)
#define LPSXXX_REG_RPDS_L               (0x18)
#define LPSXXX_REG_RPDS_H               (0x19)
#define LPSXXX_REG_RES_CONF             (0x1a)
#define LPSXXX_REG_INT_SOURCE           (0x25)
#define LPSXXX_REG_FIFO_STATUS          (0x26)
#define LPSXXX_REG_LPFP_RES             (0x33)
/** @} */

/**
 * @name    LPS22HB CTRL_REG1 bitfields
 * @{
 */
#define LPSXXX_CTRL_REG1_EN_LPFP        (0x08)
#define LPSXXX_CTRL_REG1_LPFP_CFG       (0x04)
#define LPSXXX_CTRL_REG1_BDU            (0x02)
/** @} */

/**
 * @name    LPS22HB CTRL_REG2 bitfields
 * @{
 */
#define LPSXXX_CTRL_REG2_ID_ADD_INC     (0x10)
/** @} */

/**
 * @brief   LPS22HB WHO_AM_I register value
 */
#define LPSXXX_WHO_AM_I                 (0xb1)

#elif MODULE_LPS22HH || MODULE_LPS22CH

/**
 * @name    LPS22HH/LPS22CH registers
 * @{
 */
#define LPSXXX_REG_INT_CFG              (0x0b)
#define LPSXXX_REG_THS_P_L              (0x0c)
#define LPSXXX_REG_THS_P_H              (0x0d)
#define LPSXXX_REG_IF_CTRL              (0x0e)
#define LPSXXX_REG_CTRL_REG1            (0x10)
#define LPSXXX_REG_CTRL_REG2            (0x11)
#define LPSXXX_REG_CTRL_REG3            (0x12)
#define LPSXXX_REG_FIFO_CTRL            (0x13)
#define LPSXXX_REG_FIFO_WTM             (0x14)
#define LPSXXX_REG_REF_P_L              (0x15)
#define LPSXXX_REG_REF_P_H              (0x16)
#define LPSXXX_REG_RPDS_L               (0x18)
#define LPSXXX_REG_RPDS_H               (0x19)
#define LPSXXX_REG_INT_SOURCE           (0x24)
#define LPSXXX_REG_FIFO_STATUS1         (0x25)
#define LPSXXX_REG_FIFO_STATUS2         (0x26)
#define LPSXXX_REG_FIFO_OUT_PRESS_XL    (0x78)
#define LPSXXX_REG_FIFO_OUT_PRESS_L     (0x79)
#define LPSXXX_REG_FIFO_OUT_PRESS_H     (0x7a)
#define LPSXXX_REG_FIFO_OUT_TEMP_L      (0x7b)
#define LPSXXX_REG_FIFO_OUT_TEMP_H      (0x7c)
/** @} */

/**
 * @name    LPS22HH/LPS22CH CTRL_REG1 bitfields
 * @{
 */
#define LPSXXX_CTRL_REG1_EN_LPFP        (0x08)
#define LPSXXX_CTRL_REG1_LPFP_CFG       (0x04)
#define LPSXXX_CTRL_REG1_BDU            (0x02)
/** @} */

/**
 * @name    LPS22HH/LPS22CH CTRL_REG2 bitfields
 * @{
 */
#define LPSXXX_CTRL_REG2_ID_ADD_INC     (0x10)
/** @} */

/**
 * @brief   LPS22HH/LPS22CH WHO_AM_I register value
 */
#define LPSXXX_WHO_AM_I                 (0xb3)

#endif /* MODULE_LPS22HH/LPS22CH */

#ifdef __cplusplus
}
#endif

/** @} */
