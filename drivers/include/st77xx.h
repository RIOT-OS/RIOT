/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_st77xx ST77xx display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for displays with a ST77xx display controller
 *
 * The ST77xx display controller driver is a driver for small RGB displays
 * using a ST77xx display controller. The driver supports ST7735, ST7789 and
 * ST7796. The support of the respective controller is enabled by the
 * corresponding module `st7735`, `st7789` or `st7796`. The ST77xx controller
 * used by a display must be specified in the board definition in parameter
 * @ref lcd_params_t::cntrl or as macro @ref ST77XX_PARAM_CNTRL if the
 * default parameter set @ref ST77XX_PARAMS is used.
 *
 * The driver uses the SPI serial interface to communicate with the display
 * controller.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ST77XX_H
#define ST77XX_H

#include "lcd.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ST77xx display rotation modes
 * @{
 */
#define ST77XX_ROTATION_VERT            0                   /**< Vertical mode */
#define ST77XX_ROTATION_VERT_FLIP       LCD_MADCTL_MX | \
                                        LCD_MADCTL_MY       /**< Flipped vertical */
#define ST77XX_ROTATION_HORZ            LCD_MADCTL_MV | \
                                        LCD_MADCTL_MX       /**< Horizontal mode */
#define ST77XX_ROTATION_HORZ_FLIP       LCD_MADCTL_MV | \
                                        LCD_MADCTL_MY       /**< Horizontal flipped */
/** @} */

/**
 * @brief    ST77xx display controller variants
 */
enum {
    ST77XX_CNTRL_ST7735,
    ST77XX_CNTRL_ST7789,
    ST77XX_CNTRL_ST7796,
};

/**
 * @brief   Device descriptor for a ST77xx display
 *
 */
typedef struct {
    lcd_t dev;                    /**< Pointer to the common lcd device */
} st77xx_t;

/**
 * @brief   LCD device operations table
 */
extern const lcd_driver_t lcd_st77xx_driver;

#ifdef __cplusplus
}
#endif
#endif /* ST77XX_H */
/** @} */
