/*
 * Copyright (C) 2025 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_gp8xxx
 * @{
 *
 * @file
 * @brief       Actuator information of Guestgood GP8xxx I2C DACs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef GP8XXX_INFO_H
#define GP8XXX_INFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GP8xxx actuator information
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
 * @brief   GP8503 info
 * @{
 */
#define GP8503_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC
#define GP8503_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_DUAL
#define GP8503_INFO_RANGE               GP8XXX_INFO_RANGE_2V5_VCC
#define GP8503_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_12_BIT
/** @} */

/**
 * @brief   GP8512 info
 * @{
 */
#define GP8512_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC
#define GP8512_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_SINGLE
#define GP8512_INFO_RANGE               GP8XXX_INFO_RANGE_2V5_VCC
#define GP8512_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_15_BIT
/** @} */

/**
 * @brief   GP8211S info
 * @{
 */
#define GP8211S_INFO_TYPE               GP8XXX_INFO_TYPE_VDAC
#define GP8211S_INFO_CHANNELS           GP8XXX_INFO_CHANNELS_SINGLE
#define GP8211S_INFO_RANGE              GP8XXX_INFO_RANGE_5V_10V
#define GP8211S_INFO_RESOLUTION         GP8XXX_INFO_RESOLUTION_15_BIT
/** @} */

/**
 * @brief   GP8403 info
 * @{
 */
#define GP8403_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC
#define GP8403_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_DUAL
#define GP8403_INFO_RANGE               GP8XXX_INFO_RANGE_5V_10V
#define GP8403_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_12_BIT
/** @} */

/**
 * @brief   GP8413 info
 * @{
 */
#define GP8413_INFO_TYPE                GP8XXX_INFO_TYPE_VDAC
#define GP8413_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_DUAL
#define GP8413_INFO_RANGE               GP8XXX_INFO_RANGE_5V_10V
#define GP8413_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_15_BIT
/** @} */

/**
 * @brief   GP8302 info
 * @{
 */
#define GP8302_INFO_TYPE                GP8XXX_INFO_TYPE_IDAC
#define GP8302_INFO_CHANNELS            GP8XXX_INFO_CHANNELS_SINGLE
#define GP8302_INFO_RANGE               GP8XXX_INFO_RANGE_4MA_20MA
#define GP8302_INFO_RESOLUTION          GP8XXX_INFO_RESOLUTION_12_BIT
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

#endif /* GP8XXX_INFO_H */
/** @} */
