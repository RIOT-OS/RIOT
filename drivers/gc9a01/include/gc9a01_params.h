/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_gc9a01
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include "board.h"
#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for GC9A01 display */
/**
 * @name    Set default configuration parameters for the GC9A01
 * @{
 */
#ifndef GC9A01_PARAM_SPI
/** SPI device */
#  define GC9A01_PARAM_SPI               SPI_DEV(0)
#endif
#ifndef GC9A01_PARAM_SPI_CLK
/** SPI clock frequency */
#  define GC9A01_PARAM_SPI_CLK           SPI_CLK_5MHZ
#endif
#ifndef GC9A01_PARAM_CS
/** Chip select pin */
#  define GC9A01_PARAM_CS                GPIO_PIN(2, 2)
#endif
#ifndef GC9A01_PARAM_DCX
/** DCX pin */
#  define GC9A01_PARAM_DCX               GPIO_PIN(3, 13)
#endif
#ifndef GC9A01_PARAM_RST
/** Reset pin */
#  define GC9A01_PARAM_RST               GPIO_UNDEF
#endif
#ifndef GC9A01_PARAM_SPI_MODE
/** SPI mode */
#  define GC9A01_PARAM_SPI_MODE          SPI_MODE_0
#endif
#ifndef GC9A01_PARAM_RGB
/** RGB mode enable */
#  define GC9A01_PARAM_RGB               0
#endif
#ifndef GC9A01_PARAM_INVERTED
/** Inverted mode enable */
#  define GC9A01_PARAM_INVERTED          0
#endif
#ifndef GC9A01_PARAM_NUM_LINES
/** Number of lines */
#  define GC9A01_PARAM_NUM_LINES         240U
#endif
#ifndef GC9A01_PARAM_RGB_CHANNELS
/** Number of RGB channels */
#  define GC9A01_PARAM_RGB_CHANNELS      240U
#endif
#ifndef GC9A01_PARAM_ROTATION
/** Rotation mode */
#  define GC9A01_PARAM_ROTATION          GC9A01_ROTATION_VERT
#endif
#ifndef GC9A01_PARAM_OFFSET_X
/** Horizontal offset */
#  define GC9A01_PARAM_OFFSET_X          0
#endif
#ifndef GC9A01_PARAM_OFFSET_Y
/** Vertical offset */
#  define GC9A01_PARAM_OFFSET_Y          0
#endif

#if MODULE_LCD_SPI || DOXYGEN
/** Default interface params if SPI serial interface is enabled */
#  define GC9A01_PARAM_IF_SPI        .spi = GC9A01_PARAM_SPI, \
        .spi_clk = GC9A01_PARAM_SPI_CLK, \
        .spi_mode = GC9A01_PARAM_SPI_MODE,
#else
#  define GC9A01_PARAM_IF_SPI
#endif

#if MODULE_LCD_PARALLEL || DOXYGEN
/** Default interface params if MCU 8080 8-bit parallel interface is enabled */
#  define GC9A01_PARAM_IF_PAR        .d0_pin = GC9A01_PARAM_D0, \
        .d1_pin = GC9A01_PARAM_D1, \
        .d2_pin = GC9A01_PARAM_D2, \
        .d3_pin = GC9A01_PARAM_D3, \
        .d4_pin = GC9A01_PARAM_D4, \
        .d5_pin = GC9A01_PARAM_D5, \
        .d6_pin = GC9A01_PARAM_D6, \
        .d7_pin = GC9A01_PARAM_D7, \
        .wrx_pin = GC9A01_PARAM_WRX, \
        .rdx_pin = GC9A01_PARAM_RDX,
#else
#  define GC9A01_PARAM_IF_PAR
#endif

#if MODULE_LCD_PARALLEL_16BIT || DOXYGEN
/** Additional default interface params if MCU 8080 16-bit parallel interface is enabled */
#  define GC9A01_PARAM_IF_PAR_16BIT  .d8_pin = GC9A01_PARAM_D8, \
        .d9_pin = GC9A01_PARAM_D9, \
        .d10_pin = GC9A01_PARAM_D10, \
        .d11_pin = GC9A01_PARAM_D11, \
        .d12_pin = GC9A01_PARAM_D12, \
        .d13_pin = GC9A01_PARAM_D13, \
        .d14_pin = GC9A01_PARAM_D14, \
        .d15_pin = GC9A01_PARAM_D15,
#else
#  define GC9A01_PARAM_IF_PAR_16BIT
#endif

#if MODULE_LCD_PARALLEL_16BIT || DOXYGEN
/** Interface mode is MCU 8080 16-bit parallel */
#  define GC9A01_PARAM_IF_MODE       .mode = LCD_IF_PARALLEL_16BIT,
#elif MODULE_LCD_PARALLEL
/** Interface mode is MCU 8080 8-bit parallel */
#  define GC9A01_PARAM_IF_MODE       .mode = LCD_IF_PARALLEL_8BIT,
#else
/** Interface mode parameter is not defined */
#  define GC9A01_PARAM_IF_MODE
#endif

/**
 * @brief   Default params
 *
 * @note The default parameter set defined here can only be used if a single
 *       GC9A01 display and only one interface mode is used. If multiple
 *       GC9A01 displays are used or if multiple interface modes are enabled
 *       by the modules `lcd_spi`, `lcd_parallel` and `lcd_parallel_16bit`, a user
 *       defined parameter set @ref GC9A01_PARAMS has to be defined. In the
 *       latter case @ref lcd_params_t::spi must then be set to @ref SPI_UNDEF
 *       for displays with MCU 8080 8-/16-bit parallel interfaces.
 */
#ifndef GC9A01_PARAMS
/** Configuration parameter set */
#  define GC9A01_PARAMS              { GC9A01_PARAM_IF_MODE \
                                        GC9A01_PARAM_IF_SPI \
                                        GC9A01_PARAM_IF_PAR \
                                        GC9A01_PARAM_IF_PAR_16BIT \
                                        .cs_pin = GC9A01_PARAM_CS, \
                                        .dcx_pin = GC9A01_PARAM_DCX, \
                                        .rst_pin = GC9A01_PARAM_RST, \
                                        .rgb = GC9A01_PARAM_RGB, \
                                        .inverted = GC9A01_PARAM_INVERTED, \
                                        .lines = GC9A01_PARAM_NUM_LINES, \
                                        .rgb_channels = GC9A01_PARAM_RGB_CHANNELS, \
                                        .rotation = GC9A01_PARAM_ROTATION, \
                                        .offset_x = GC9A01_PARAM_OFFSET_X, \
                                        .offset_y = GC9A01_PARAM_OFFSET_Y }
#endif
/** @} */

/**
 * @brief   Configure GC9A01
 */
static const lcd_params_t gc9a01_params[] =
{
    GC9A01_PARAMS,
};

/**
 * @brief   Default screen identifiers
 */
#ifndef GC9A01_PARAM_SCREEN_IDS
#  define GC9A01_PARAM_SCREEN_IDS    0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t gc9a01_screen_ids[] =
{
    GC9A01_PARAM_SCREEN_IDS,
};

/**
 * @brief   Define the number of configured displays
 */
#define GC9A01_NUMOF           ARRAY_SIZE(gc9a01_params)
/**
 * @brief   Define the number screens this display driver is attached to
 */
#define GC9A01_SCREEN_NUMOF    ARRAY_SIZE(gc9a01_screen_ids)

#ifdef __cplusplus
}
#endif

/** @} */
