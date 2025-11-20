/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_vl6180x
 * @brief       Register definitions for ST VL6180X Ranging and Ambient Light Sensing (ALS) module
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
#define VL6180X_REG_MODEL_ID          (0x00)  /**< Device ID */
#define VL6180X_REG_MODEL_REV_MAJOR   (0x01)  /**< Device revision (major) */
#define VL6180X_REG_MODEL_REV_MINOR   (0x02)  /**< Device revision (minor) */
#define VL6180X_REG_MODULE_REV_MAJOR  (0x03)  /**< Module revision (major) */
#define VL6180X_REG_MODULE_REV_MINOR  (0x04)  /**< Module revision (minor) */

#define VL6180X_REG_GPIO0_MODE        (0x10)  /**< GPIO0 mode definition */
#define VL6180X_REG_GPIO1_MODE        (0x11)  /**< GPIO1 mode definition */
#define VL6180X_REG_HISTORY_CTRL      (0x12)  /**< ALS and Ranging history control */
#define VL6180X_REG_INT_CONFIG        (0x14)  /**< Interrupt mode config */
#define VL6180X_REG_INT_CLR           (0x15)  /**< Interrupt clear */
#define VL6180X_REG_FRESH_RST         (0x16)  /**< Fresh out of reset bit */

#define VL6180X_REG_RNG_START         (0x18)  /**< Range measurement start */
#define VL6180X_REG_RNG_THRESH_HI     (0x19)  /**< Range measurement high threshold */
#define VL6180X_REG_RNG_THRESH_LO     (0x1a)  /**< Range measurement low threshold */
#define VL6180X_REG_RNG_PERIOD        (0x1b)  /**< Range measurement period in continuous mode */
#define VL6180X_REG_RNG_MAX_TIME      (0x1c)  /**< Range measurement time limit */
#define VL6180X_REG_RNG_VALUE         (0x62)  /**< Range 8-bit value in mm */

#define VL6180X_REG_ALS_START         (0x38)  /**< ALS measurement start */
#define VL6180X_REG_ALS_THRESH_HI     (0x3a)  /**< ALS measurement high threshold */
#define VL6180X_REG_ALS_THRESH_LO     (0x3c)  /**< ALS measurement low threshold */
#define VL6180X_REG_ALS_PERIOD        (0x3e)  /**< ALS measurement period in continuous mode */
#define VL6180X_REG_ALS_GAIN          (0x3f)  /**< ALS analogue gain */
#define VL6180X_REG_ALS_INT_TIME      (0x40)  /**< ALS integration time */
#define VL6180X_REG_ALS_VALUE         (0x50)  /**< ALS 16-bit count value */

#define VL6180X_REG_RNG_STATUS        (0x4d)  /**< Range measurement status */
#define VL6180X_REG_ALS_STATUS        (0x4e)  /**< ALS measurement status */
#define VL6180X_REG_INT_STATUS        (0x4f)  /**< Interrupt status */

#define VL6180X_REG_I2C_ADDR          (0x212) /**< Programmable device address */
#define VL6180X_REG_INTERLEAVED_MODE  (0x2a3) /**< Interleaved mode enable */
/** @} */

/**
 * @name    Register structures
 * @{
 */

/* VL6180X_REG_RNG_START */
#define VL6180X_RNG_MODE_CONT   (0x02) /**< Continuous range measurement mode */
#define VL6180X_RNG_START_STOP  (0x01) /**< Start/stop range measurement */

/* VL6180X_REG_ALS_START */
#define VL6180X_ALS_MODE_CONT   (0x02) /**< ALS measurement mode */
#define VL6180X_ALS_START_STOP  (0x01) /**< Start/stop ALS measurement */

/* VL6180X_REG_RNG_STATUS */
#define VL6180X_RNG_ERR_CODE    (0xf0) /**< Range measurement error code mask */
#define VL6180X_RNG_ERR_CODE_S  (4)    /**< Range measurement error code shift */
#define VL6180X_RNG_DEVICE_RDY  (0x01) /**< Range device ready */

/* VL6180X_REG_ALS_STATUS */
#define VL6180X_ALS_ERR_CODE    (0xf0) /**< ALS measurement error code mask */
#define VL6180X_ALS_ERR_CODE_S  (4)    /**< ALS measurement error code shift */
#define VL6180X_ALS_DEVICE_RDY  (0x01) /**< ALS device ready */

