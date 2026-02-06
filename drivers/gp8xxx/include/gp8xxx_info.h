/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_gp8xxx
 * @{
 *
 * @file
 * @brief       Definitions of the Guestgood GP8xxx I2C DACs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    GP8xxx DAC information
 * @{
 */
#define GP8XXX_INFO_TYPE_VDAC           (0)         /**< VDAC type */
#define GP8XXX_INFO_TYPE_IDAC           (1)         /**< IDAC type */

#define GP8XXX_INFO_CHANNELS_SINGLE     (1)         /**< Single channel */
#define GP8XXX_INFO_CHANNELS_DUAL       (2)         /**< Dual channel */

#define GP8XXX_INFO_RANGE_2V5_VCC       (0)         /**< 0-2.5 V or 0-VCC V */
#define GP8XXX_INFO_RANGE_5V_10V        (1)         /**< 0-5 V or 0-10 V */
#define GP8XXX_INFO_RANGE_4MA_20MA      (2)         /**< 0-25 mA */

#define GP8XXX_INFO_RESOLUTION_12_BIT   (0x0fff)    /**< 12-bit resolution */
#define GP8XXX_INFO_RESOLUTION_15_BIT   (0x7fff)    /**< 15-bit resolution */
/** @} */

/**
 * @name    GP8503 info
 * @{
 */
#define GP8503_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC           /**< DAC type */
#define GP8503_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_DUAL       /**< Number of channels */
#define GP8503_INFO_RANGE               GP8XXX_INFO_RANGE_2V5_VCC       /**< Output range */
#define GP8503_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_12_BIT   /**< Output resolution */
/** @} */

/**
 * @name    GP8512 info
 * @{
 */
#define GP8512_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC           /**< DAC type */
#define GP8512_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_SINGLE     /**< Number of channels */
#define GP8512_INFO_RANGE               GP8XXX_INFO_RANGE_2V5_VCC       /**< Output range */
#define GP8512_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_15_BIT   /**< Output resolution */
/** @} */

/**
 * @name    GP8211S info
 * @{
 */
#define GP8211S_INFO_TYPE               GP8XXX_INFO_TYPE_VDAC           /**< DAC type */
#define GP8211S_INFO_CHANNELS           GP8XXX_INFO_CHANNELS_SINGLE     /**< Number of channels */
#define GP8211S_INFO_RANGE              GP8XXX_INFO_RANGE_5V_10V        /**< Output range */
#define GP8211S_INFO_RESOLUTION         GP8XXX_INFO_RESOLUTION_15_BIT   /**< Output resolution */
/** @} */

/**
 * @name    GP8403 info
 * @{
 */
#define GP8403_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC           /**< DAC type */
#define GP8403_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_DUAL       /**< Number of channels */
#define GP8403_INFO_RANGE               GP8XXX_INFO_RANGE_5V_10V        /**< Output range */
#define GP8403_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_12_BIT   /**< Output resolution */
/** @} */

/**
 * @name    GP8413 info
 * @{
 */
#define GP8413_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC           /**< DAC type */
#define GP8413_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_DUAL       /**< Number of channels */
#define GP8413_INFO_RANGE               GP8XXX_INFO_RANGE_5V_10V        /**< Output range */
#define GP8413_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_15_BIT   /**< Output resolution */
/** @} */

/**
 * @name    GP8302 info
 * @{
 */
#define GP8302_INFO_TYPE                GP8XXX_INFO_TYPE_IDAC           /**< DAC type */
#define GP8302_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_SINGLE     /**< Number of channels */
#define GP8302_INFO_RANGE               GP8XXX_INFO_RANGE_4MA_20MA      /**< Output range */
#define GP8302_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_12_BIT   /**< Output resolution */
/** @} */

/**
 * @brief   Struct to record GP8xxx actuator information.
 */
typedef struct {
    uint8_t type;           /**< Actuator type (VDAC or IDAC) */
    uint8_t channels;       /**< Number of channels */
    uint8_t range;          /**< Output range */
    uint16_t resolution;    /**< Output resolution */
} gp8xxx_info_t;

/**
 * @brief   GP8503 info structure
 */
extern const gp8xxx_info_t gp8503_info;

/**
 * @brief   GP8512 info structure
 */
extern const gp8xxx_info_t gp8512_info;

/**
 * @brief   GP8211S info structure
 */
extern const gp8xxx_info_t gp8211s_info;

/**
 * @brief   GP8403 info structure
 */
extern const gp8xxx_info_t gp8403_info;

/**
 * @brief   GP8413 info structure
 */
extern const gp8xxx_info_t gp8413_info;

/**
 * @brief   GP8302 info structure
 */
extern const gp8xxx_info_t gp8302_info;

#ifdef __cplusplus
}
#endif

/** @} */
