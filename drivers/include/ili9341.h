/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg
 * SPDX-FileCopyrightText: 2021 Francisco Molina
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_ili9341 ILI9341 display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the ILI9341 display
 *
 * @{
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * The ILI9341 is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 */

#include "lcd.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ILI9341 gvdd level.
 *
 * Default GVDD voltage of 4.8V. GVDD is reference level for the VCOM level and
 * the grayscale voltage level. GVDD should be ≦ (AVDD - 0.5) V .
 */
#ifndef CONFIG_ILI9341_GVDD
#define CONFIG_ILI9341_GVDD             4800
#endif

/**
 * @brief ILI9341 VCOMH voltage level.
 *
 * Default VCOMH voltage of 4.25V. VCOMH represents the high level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering.
 */
#ifndef CONFIG_ILI9341_VCOMH
#define CONFIG_ILI9341_VCOMH            4250
#endif

/**
 * @brief ILI9341 VCOML voltage level.
 *
 * Default VCOML voltage of -2V. VCOML represents the low level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering
 */
#ifndef CONFIG_ILI9341_VCOML
#define CONFIG_ILI9341_VCOML            -2000
#endif

/**
 * @name    ILI9341 display rotation modes
 * @{
 */
#define ILI9341_ROTATION_VERT           LCD_MADCTL_MX       /**< Vertical mode */
#define ILI9341_ROTATION_VERT_FLIP      LCD_MADCTL_MY       /**< Flipped vertical */
#define ILI9341_ROTATION_HORZ           LCD_MADCTL_MV       /**< Horizontal mode */
#define ILI9341_ROTATION_HORZ_FLIP      LCD_MADCTL_MV | \
                                        LCD_MADCTL_MY | \
                                        LCD_MADCTL_MX       /**< Horizontal flipped */
/** @} */

/**
 * @brief   Device descriptor for a ili9341
 */
typedef struct {
    lcd_t dev;                    /**< Pointer to the common lcd device */
} ili9341_t;

/**
 * @brief   LCD device operations table
 */
extern const lcd_driver_t lcd_ili9341_driver;

#ifdef __cplusplus
}
#endif
/** @} */