/* VL6180X_REG_ALS_GAIN */
#define VL6180X_ALS_GAIN_LIGHT  (0x07) /**< ALS analogue gain mask (light channel) */

/* VL6180X_REG_INT_CONFIG and VL6180X_REG_INT_STATUS */
#define VL6180X_INT_RNG         (0x07) /**< RNG interrupt mask */
#define VL6180X_INT_RNG_S       (0)    /**< RNG interrupt shift */
#define VL6180X_INT_ALS         (0x38) /**< ALS interrupt mask */
#define VL6180X_INT_ALS_S       (3)    /**< ALS interrupt shift */
#define VL6180X_ERR_INT         (0xc0) /**< Error interrupt mask (VL6180X_REG_INT_STATUS only) */
#define VL6180X_ERR_INT_S       (6)    /**< Error interrupt shift */

#define VL6180X_INT_RNG_LOW     (0x01) /**< range < lower threshold */
#define VL6180X_INT_RNG_HIGH    (0x02) /**< range > upper threshold */
#define VL6180X_INT_RNG_OUT     (0x03) /**< range < lower threshold or range > upper threshold */
#define VL6180X_INT_RNG_DRDY    (0x04) /**< new range data are ready to be read */
#define VL6180X_INT_ALS_LOW     (0x08) /**< ALS < lower threshold */
#define VL6180X_INT_ALS_HIGH    (0x10) /**< ALS > upper threshold */
#define VL6180X_INT_ALS_OUT     (0x18) /**< ALS < lower threshold or ALS > upper threshold */
#define VL6180X_INT_ALS_DRDY    (0x20) /**< new ALS data are ready to be read */
#define VL6180X_INT_ERR_LASER   (0x40) /**< Laser safety error */
#define VL6180X_INT_ERR_PLL     (0x80) /**< PLL error */

/* VL6180X_REG_INT_CLR */
#define VL6180X_CLR_ERR_INT     (0x04) /**< Clear error interrupt */
#define VL6180X_CLR_ALS_INT     (0x02) /**< Clear ALS interrupt */
#define VL6180X_CLR_RNG_INT     (0x01) /**< Clear range interrupt */
#define VL6180X_CLR_ALL_INT     (0x07) /**< Clear all interrupts */

/* VL6180X_REG_GPIO0_MODE */
#define VL6180X_GPIO0_SHUT      (0x40) /**< GPIO0 shutdown function mask */
#define VL6180X_GPIO0_SHUT_ON   (0x40) /**< GPIO0 shutdown function enabled */
#define VL6180X_GPIO0_SHUT_OFF  (0x00) /**< GPIO0 shutdown function disabled */
#define VL6180X_GPIO0_POL       (0x20) /**< GPIO0 polarity mask */
#define VL6180X_GPIO0_POL_LOW   (0x00) /**< GPIO0 polarity is low */
#define VL6180X_GPIO0_POL_HIGH  (0x20) /**< GPIO0 polarity is high */
#define VL6180X_GPIO0_FUNC      (0x1e) /**< GPIO0 function mask */
#define VL6180X_GPIO0_FUNC_OFF  (0x00) /**< GPIO0 function off*/
#define VL6180X_GPIO0_FUNC_ON   (0x10) /**< GPIO0 function on */

/* VL6180X_REG_GPIO1_MODE */
#define VL6180X_GPIO1_POL       (0x20) /**< GPIO1 polarity mask */
#define VL6180X_GPIO1_POL_LOW   (0x00) /**< GPIO1 polarity is low */
#define VL6180X_GPIO1_POL_HIGH  (0x20) /**< GPIO1 polarity is high */
#define VL6180X_GPIO1_FUNC      (0x1e) /**< GPIO1 function mask */
#define VL6180X_GPIO1_FUNC_OFF  (0x00) /**< GPIO1 function off */
#define VL6180X_GPIO1_FUNC_ON   (0x10) /**< GPIO1 function on */

/** @} */

/**
 * @name    Default register values
 *
 * These values are the register values after reset or overwritten at
 * boot-up by NVM contents.
 * @{
 */
#define VL6180X_MODEL_ID        (0xb4)  /**< VNCL6180 Device ID */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
