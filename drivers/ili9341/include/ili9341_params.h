/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ili9341
 *
 * @{
 * @file
 * @brief       Default configuration for ili9341
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "board.h"
#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for ILI9341 display */
/**
 * @name    Set default configuration parameters for the ILI9341
 * @{
 */
#ifndef ILI9341_PARAM_SPI
#define ILI9341_PARAM_SPI          SPI_DEV(0)
#endif
#ifndef ILI9341_PARAM_SPI_CLK
#define ILI9341_PARAM_SPI_CLK      SPI_CLK_5MHZ
#endif
#ifndef ILI9341_PARAM_CS
#define ILI9341_PARAM_CS           GPIO_PIN(2, 2)
#endif
#ifndef ILI9341_PARAM_DCX
#define ILI9341_PARAM_DCX          GPIO_PIN(3, 13)
#endif
#ifndef ILI9341_PARAM_RST
#define ILI9341_PARAM_RST          GPIO_UNDEF
#endif
#ifndef ILI9341_PARAM_SPI_MODE
#define ILI9341_PARAM_SPI_MODE     SPI_MODE_0
#endif
#ifndef ILI9341_PARAM_RGB
#define ILI9341_PARAM_RGB          0
#endif
#ifndef ILI9341_PARAM_INVERTED
#define ILI9341_PARAM_INVERTED     0
#endif
#ifndef ILI9341_PARAM_NUM_LINES
#define ILI9341_PARAM_NUM_LINES         320U    /**< Number of lines */
#endif
#ifndef ILI9341_PARAM_RGB_CHANNELS
#define ILI9341_PARAM_RGB_CHANNELS      240U    /**< Number of RGB channels (e.g. columns) */
#endif
#ifndef ILI9341_PARAM_ROTATION
#define ILI9341_PARAM_ROTATION          ILI9341_ROTATION_HORZ_FLIP
#endif
#ifndef ILI9341_PARAM_OFFSET_X
#define ILI9341_PARAM_OFFSET_X          0               /**< Horizontal offset */
#endif
#ifndef ILI9341_PARAM_OFFSET_Y
#define ILI9341_PARAM_OFFSET_Y          0               /**< Vertival offset */
#endif

#if MODULE_LCD_SPI || DOXYGEN
/** Default interface params if SPI serial interface is enabled */
#define ILI9341_PARAM_IF_SPI        .spi = ILI9341_PARAM_SPI, \
                                    .spi_clk = ILI9341_PARAM_SPI_CLK, \
                                    .spi_mode = ILI9341_PARAM_SPI_MODE,
#else
#define ILI9341_PARAM_IF_SPI
#endif

#if MODULE_LCD_PARALLEL || DOXYGEN
/** Default interface params if MCU 8080 8-bit parallel interface is enabled */
#define ILI9341_PARAM_IF_PAR        .d0_pin = ILI9341_PARAM_D0, \
                                    .d1_pin = ILI9341_PARAM_D1, \
                                    .d2_pin = ILI9341_PARAM_D2, \
                                    .d3_pin = ILI9341_PARAM_D3, \
                                    .d4_pin = ILI9341_PARAM_D4, \
                                    .d5_pin = ILI9341_PARAM_D5, \
                                    .d6_pin = ILI9341_PARAM_D6, \
                                    .d7_pin = ILI9341_PARAM_D7, \
                                    .wrx_pin = ILI9341_PARAM_WRX, \
                                    .rdx_pin = ILI9341_PARAM_RDX,
#else
#define ILI9341_PARAM_IF_PAR
#endif

#if MODULE_LCD_PARALLEL_16BIT || DOXYGEN
/** Additional default interface params if MCU 8080 16-bit parallel interface is enabled */
#define ILI9341_PARAM_IF_PAR_16BIT  .d8_pin = ILI9341_PARAM_D8, \
                                    .d9_pin = ILI9341_PARAM_D9, \
                                    .d10_pin = ILI9341_PARAM_D10, \
                                    .d11_pin = ILI9341_PARAM_D11, \
                                    .d12_pin = ILI9341_PARAM_D12, \
                                    .d13_pin = ILI9341_PARAM_D13, \
                                    .d14_pin = ILI9341_PARAM_D14, \
                                    .d15_pin = ILI9341_PARAM_D15,
#else
#define ILI9341_PARAM_IF_PAR_16BIT
#endif

#if MODULE_LCD_PARALLEL_16BIT || DOXYGEN
/** Interface mode is MCU 8080 16-bit parallel */
#define ILI9341_PARAM_IF_MODE       .mode = LCD_IF_PARALLEL_16BIT,
#elif MODULE_LCD_PARALLEL
/** Interface mode is MCU 8080 8-bit parallel */
#define ILI9341_PARAM_IF_MODE       .mode = LCD_IF_PARALLEL_8BIT,
#else
/** Interface mode parameter is not defined */
#define ILI9341_PARAM_IF_MODE
#endif

/**
 * @brief   Default params
 *
 * @note The default parameter set defined here can only be used if a single
 *       ILI9341 display and only one interface mode is used. If multiple
 *       ILI9341 displays are used or if multiple interface modes are enabled
 *       by the modules `lcd_spi`, lcd_parallel and `lcd_parallel_16bit`, a user
 *       defined parameter set @ref ILI9341_PARAMS has to be defined. In the
 *       latter case @ref lcd_params_t::spi must then be set to @ref SPI_UNDEF
 *       for displays with MCU 8080 8-/16-bit parallel interfaces.
 */
#ifndef ILI9341_PARAMS
#define ILI9341_PARAMS              { ILI9341_PARAM_IF_MODE \
                                      ILI9341_PARAM_IF_SPI \
                                      ILI9341_PARAM_IF_PAR \
                                      ILI9341_PARAM_IF_PAR_16BIT \
                                      .cs_pin = ILI9341_PARAM_CS, \
                                      .dcx_pin = ILI9341_PARAM_DCX, \
                                      .rst_pin = ILI9341_PARAM_RST, \
                                      .rgb = ILI9341_PARAM_RGB, \
                                      .inverted = ILI9341_PARAM_INVERTED, \
                                      .lines = ILI9341_PARAM_NUM_LINES, \
                                      .rgb_channels = ILI9341_PARAM_RGB_CHANNELS, \
                                      .rotation = ILI9341_PARAM_ROTATION, \
                                      .offset_x = ILI9341_PARAM_OFFSET_X, \
                                      .offset_y = ILI9341_PARAM_OFFSET_Y, \
}
#endif
/** @} */

/**
 * @brief   Configure ILI9341
 */
static const lcd_params_t ili9341_params[] =
{
    ILI9341_PARAMS,
};

/**
 * @brief   Default screen identifiers
 */
#ifndef ILI9341_PARAM_SCREEN_IDS
#define ILI9341_PARAM_SCREEN_IDS    0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t ili9341_screen_ids[] =
{
    ILI9341_PARAM_SCREEN_IDS,
};

/**
 * @brief   Define the number of configured displays
 */
#define ILI9341_NUMOF           ARRAY_SIZE(ili9341_params)
/**
 * @brief   Define the number screens this display driver is attached to
 */
#define ILI9341_SCREEN_NUMOF    ARRAY_SIZE(ili9341_screen_ids)

#ifdef __cplusplus
}
#endif

/** @} */
