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

#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type for 32-bit registers
 */
#define REG32(ADDR)            (*(uint32_t volatile(*))(ADDR))

/**
 * @brief   Type for 16-bit registers
 */
#define REG16(ADDR)            (*(uint16_t volatile(*))(ADDR))

/**
 * @brief   Type for 8-bit registers
 */
#define REG8(ADDR)            (*(uint8_t volatile(*))(ADDR))

/**
 * @brief   Type for 16-bit memory addresses
 */
#define MEM16(ADDR)            ((uint16_t(*))(ADDR))

/**
 * @brief   Pointer to our IRQ handler, must be in 32bit ARM code
 */
#define GBA_PTR_TO_IRQ_HANDLER REG32(0x3007ffc)

/**
 * @brief   Interrupt enable register
 *
 * Bit   Expl.
 * 0     LCD V-Blank
 * 1     LCD H-Blank
 * 2     LCD V-Counter Match
 * 3     Timer 0 Overflow
 * 4     Timer 1 Overflow
 * 5     Timer 2 Overflow
 * 6     Timer 3 Overflow
 * 7     Serial Communication
 * 8     DMA 0
 * 9     DMA 1
 * 10    DMA 2
 * 11    DMA 3
 * 12    Keypad
 * 13    Game Pak (external IRQ source)
 * 14-15 Not used
 */
#define GBA_IE                 REG16(0x4000200)

/**
 * @brief   Bit mask for the timer interrupts enable-flag
 */
#define GBA_IE_TIMERS          (0x0F << 3)

/**
 * @brief   Bit mask for the uart interrupt enable-flag
 */
#define GBA_IE_UART          (1 << 7)

/**
 * @brief   Interrupt flags register
 *
 * Bit   Expl.
 * 0     LCD V-Blank
 * 1     LCD H-Blank
 * 2     LCD V-Counter Match
 * 3     Timer 0 Overflow
 * 4     Timer 1 Overflow
 * 5     Timer 2 Overflow
 * 6     Timer 3 Overflow
 * 7     Serial Communication
 * 8     DMA 0
 * 9     DMA 1
 * 10    DMA 2
 * 11    DMA 3
 * 12    Keypad
 * 13    Game Pak (external IRQ source)
 * 14-15 Not used
 */
#define GBA_IF                 REG16(0x4000202)

/**
 * @brief   Bit mask to clear all pending interrupts
 */
#define GBA_IF_CLEAR_ALL       (0x3FFF)

/**
 * @brief   Bit mask for the timer interrupts has-fired-flag
 */
#define GBA_IF_TIMERS          (0x0F << 3)

/**
 * @brief   Bit mask for the uart/serial interrupt has-fired-flag
 */
#define GBA_IF_UART          (1 << 7)

/**
 * @brief   Interrupt Master Enable, globally enables / disables interrupts
 *
 * We usually keep this register enabled but use the "official" ARM IME register for our needs
 */
#define GBA_IME                REG16(0x4000208) /* Technically 32 bit reg, but only bit 0 is used */

/**
 * @name Timer control register
 * @{
 */
#define GBA_TM0_CNT_L          REG16(0x4000100) /**< current value */
#define GBA_TM0_CNT_H          REG16(0x4000102) /**< settings */
#define GBA_TM1_CNT_L          REG16(0x4000104) /**< current value */
#define GBA_TM1_CNT_H          REG16(0x4000106) /**< settings */
#define GBA_TM2_CNT_L          REG16(0x4000108) /**< current value */
#define GBA_TM2_CNT_H          REG16(0x400010A) /**< settings */
#define GBA_TM3_CNT_L          REG16(0x400010C) /**< current value */
#define GBA_TM3_CNT_H          REG16(0x400010E) /**< settings */
/** @} */

/**
 * @brief We have 16 bit timer
 */
#define GBA_TIMER_MAX_TICKS    (0xFFFF)

/**
 * @brief    Possible timer frequencies
 * @{
 */
#define GBA_TIMER_FRQ_16K      (16400lu)    /**< F = 16.4kHz */
#define GBA_TIMER_FRQ_65K      (65500lu)    /**< F = 65.5kHz */
#define GBA_TIMER_FRQ_262K     (262200lu)   /**< F = 262.2kHz */
#define GBA_TIMER_FRQ_16M      (16780000lu) /**< F = 16.78MHz */
/** @} */

/**
 * @brief Struct whose members map onto the timer register
 */
typedef struct {
    uint16_t current_value; /**< The counter / current value of this timer */
    uint16_t prescaler : 2; /**< The prescaler sets the divider for this timer */
    uint16_t count_up_on_neighbour_overflow : 1; /**< If set, counts up this timer when the previous
                                                    overflowed */
    uint16_t unused_3_5 : 3;                     /**< unused */
    uint16_t irq_enable : 1;  /**< enables interrupt request for this timer on overflow */
    uint16_t run : 1;         /**< starts the timer */
    uint16_t unused_8_15 : 8; /**< unused */
} gba_timer;
/* I don't trust bit-fields over all compilers & toolchains */
static_assert(sizeof(gba_timer) == 4, "2 x Timer register should be 4 bytes");

