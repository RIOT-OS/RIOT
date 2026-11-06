/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_gc9a01
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include "lcd_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LCD GC9A01 commands
 *
 * LCD commands extension available for GC9A01 LCD controllers
 * @{
 */
#define GC9A01_REG_ID          (0x04)      /**< Display identification information */
#define GC9A01_REG_POWER7      (0xA7)      /**< Power Control 7 */
#define GC9A01_REG_POWER1      (0xC1)      /**< Power Control 1 */
#define GC9A01_REG_POWER2      (0xC3)      /**< Power Control 2 */
#define GC9A01_REG_POWER3      (0xC4)      /**< Power Control 3 */
#define GC9A01_REG_POWER4      (0xC9)      /**< Power Control 4 */
#define GC9A01_REG_FRAMERATE   (0xE8)      /**< Frame rate control */
#define GC9A01_REG_INREGEN1    (0xFE)      /**< Inter register enable 1 */
#define GC9A01_REG_INREGEN2    (0xEF)      /**< Inter register enable 2 */
#define GC9A01_REG_GAMMA1      (0xF0)      /**< Set gamma 1 */
#define GC9A01_REG_GAMMA2      (0xF1)      /**< Set gamma 2 */
#define GC9A01_REG_GAMMA3      (0xF2)      /**< Set gamma 3 */
#define GC9A01_REG_GAMMA4      (0xF3)      /**< Set gamma 4 */
/** @} */

/**
 * @name LCD GC9A01 values
 * @{
 */
/** COLMOD value for 16 bit pixel format */
#define GC9A01_16_BIT_FORMAT               (0x55)
/** Default value for VRH */
#define GC9A01_VRH_DEFAULT                 (0x28)
/** Framrate register value to enable 4 dot inversion */
#define GC9A01_FRAMERATE_4DOT_INVERSION    (0x34)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
