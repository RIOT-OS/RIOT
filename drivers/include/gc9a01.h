/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_gc9a01 GC9A01 display driver
 * @ingroup     drivers_display
 * @brief       Device driver implementation for the GC9A01 display controller
 *
 * ## About
 * The GC9A01 is a single chip display driver for 240x240 TFT LCD displays.
 *
 * The driver communicates with the device either via an
 *
 * - SPI serial interface (if module `lcd_spi` enabled) or an
 * - MCU 8080 8-/16-bit parallel interface (if module `lcd_parallel` or
 *   module `lcd_parallel_16` is enabled).
 *
 * ## Usage
 * The driver can be configured by redefining the default parameters from the
 * `gc9a01_params.h` header. For new or custom boards, the best place for this
 * configuration is the board definition header `board.h`.
 * For example, to enable the global color inversion, you can add the following
 * line:
 *
 * ```c
 * #define GC9A01_PARAM_INVERTED 1
 * ```
 *
 * For applications using the display with e.g. a development board, you can
 * also define the parameters through `CFLAGS` in your application's `Makefile`.
 * For example, to set the Chip Select pin, you can add the following line:
 *
 * ```makefile
 * CFLAGS += -DGC9A01_PARAM_CS="GPIO_PIN(1,1)"
 * ```
 *
 * @note The SPI pins are defined by the selected SPI controller. The default
 *       is `SPI_DEV(0)`. You can find the according pin definitions for the
 *       SPI controller in your board's `periph_conf.h` header.
 *
 * After configuring the driver, it can be used
 * with the generic LCD API (@ref drivers_lcd). See `tests/drivers/gc9a01`
 * for an example application.
 *
 * @note Depending on the manufacturer, some panels require inversion to be enabled
 *       in order to display the colors properly. If colors show up inverted, try
 *       enabling it via @ref GC9A01_PARAM_INVERTED.
 *
 * @{
 *
 * @file
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

#include "lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    GC9A01 reference voltage configuration
 * @{
 */
/**
 * @brief GC9A01 VREG1A level.
 *
 * Default VREG1A voltage of 5.34V. VREG1A is the highest positive grayscale reference voltage.
 * VREG1A should be between 4.8 V and 7.34 V .
 */
#ifndef CONFIG_GC9A01_VREG1A
#  define CONFIG_GC9A01_VREG1A           (5340)
#endif

/**
 * @brief GC9A01 VREG2A level.
 *
 * Default VREG2A voltage of -3.98V. VREG1A is the lowest negative grayscale reference voltage.
 * VREG2A should be between -4.2 V and -1.66 V .
 */
#ifndef CONFIG_GC9A01_VREG2A
#  define CONFIG_GC9A01_VREG2A          (-3980)
#endif
/** @} */

/**
 * @name    GC9A01 display rotation modes
 * @{
 */
#define GC9A01_ROTATION_VERT           LCD_MADCTL_MX           /**< Vertical mode */
#define GC9A01_ROTATION_VERT_FLIP      LCD_MADCTL_MY           /**< Flipped vertical */
#define GC9A01_ROTATION_HORZ           LCD_MADCTL_MV           /**< Horizontal mode */
#define GC9A01_ROTATION_HORZ_FLIP      LCD_MADCTL_MV | \
        LCD_MADCTL_MY | \
        LCD_MADCTL_MX                                           /**< Horizontal flipped */
/** @} */

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /** Device initialization parameters */
    lcd_t dev;
} gc9a01_t;

/**
 * @brief   LCD device operations table
 */
extern const lcd_driver_t lcd_gc9a01_driver;

#ifdef __cplusplus
}
#endif

/** @} */