/**
 * @brief The type for the GBA timers
 */
typedef volatile gba_timer gba_timer_t;

/**
 * @brief    The four timers of the GBA
 * @{
 */
#define GBA_TIMER_0          ((gba_timer_t *)0x4000100) /**< A GBA timer */
#define GBA_TIMER_1          ((gba_timer_t *)0x4000104) /**< A GBA timer */
#define GBA_TIMER_2          ((gba_timer_t *)0x4000108) /**< A GBA timer */
#define GBA_TIMER_3          ((gba_timer_t *)0x400010C) /**< A GBA timer */
/** @} */

/**
 * @brief Serial IO control register
 */
#define GBA_SIO_SCNT REG16(0x4000128)

/**
 * @brief Serial IO 8bit data in/out
 */
#define GBA_SIO_DATA8 REG8(0x400012A)

/**
 * @brief Serial IO mode selection register
 */
#define GBA_SIO_RCNT REG16(0x4000134)

/**
 * @brief Struct whose members map onto the uart register
 */
struct gba_uart {
    uint16_t baud : 2;               /**< Baud rate, 0:9600, 38400, 57600, 3:115200 */
    uint16_t cts : 1;                /**< 0: Always Send, 1: Wait for SC=LOW */
    uint16_t parity : 1;             /**< 0: even, 1: odd */
    uint16_t send_data_full : 1;     /**< Send FIFO full */
    uint16_t receive_data_empty : 1; /**< Receive FIFO empty */
    uint16_t error : 1;              /**< Error occurred */
    uint16_t data_bits : 1;          /**< 0: 7 bits, 1: 8 bits */
    uint16_t fifo_enable : 1;        /**< Enable FIFO */
    uint16_t parity_enable : 1;      /**< Enable parity */
    uint16_t send_enable : 1;        /**< Enable sending data */
    uint16_t receive_enable : 1;     /**< Enable receiving data */
    uint16_t must_be_1_for_uart : 2; /**< Config bits, unrelated to UART configuration */
    uint16_t irq_enable : 1; /**< Enable IRQ on send_data_full, receive_data_empty and error */
    uint16_t unused_15 : 1;  /**< Unused */
};

/**
 * @brief The type for the GBA uart
 */
typedef struct gba_uart gba_uart_t;

/**
 * @brief The GBA uart (it's the SIO_SCNT register type casted)
 */
#define GBA_UART ((volatile gba_uart_t *) 0x4000128)

/**
 * @brief    Screen dimension in pixel
 * @{
 */
#define GBA_SCREEN_WIDTH     (240) /**< width */
#define GBA_SCREEN_HEIGHT    (160) /**< height */
/** @} */

/**
 * @brief Display Control register
 */
#define GBA_DISPCNT          REG16(0x4000000)

/**
 * @name Settings for GBA_DISPCNT
 * @{
 */
#define GBA_DISPCNT_BGMODE_0 0x0000 /**< Tiled, four layers, up to 1024 tiles */
#define GBA_DISPCNT_BGMODE_1 0x0001 /**< Tiled, three layers, up to 1024 tiles */
#define GBA_DISPCNT_BGMODE_2 0x0002 /**< Tiled, two layers, up to 1024 tiles */
#define GBA_DISPCNT_BGMODE_3 0x0003 /**< Bitmap, 240x160, 15-bit RGB */
#define GBA_DISPCNT_BGMODE_4 0x0004 /**< Bitmap, 240x160, 8-bit RGB */
#define GBA_DISPCNT_BGMODE_5 0x0005 /**< Bitmap, 160x128, 15-bit RGB */

#define GBA_DISPCNT_SDBG_0   0x0100 /**< Enable display of background 0 */
#define GBA_DISPCNT_SDBG_1   0x0200 /**< Enable display of background 1 */
#define GBA_DISPCNT_SDBG_2   0x0400 /**< Enable display of background 2 */
#define GBA_DISPCNT_SDBG_3   0x0800 /**< Enable display of background 3 */

#define GBA_DISPCNT_SDOBJ    0x1000 /**< Enable display of objects */
/** @} */

/**
 * @brief General LCD Status register
 */
#define GBA_DISPSTAT         REG16(0x4000004)

/**
 * @name LCD background control registers
 * @{
 */
#define GBA_BG0CNT           REG16(0x4000008) /**< BG0 */
#define GBA_BG1CNT           REG16(0x400000a) /**< BG1 */
#define GBA_BG2CNT           REG16(0x400000c) /**< BG2 */
#define GBA_BG3CNT           REG16(0x400000e) /**< BG3 */
/** @} */

/**
 * @brief VRAM color palette memory
 */
#define GBA_VRAM_PALETTE     MEM16(0x5000000)

/**
 * @brief VRAM, 96Kb, 0x6017FFF - size depends on display mode!
 */
#define GBA_VRAM             MEM16(0x6000000)

#ifdef __cplusplus
}
#endif

/** @} */
