/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ft5x06
 * @{
 *
 * @file
 * @brief       Internal register addresses, bitfields and constants
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constants (depends on variants)
 * @{
 */
#define FT5X06_I2C_DEFAULT_ADDRESS                  (0x38)

/**
 * @brief Vendor ID for FT5606, FT5X16, FT5X06I, FT5336, FT3316, FT5436I, FT5336I, FT5X46 models.
 */
#define FT5X06_VENDOR_ID_1                          (0x51)

/**
 * @brief Vendor ID used for most FT6X06 and FT6X36 as well as FT3X67 models.
 */
#define FT5X06_VENDOR_ID_2                          (0x11)

/**
 * @brief Vendor ID used for some FT6X06 and FT6X36 models.
 */
#define FT5X06_VENDOR_ID_3                          (0xcd)

/**
 * @brief Maximum touches count for FT6X06 and FT6X36 models.
 */
#define FT6XX6_TOUCHES_COUNT_MAX                    (2)

/**
 * @brief Maximum touches count for FT5606, FT5X16, FT5X06I, FT5336, FT3316, FT5436I, FT5336I,
 * FT5X46 models.
 */
#define FT5X06_TOUCHES_COUNT_MAX                    (5)
/** @} */

/**
 * @name Register addresses
 * @{
 */
#define FT5X06_DEVIDE_MODE_REG                      (0x00)
#define FT5X06_GESTURE_ID_REG                       (0x01)
#define FT5X06_TD_STATUS_REG                        (0x02)
#define FT5X06_TOUCH1_XH_REG                        (0x03)
#define FT5X06_TOUCH1_XL_REG                        (0x04)
#define FT5X06_TOUCH1_YH_REG                        (0x05)
#define FT5X06_TOUCH1_YL_REG                        (0x06)
#define FT5X06_TOUCH2_XH_REG                        (0x09)
#define FT5X06_TOUCH2_XL_REG                        (0x0A)
#define FT5X06_TOUCH2_YH_REG                        (0x0B)
#define FT5X06_TOUCH2_YL_REG                        (0x0C)
#define FT5X06_TOUCH3_XH_REG                        (0x0F)
#define FT5X06_TOUCH3_XL_REG                        (0x11)
#define FT5X06_TOUCH3_YH_REG                        (0x12)
#define FT5X06_TOUCH3_YL_REG                        (0x13)
#define FT5X06_TOUCH4_XH_REG                        (0x15)
#define FT5X06_TOUCH4_XL_REG                        (0x16)
#define FT5X06_TOUCH4_YH_REG                        (0x17)
#define FT5X06_TOUCH4_YL_REG                        (0x18)
#define FT5X06_TOUCH5_XH_REG                        (0x1B)
#define FT5X06_TOUCH5_XL_REG                        (0x1C)
#define FT5X06_TOUCH5_YH_REG                        (0x1D)
#define FT5X06_TOUCH5_YL_REG                        (0x1E)
#define FT5X06_G_AUTO_CLB_MODE_REG                  (0xA0)
#define FT5X06_G_LIB_VERSION_H_REG                  (0xA1)
#define FT5X06_G_LIB_VERSION_L_REG                  (0xA2)
#define FT5X06_G_CIPHER_REG                         (0xA3)
#define FT5X06_G_MODE_REG                           (0xA4)
#define FT5X06_G_PMODE_REG                          (0xA5)
#define FT5X06_G_FIRMID_REG                         (0xA6)
#define FT5X06_G_STATE_REG                          (0xA7)
#define FT5X06_G_VENDOR_ID_REG                      (0xA8)
/** @} */

/**
 * @name Gesture ID register bitfields
 * @{
 */
#define FT5X06_GESTURE_ID_MOVE_UP                   (0x10)
#define FT5X06_GESTURE_ID_MOVE_LEFT                 (0x14)
#define FT5X06_GESTURE_ID_MOVE_DOWN                 (0x18)
#define FT5X06_GESTURE_ID_MOVE_RIGHT                (0x1C)
#define FT5X06_GESTURE_ID_ZOOM_IN                   (0x48)
#define FT5X06_GESTURE_ID_ZOOM_OUT                  (0x49)
#define FT5X06_GESTURE_ID_NONE                      (0x00)
/** @} */

/**
 * @name Touch detect status register bitfields
 * @{
 */
#define FT5X06_TD_STATUS_MASK                       (0x0F)
/** @} */

/**
 * @name Touch position (LSB/MSB) registers masks
 * @{
 */
#define FT5X06_TOUCH_POS_LSB_MASK                   (0xFF)
#define FT5X06_TOUCH_POS_MSB_MASK                   (0x0F)
/** @} */

/**
 * @name Interrupt mode register masks and bitfields
 * @{
 */
#define FT5X06_G_MODE_INTERRUPT_MASK                (0x03)
#define FT5X06_G_MODE_INTERRUPT_SHIFT               (0x00)
#define FT5X06_G_MODE_INTERRUPT_POLLING             (0x00)
#define FT5X06_G_MODE_INTERRUPT_TRIGGER             (0x01)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
