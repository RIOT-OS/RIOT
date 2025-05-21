/*
 * Copyright (C) 2023 HAW Hamburg. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_arm7tdmi_gba
 * @{
 *
 * @file
 * @brief       GBA specific definitions for internal peripheral handling
 *
 * @author      Bennet Blischke
 */


#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Type for 32-bit registers
 */
#define REG32(ADDR) (*(uint32_t volatile (*))(ADDR))

/**
 * @brief   Type for 16-bit registers
 */
#define REG16(ADDR) (*(uint16_t volatile (*))(ADDR))

/**
 * @brief   Type for 16-bit memory addresses
 */
#define MEM16(ADDR) ((uint16_t(*))(ADDR))

/**
 * @brief    Screen dimension in pixel
 * @{
 */
#define GBA_SCREEN_WIDTH (240)  /**< width */
#define GBA_SCREEN_HEIGHT (160) /**< height */
/** @} */

/**
 * @brief Display Control register
 */
#define GBA_DISPCNT REG16(0x4000000)

/**
 * @name Settings for GBA_DISPCNT
 * @{
 */
#define GBA_DISPCNT_BGMODE_0 0x0000    /**< Tiled, four layers, up to 1024 tiles */
#define GBA_DISPCNT_BGMODE_1 0x0001    /**< Tiled, three layers, up to 1024 tiles */
#define GBA_DISPCNT_BGMODE_2 0x0002    /**< Tiled, two layers, up to 1024 tiles */
#define GBA_DISPCNT_BGMODE_3 0x0003    /**< Bitmap, 240x160, 15-bit RGB */
#define GBA_DISPCNT_BGMODE_4 0x0004    /**< Bitmap, 240x160, 8-bit RGB */
#define GBA_DISPCNT_BGMODE_5 0x0005    /**< Bitmap, 160x128, 15-bit RGB */

#define GBA_DISPCNT_SDBG_0 0x0100      /**< Enable display of background 0 */
#define GBA_DISPCNT_SDBG_1 0x0200      /**< Enable display of background 1 */
#define GBA_DISPCNT_SDBG_2 0x0400      /**< Enable display of background 2 */
#define GBA_DISPCNT_SDBG_3 0x0800      /**< Enable display of background 3 */

#define GBA_DISPCNT_SDOBJ 0x1000       /**< Enable display of objects */
/** @} */

/**
 * @brief General LCD Status register
 */
#define GBA_DISPSTAT REG16(0x4000004)

/**
 * @name LCD background control registers
 * @{
 */
#define GBA_BG0CNT REG16(0x4000008)    /**< BG0 */
#define GBA_BG1CNT REG16(0x400000a)    /**< BG1 */
#define GBA_BG2CNT REG16(0x400000c)    /**< BG2 */
#define GBA_BG3CNT REG16(0x400000e)    /**< BG3 */
/** @} */

/**
 * @brief VRAM color palette memory
 */
#define GBA_VRAM_PALETTE MEM16(0x5000000)

/**
 * @brief VRAM, 96Kb, 0x6017FFF - size depends on display mode!
 */
#define GBA_VRAM MEM16(0x6000000)

#ifdef __cplusplus
}
#endif

/** @} */
