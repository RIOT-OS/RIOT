/*
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

#define GBA_IE REG16(0x4000200)
#define GBA_IE_TIMERS (0x0F << 3)

#define GBA_IF REG16(0x4000202)
#define GBA_IF_TIMERS (0x0F << 3)

#define GBA_IME REG16(0x4000208) /* Technically 32 bit reg, but only bit 0 is used */

#define GBA_PTR_TO_IRQ_HANDLER REG32(0x3007ffc) /* Pointer to user IRQ handler (32bit ARM code) */

#define GBA_TM0_CNT_L REG16(0x4000100)
#define GBA_TM0_CNT_H REG16(0x4000102)
#define GBA_TM1_CNT_L REG16(0x4000104)
#define GBA_TM1_CNT_H REG16(0x4000106)
#define GBA_TM2_CNT_L REG16(0x4000108)
#define GBA_TM2_CNT_H REG16(0x400010A)
#define GBA_TM3_CNT_L REG16(0x400010C)
#define GBA_TM3_CNT_H REG16(0x400010E)

#define GBA_TIMER_MAX_TICKS (0xFFFF)

typedef struct {
    uint16_t current_value;
    uint16_t prescaler : 2;
    uint16_t count_up_on_neighbour_overflow: 1;
    uint16_t unused_3_5 : 3;
    uint16_t irq_enable : 1;
    uint16_t run : 1;
    uint16_t unused_8_15 : 8;
} gba_timer;
/* I don't trust bit-fields over all compilers & toolchains */
_Static_assert(sizeof(gba_timer) == 4, "2 x Timer register should be 4 bytes");

typedef gba_timer volatile gba_timer_t;

#define GBA_TIMER_0 ((gba_timer_t volatile *) 0x4000100)
#define GBA_TIMER_1 ((gba_timer_t volatile *) 0x4000104)
#define GBA_TIMER_2 ((gba_timer_t volatile *) 0x4000108)
#define GBA_TIMER_3 ((gba_timer_t volatile *) 0x400010C)


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
